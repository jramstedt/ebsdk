/********************************************************************************************************************************/
/* Created 20-FEB-1991 09:23:27 by VAX SDL V3.2-12     Source:  1-DEC-1989 14:03:53 CLOUD_USER:[SHIRRON.V5CP.SRC]F11DEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $FH1DEF ***/
#define FH1$C_LEVEL1 257                /* 401 octal = structure level 1    */
#define FH1$K_LENGTH 46                 /* length of header area            */
#define FH1$C_LENGTH 46                 /* length of header area            */
struct FH1DEF {
    unsigned char FH1$B_IDOFFSET;       /* ident area offset in words       */
    unsigned char FH1$B_MPOFFSET;       /* map area offset in words         */
    variant_union  {
        unsigned short int FH1$W_FID [2]; /* file ID                        */
        variant_struct  {
            unsigned short int FH1$W_FID_NUM; /* file number                */
            unsigned short int FH1$W_FID_SEQ; /* file sequence number       */
            } FH1$R_FID_FIELDS;
        } FH1$R_FID_OVERLAY;
    unsigned short int FH1$W_STRUCLEV;  /* file structure level             */
    variant_union  {
        unsigned short int FH1$W_FILEOWNER; /* file owner UIC               */
        variant_struct  {
            unsigned char FH1$B_UICMEMBER; /* UIC member number             */
            unsigned char FH1$B_UICGROUP; /* UIC group number               */
            } FH1$R_FILEOWNER_FIELDS;
        } FH1$R_FILEOWNER_OVERLAY;
    variant_union  {
        unsigned short int FH1$W_FILEPROT; /* file protection               */
        variant_struct  {
            unsigned FH1$V_SYSPRO : 4;  /* system protection                */
            unsigned FH1$V_OWNPRO : 4;  /* owner protection                 */
            unsigned FH1$V_GROUPPRO : 4; /* group protection                */
            unsigned FH1$V_WORLDPRO : 4; /* world protection                */
            } FH1$R_FILEPROT_BITS;
        } FH1$R_FILEPROT_OVERLAY;
    variant_union  {
        unsigned short int FH1$W_FILECHAR; /* file characteristics          */
        variant_struct  {
            variant_union  {
                unsigned char FH1$B_USERCHAR; /* user controlled characteristics  */
                variant_struct  {
                    unsigned FH1$V_WASCONTIG : 1; /* file was (and should be) contiguous */
                    unsigned FH1$V_NOBACKUP : 1; /* file is not to be backed up  */
                    unsigned FH1DEF$$_FILL_2 : 1; /* reserved               */
                    unsigned FH1$V_READCHECK : 1; /* verify all read operations  */
                    unsigned FH1$V_WRITCHECK : 1; /* verify all write operations  */
                    unsigned FH1$V_CONTIGB : 1; /* keep file as contiguous as possible  */
                    unsigned FH1$V_LOCKED : 1; /* file is deaccess locked   */
                    unsigned FH1$V_CONTIG : 1; /* file is contiguous        */
                    } FH1$R_USERCHAR_BITS;
                } FH1$R_USERCHAR_OVERLAY;
            variant_union  {
                unsigned char FH1$B_SYSCHAR; /* system controlled characteristics  */
                variant_struct  {
                    unsigned FH1DEF$$_FILL_3 : 4; /* reserved               */
                    unsigned FH1$V_SPOOL : 1; /* intermediate spool file    */
                    unsigned FH1DEF$$_FILL_4 : 1; /* reserved               */
                    unsigned FH1$V_BADBLOCK : 1; /* file contains bad blocks  */
                    unsigned FH1$V_MARKDEL : 1; /* file is marked for delete  */
                    } FH1$R_SYSCHAR_BITS;
                } FH1$R_SYSCHAR_OVERLAY;
            } FH1$R_FILECHAR_FIELDS;
        } FH1$R_FILECHAR_OVERLAY;
    unsigned short int FH1$W_RECATTR [16]; /* file record attributes        */
    short int FH1DEF$$_FILL_5 [232];    /* rest of file header              */
    unsigned short int FH1$W_CHECKSUM;  /* file header checksum             */
    } ;
 
/*** MODULE $FI1DEF ***/
#define FI1$K_LENGTH 46                 /* length of ident area             */
#define FI1$C_LENGTH 46                 /* length of ident area             */
struct FI1DEF {
    unsigned short int FI1$W_FILENAME [3]; /* file name (RAD-50)            */
    unsigned short int FI1$W_FILETYPE;  /* file type (RAD-50)               */
    unsigned short int FI1$W_VERSION;   /* version number (binary)          */
    unsigned short int FI1$W_REVISION;  /* revision number (binary)         */
    char FI1$T_REVDATE [7];             /* revision date (ASCII DDMMMYY)    */
    char FI1$T_REVTIME [6];             /* revision time (ASCII HHMMSS)     */
    char FI1$T_CREDATE [7];             /* creation date (ASCII DDMMMYY)    */
    char FI1$T_CRETIME [6];             /* creation time (ASCII HHMMSS)     */
    char FI1$T_EXPDATE [7];             /* expiration date (ASCII DDMMMYY)  */
    char FI1DEF$$_FILL_1;               /* dummy byte to round up           */
    char FI1$T_MTHDR1 [80];             /* HDR1 of ANSI magnetic tape file  */
    char FI1$T_MTHDR2 [80];             /* HDR2 of ANSI magnetic tape file  */
    char FI1$T_MTHDR3 [80];             /* HDR3 of ANSI magnetic tape file  */
    } ;
 
/*** MODULE $FM1DEF ***/
#define FM1$K_POINTERS 10               /* start of retrieval pointers      */
#define FM1$C_POINTERS 10               /* start of retrieval pointers      */
#define FM1$K_LENGTH 10                 /* length of map area               */
#define FM1$C_LENGTH 10                 /* length of map area               */
/* retrieval pointer format                                                 */
struct FM1DEF {
    unsigned char FM1$B_EX_SEGNUM;      /* extension segment number of this header  */
    unsigned char FM1$B_EX_RVN;         /* extension relative volume number  */
    unsigned short int FM1$W_EX_FILNUM; /* extension file number            */
    unsigned short int FM1$W_EX_FILSEQ; /* extension file sequence number   */
    unsigned char FM1$B_COUNTSIZE;      /* retrieval pointer count field size  */
    unsigned char FM1$B_LBNSIZE;        /* retrieval pointer LBN field size  */
    unsigned char FM1$B_INUSE;          /* number of retrieval words in use  */
    unsigned char FM1$B_AVAIL;          /* number of retrieval words available  */
    } ;
struct FM1DEF1 {
    unsigned char FM1$B_HIGHLBN;        /* high order LBN                   */
    unsigned char FM1$B_COUNT;          /* block count                      */
    unsigned short int FM1$W_LOWLBN;    /* low order LBN                    */
    } ;
struct FM1DEF2 { /* WARNING: aggregate has origin of -4 */
    unsigned char FM1$B_PREVHLBN;
    unsigned char FM1$B_PREVCOUNT;
    unsigned short int FM1$W_PREVLLBN;  /* previous retrieval pointer       */
    char FM1DEF$$_FILL_1;
    } ;
 
/*** MODULE $FH2DEF ***/
/*+                                                                         */
/*                                                                          */
/* File header definitions for Files-11 Structure Level 2                   */
/*                                                                          */
/*-                                                                         */
#define FH2$C_LEVEL1 257                /* 401 octal = structure level 1    */
#define FH2$C_LEVEL2 512                /* 1000 octal = structure level 2   */
#define FH2$M_VCC_STATE 1792
#define FH2$M_ALM_STATE 1835008
#define FH2$M_WASCONTIG 1
#define FH2$M_NOBACKUP 2
#define FH2$M_WRITEBACK 4
#define FH2$M_READCHECK 8
#define FH2$M_WRITCHECK 16
#define FH2$M_CONTIGB 32
#define FH2$M_LOCKED 64
#define FH2$M_CONTIG 128
#define FH2$M_BADACL 2048
#define FH2$M_SPOOL 4096
#define FH2$M_DIRECTORY 8192
#define FH2$M_BADBLOCK 16384
#define FH2$M_MARKDEL 32768
#define FH2$M_NOCHARGE 65536
#define FH2$M_ERASE 131072
#define FH2$M_ALM_AIP 262144
#define FH2$M_ALM_ARCHIVED 524288
#define FH2$M_ALM_DELETED 1048576
#define FH2$M_ONLY_RU 1
#define FH2$M_RUJNL 2
#define FH2$M_BIJNL 4
#define FH2$M_AIJNL 8
#define FH2$M_ATJNL 16
#define FH2$M_NEVER_RU 32
#define FH2$M_JOURNAL_FILE 64
#define FH2$C_RU_FACILITY_RMS 1         /* RMS                              */
#define FH2$C_RU_FACILITY_DBMS 2        /* DBMS                             */
#define FH2$C_RU_FACILITY_RDB 3         /* Rdb/VMS                          */
#define FH2$C_RU_FACILITY_CHKPNT 4      /* Checkpoint/Restart               */
#define FH2$K_LENGTH 80                 /* length of header area            */
#define FH2$C_LENGTH 80                 /* length of header area            */
#define FH2$K_SUBSET0_LENGTH 88         /* length of header area            */
#define FH2$C_SUBSET0_LENGTH 88         /* length of header area            */
#define FH2$K_FULL_LENGTH 108           /* length of full header            */
#define FH2$C_FULL_LENGTH 108           /* length of full header            */
struct FH2DEF {
    unsigned char FH2$B_IDOFFSET;       /* ident area offset in words       */
    unsigned char FH2$B_MPOFFSET;       /* map area offset in words         */
    unsigned char FH2$B_ACOFFSET;       /* access control list offset in words  */
    unsigned char FH2$B_RSOFFSET;       /* reserved area offset in words    */
    unsigned short int FH2$W_SEG_NUM;   /* file segment number              */
    variant_union  {
        unsigned short int FH2$W_STRUCLEV; /* file structure level          */
        variant_struct  {
            unsigned char FH2$B_STRUCVER; /* file structure version         */
            unsigned char FH2$B_STRUCLEV; /* principal file structure level  */
            } FH2$R_STRUCLEV_FIELDS;
        } FH2$R_STRUCLEV_OVERLAY;
    variant_union  {
        unsigned short int FH2$W_FID [3]; /* file ID                        */
        variant_struct  {
            unsigned short int FH2$W_FID_NUM; /* file number                */
            unsigned short int FH2$W_FID_SEQ; /* file sequence number       */
            variant_union  {
                unsigned short int FH2$W_FID_RVN; /* relative volume number  */
                variant_struct  {
                    unsigned char FH2$B_FID_RVN; /* alternate format RVN    */
                    unsigned char FH2$B_FID_NMX; /* alternate format file number extension  */
                    } FH2$R_FID_RVN_FIELDS;
                } FH2$R_FID_RVN_OVERLAY;
            } FH2$R_FID_FIELDS;
        } FH2$R_FID_OVERLAY;
    variant_union  {
        unsigned short int FH2$W_EXT_FID [3]; /* extension file ID          */
        variant_struct  {
            unsigned short int FH2$W_EX_FIDNUM; /* extension file number    */
            unsigned short int FH2$W_EX_FIDSEQ; /* extension file sequence number  */
            variant_union  {
                unsigned short int FH2$W_EX_FIDRVN; /* extension relative volume number  */
                variant_struct  {
                    unsigned char FH2$B_EX_FIDRVN; /* alternate format extension RVN  */
                    unsigned char FH2$B_EX_FIDNMX; /* alternate format extension file number extension  */
                    } FH2$R_EX_FIDRVN_FIELDS;
                } FH2$R_EX_FIDRVN_OVERLAY;
            } FH2$R_EXT_FID_FIELDS;
        } FH2$R_EXT_FID_OVERLAY;
    unsigned short int FH2$W_RECATTR [16]; /* file record attributes        */
    variant_union  {
        unsigned long int FH2$L_FILECHAR; /* file characteristics           */
        variant_struct  {
            unsigned FH2DEF$$_FILL_21 : 8; /* reserved                      */
            unsigned FH2$V_VCC_STATE : 3; /* VCC state bits                 */
            unsigned FH2DEF$$_FILL_22 : 7; /* reserved                      */
            unsigned FH2$V_ALM_STATE : 3; /* ALM state bits                 */
            unsigned FH2$V_fill_0 : 3;
            } FH2$R_FILECHAR_CHUNKS;
        variant_struct  {
            unsigned FH2$V_WASCONTIG : 1; /* file was (and should be) contiguous */
            unsigned FH2$V_NOBACKUP : 1; /* file is not to be backed up     */
            unsigned FH2$V_WRITEBACK : 1; /* file may be write-back cached  */
            unsigned FH2$V_READCHECK : 1; /* verify all read operations     */
            unsigned FH2$V_WRITCHECK : 1; /* verify all write operations    */
            unsigned FH2$V_CONTIGB : 1; /* keep file as contiguous as possible  */
            unsigned FH2$V_LOCKED : 1;  /* file is deaccess locked          */
            unsigned FH2$V_CONTIG : 1;  /* file is contiguous               */
            unsigned FH2DEF$$_FILL_2 : 3; /* reserved                       */
            unsigned FH2$V_BADACL : 1;  /* ACL is invalid                   */
            unsigned FH2$V_SPOOL : 1;   /* intermediate spool file          */
            unsigned FH2$V_DIRECTORY : 1; /* file is a directory            */
            unsigned FH2$V_BADBLOCK : 1; /* file contains bad blocks        */
            unsigned FH2$V_MARKDEL : 1; /* file is marked for delete        */
            unsigned FH2$V_NOCHARGE : 1; /* file space is not to be charged  */
            unsigned FH2$V_ERASE : 1;   /* erase file contents before deletion  */
            unsigned FH2$V_ALM_AIP : 1; /* Archive in progress              */
            unsigned FH2$V_ALM_ARCHIVED : 1; /* File archived               */
            unsigned FH2$V_ALM_DELETED : 1; /* File contents deleted        */
/* Note: The high 8 bits of this longword                                   */
/* are reserved for user and CSS use.                                       */
            unsigned FH2$V_fill_1 : 3;
            } FH2$R_FILECHAR_BITS;
        } FH2$R_FILECHAR_OVERLAY;
    unsigned short int FH2$W_RECPROT;   /* record protection                */
    unsigned char FH2$B_MAP_INUSE;      /* number of map area words in use  */
    unsigned char FH2$B_ACC_MODE;       /* least privileged access mode     */
    variant_union  {
        unsigned long int FH2$L_FILEOWNER; /* file owner UIC                */
        variant_struct  {
            unsigned short int FH2$W_UICMEMBER; /* UIC member number        */
            unsigned short int FH2$W_UICGROUP; /* UIC group number          */
            } FH2$R_FILEOWNER_FIELDS;
        } FH2$R_FILEOWNER_OVERLAY;
    unsigned short int FH2$W_FILEPROT;  /* file protection                  */
    variant_union  {
        unsigned short int FH2$W_BACKLINK [3]; /* back link pointer         */
        variant_struct  {
            unsigned short int FH2$W_BK_FIDNUM; /* back link file number    */
            unsigned short int FH2$W_BK_FIDSEQ; /* back link file sequence number  */
            variant_union  {
                unsigned short int FH2$W_BK_FIDRVN; /* back link relative volume number  */
                variant_struct  {
                    unsigned char FH2$B_BK_FIDRVN; /* alternate format back link RVN  */
                    unsigned char FH2$B_BK_FIDNMX; /* alternate format back link file number extension  */
                    } FH2$R_BK_FIDRVN_FIELDS;
                } FH2$R_BK_FIDRVN_OVERLAY;
            } FH2$R_BACKLINK_FIELDS;
        } FH2$R_BACKLINK_OVERLAY;
    variant_union  {
        unsigned char FH2$B_JOURNAL;    /* journal control flags            */
        variant_struct  {
            unsigned FH2$V_ONLY_RU : 1; /* file is accessible only in recovery unit  */
            unsigned FH2$V_RUJNL : 1;   /* enable recovery unit journal     */
            unsigned FH2$V_BIJNL : 1;   /* enable before image journal      */
            unsigned FH2$V_AIJNL : 1;   /* enable after image journal       */
            unsigned FH2$V_ATJNL : 1;   /* enable audit trail journal       */
            unsigned FH2$V_NEVER_RU : 1; /* file is never accessible in recovery unit */
            unsigned FH2$V_JOURNAL_FILE : 1; /* this is a journal file      */
            unsigned FH2$V_fill_2 : 1;
            } FH2$R_JOURNAL_BITS;
        } FH2$R_JOURNAL_OVERLAY;
    unsigned char FH2$B_RU_ACTIVE;      /* If non-zero, file has active recovery units */
/* (value is recoverable facility id number)                                */
/* 1-99 reserved to DEC, 100-127 reserved for                               */
/* CSS, 128-255 reserved for customers.                                     */
    short int FH2DEF$$_FILL_3;          /* reserved                         */
    unsigned long int FH2$L_HIGHWATER;  /* high-water mark in file          */
    long int FH2$L_FILL_6 [2];          /* reserved                         */
    variant_struct  {                           /* security classification mask     */
        char FH2$B_FILL_5 [20];         /* see structure in $CLSDEF         */
        } FH2$R_CLASS_PROT;
    char FH2DEF$$_FILL_4 [402];         /* rest of file header              */
    unsigned short int FH2$W_CHECKSUM;  /* file header checksum             */
    } ;
 
/*** MODULE $FI2DEF ***/
#define FI2$K_LENGTH 120                /* length of ident area             */
#define FI2$C_LENGTH 120                /* length of ident area             */
struct FI2DEF {
    char FI2$T_FILENAME [20];           /* file name, type, and version (ASCII)  */
    unsigned short int FI2$W_REVISION;  /* revision number (binary)         */
    unsigned int FI2$Q_CREDATE [2];     /* creation date and time           */
    unsigned int FI2$Q_REVDATE [2];     /* revision date and time           */
    unsigned int FI2$Q_EXPDATE [2];     /* expiration date and time         */
    unsigned int FI2$Q_BAKDATE [2];     /* backup date and time             */
    char FI2$T_FILENAMEXT [66];         /* extension file name area         */
    char FI2$T_USERLABEL [80];          /* optional user file label         */
    } ;
 
/*** MODULE $FM2DEF ***/
/* retrieval pointer type codes                                             */
#define FM2$C_PLACEMENT 0               /* 00 = placement control data      */
#define FM2$C_FORMAT1 1                 /* 01 = format 1                    */
#define FM2$C_FORMAT2 2                 /* 10 = format 2                    */
#define FM2$C_FORMAT3 3                 /* 11 = format 3                    */
/* format of retrieval pointer                                              */
#define FM2$K_LENGTH0 2                 /* length of format 0 (placement)   */
#define FM2$C_LENGTH0 2                 /* length of format 0 (placement)   */
#define FM2$K_LENGTH1 4                 /* length of format 1               */
#define FM2$C_LENGTH1 4                 /* length of format 1               */
struct FM2DEF {
    variant_union  {
        unsigned short int FM2$W_WORD0; /* first word, of many uses         */
        variant_struct  {
            unsigned FM2DEF$$_FILL_1 : 14; /* type specific data            */
            unsigned FM2$V_FORMAT : 2;  /* format type code                 */
            } FM2$R_WORD0_BITS0;
        variant_struct  {
            unsigned FM2$V_EXACT : 1;   /* exact placement specified        */
            unsigned FM2$V_ONCYL : 1;   /* on cylinder allocation desired   */
            unsigned FM2DEF$$_FILL_2 : 10;
            unsigned FM2$V_LBN : 1;     /* use LBN of next map pointer      */
            unsigned FM2$V_RVN : 1;     /* place on specified RVN           */
            unsigned FM2$V_fill_3 : 2;
            } FM2$R_WORD0_BITS1;
        variant_struct  {
            unsigned FM2DEF$$_FILL_3 : 8; /* low byte described below       */
            unsigned FM2$V_HIGHLBN : 6; /* high order LBN                   */
            unsigned FM2$V_fill_4 : 2;
            } FM2$R_WORD0_BITS2;
        variant_struct  {
            unsigned FM2$V_COUNT2 : 14; /* format 2 & 3 count field         */
            unsigned FM2$V_fill_5 : 2;
            } FM2$R_WORD0_BITS3;
        unsigned char FM2$B_COUNT1;     /* format 1 count field             */
        } FM2$R_WORD0_OVERLAY;
    unsigned short int FM2$W_LOWLBN;    /* format 1 low order LBN           */
    } ;
#define FM2$K_LENGTH2 6                 /* length of format 2               */
#define FM2$C_LENGTH2 6                 /* length of format 2               */
struct FM2DEF1 {
    char FM2DEF$$_FILL_4 [2];
    unsigned long int FM2$L_LBN2;       /* format 2 LBN (longword)          */
    } ;
#define FM2$K_LENGTH3 8                 /* length of format 3               */
#define FM2$C_LENGTH3 8                 /* length of format 3               */
struct FM2DEF2 {
    char FM2DEF$$_FILL_5 [2];
    unsigned short int FM2$W_LOWCOUNT;  /* format 3 low order count         */
    unsigned long int FM2$L_LBN3;       /* format 3 LBN (longword)          */
    } ;
 
/*** MODULE $FCHDEF ***/
/*+                                                                         */
/*                                                                          */
/* File characteristics bit definitions. These are identical to, and must   */
/* track, the bits in FILECHAR above, but are defined relative to the file  */
/* characteristics longword instead of relative to the file header.         */
/*                                                                          */
/*-                                                                         */
#define FCH$M_VCC_STATE 1792
#define FCH$M_ALM_STATE 1835008
#define FCH$M_WASCONTIG 1
#define FCH$M_NOBACKUP 2
#define FCH$M_WRITEBACK 4
#define FCH$M_READCHECK 8
#define FCH$M_WRITCHECK 16
#define FCH$M_CONTIGB 32
#define FCH$M_LOCKED 64
#define FCH$M_CONTIG 128
#define FCH$M_BADACL 2048
#define FCH$M_SPOOL 4096
#define FCH$M_DIRECTORY 8192
#define FCH$M_BADBLOCK 16384
#define FCH$M_MARKDEL 32768
#define FCH$M_NOCHARGE 65536
#define FCH$M_ERASE 131072
#define FCH$M_ALM_AIP 262144
#define FCH$M_ALM_ARCHIVED 524288
#define FCH$M_ALM_DELETED 1048576
union FCHDEF {
    long int FCHDEF$$_FILL_1;
    variant_struct  {
        unsigned FCHDEF$$_FILL_31 : 8;  /* reserved                         */
        unsigned FCH$V_VCC_STATE : 3;   /* VCC state bits                   */
        unsigned FCHDEF$$_FILL_32 : 7;  /* reserved                         */
        unsigned FCH$V_ALM_STATE : 3;   /* ALM state bits                   */
        unsigned FCH$V_fill_6 : 3;
        } FCH$R_FILL_1_CHUNKS;
    variant_struct  {
        unsigned FCH$V_WASCONTIG : 1;   /* file was (and should be) contiguous */
        unsigned FCH$V_NOBACKUP : 1;    /* file is not to be backed up      */
        unsigned FCH$V_WRITEBACK : 1;   /* file may be write-back cached    */
        unsigned FCH$V_READCHECK : 1;   /* verify all read operations       */
        unsigned FCH$V_WRITCHECK : 1;   /* verify all write operations      */
        unsigned FCH$V_CONTIGB : 1;     /* keep file as contiguous as possible  */
        unsigned FCH$V_LOCKED : 1;      /* file is deaccess locked          */
        unsigned FCH$V_CONTIG : 1;      /* file is contiguous               */
        unsigned FCHDEF$$_FILL_3 : 3;   /* reserved                         */
        unsigned FCH$V_BADACL : 1;      /* ACL is invalid                   */
        unsigned FCH$V_SPOOL : 1;       /* intermediate spool file          */
        unsigned FCH$V_DIRECTORY : 1;   /* file is a directory              */
        unsigned FCH$V_BADBLOCK : 1;    /* file contains bad blocks         */
        unsigned FCH$V_MARKDEL : 1;     /* file is marked for delete        */
        unsigned FCH$V_NOCHARGE : 1;    /* file space is not to be charged  */
        unsigned FCH$V_ERASE : 1;       /* erase file contents before deletion  */
        unsigned FCH$V_ALM_AIP : 1;     /* Archive in progress              */
        unsigned FCH$V_ALM_ARCHIVED : 1; /* File archived                   */
        unsigned FCH$V_ALM_DELETED : 1; /* File contents deleted            */
/* Note: The high 8 bits of this longword                                   */
/* are reserved for user and CSS use.                                       */
        unsigned FCH$V_fill_7 : 3;
        } FCH$R_FILL_1_BITS;
    } ;
 
/*** MODULE $FJNDEF ***/
/*+                                                                         */
/*                                                                          */
/* File journal control bit definitions. These are identical to, and must   */
/* track, the bits in JOURNAL above, but are defined relative to the journal */
/* control byte instead of relative to the file header.                     */
/*                                                                          */
/*-                                                                         */
#define FJN$M_ONLY_RU 1
#define FJN$M_RUJNL 2
#define FJN$M_BIJNL 4
#define FJN$M_AIJNL 8
#define FJN$M_ATJNL 16
#define FJN$M_NEVER_RU 32
#define FJN$M_JOURNAL_FILE 64
union FJNDEF {
    char FJNDEF$$_FILL_1;
    variant_struct  {
        unsigned FJN$V_ONLY_RU : 1;     /* file is accessible only in recovery unit  */
        unsigned FJN$V_RUJNL : 1;       /* enable recovery unit journal     */
        unsigned FJN$V_BIJNL : 1;       /* enable before image journal      */
        unsigned FJN$V_AIJNL : 1;       /* enable after image journal       */
        unsigned FJN$V_ATJNL : 1;       /* enable audit trail journal       */
        unsigned FJN$V_NEVER_RU : 1;    /* file is never accessible in recovery unit */
        unsigned FJN$V_JOURNAL_FILE : 1; /* this is a journal file          */
        unsigned FJN$V_fill_8 : 1;
        } FJN$R_FILL_1_BITS;
    } ;
 
/*** MODULE $FATDEF ***/
/*+                                                                         */
/*                                                                          */
/* Record attributes area as used by FCS and RMS.                           */
/*                                                                          */
/*-                                                                         */
#define FAT$C_UNDEFINED 0               /* undefined record type            */
#define FAT$C_FIXED 1                   /* fixed record type                */
#define FAT$C_VARIABLE 2                /* variable length                  */
#define FAT$C_VFC 3                     /* variable + fixed control         */
#define FAT$C_STREAM 4                  /* RMS-11 (DEC traditional) stream format  */
#define FAT$C_STREAMLF 5                /* LF-terminated stream format      */
#define FAT$C_STREAMCR 6                /* CR-terminated stream format      */
#define FAT$C_SEQUENTIAL 0              /* sequential organization          */
#define FAT$C_RELATIVE 1                /* relative organization            */
#define FAT$C_INDEXED 2                 /* indexed organization             */
#define FAT$C_DIRECT 3                  /* direct organization              */
#define FAT$M_FORTRANCC 1
#define FAT$M_IMPLIEDCC 2
#define FAT$M_PRINTCC 4
#define FAT$M_NOSPAN 8
#define FAT$K_LENGTH 32
#define FAT$C_LENGTH 32
struct FATDEF {
    variant_union  {
        unsigned char FAT$B_RTYPE;      /* record type                      */
        variant_struct  {
            unsigned FAT$V_RTYPE : 4;   /* record type subfield             */
            unsigned FAT$V_FILEORG : 4; /* file organization                */
            } FAT$R_RTYPE_BITS;
        } FAT$R_RTYPE_OVERLAY;
    variant_union  {
        unsigned char FAT$B_RATTRIB;    /* record attributes                */
        variant_struct  {
            unsigned FAT$V_FORTRANCC : 1; /* Fortran carriage control       */
            unsigned FAT$V_IMPLIEDCC : 1; /* implied carriage control       */
            unsigned FAT$V_PRINTCC : 1; /* print file carriage control      */
            unsigned FAT$V_NOSPAN : 1;  /* no spanned records               */
            unsigned FAT$V_fill_9 : 4;
            } FAT$R_RATTRIB_BITS;
        } FAT$R_RATTRIB_OVERLAY;
    unsigned short int FAT$W_RSIZE;     /* record size in bytes             */
    variant_union  {
        unsigned long int FAT$L_HIBLK;  /* highest allocated VBN            */
        variant_struct  {
            unsigned short int FAT$W_HIBLKH; /* high order word             */
            unsigned short int FAT$W_HIBLKL; /* low order word              */
            } FAT$R_HIBLK_FIELDS;
        } FAT$R_HIBLK_OVERLAY;
    variant_union  {
        unsigned long int FAT$L_EFBLK;  /* end of file VBN                  */
        variant_struct  {
            unsigned short int FAT$W_EFBLKH; /* high order word             */
            unsigned short int FAT$W_EFBLKL; /* low order word              */
            } FAT$R_EFBLK_FIELDS;
        } FAT$R_EFBLK_OVERLAY;
    unsigned short int FAT$W_FFBYTE;    /* first free byte in EFBLK         */
    unsigned char FAT$B_BKTSIZE;        /* bucket size in blocks            */
    unsigned char FAT$B_VFCSIZE;        /* size in bytes of fixed length control for VFC records  */
    unsigned short int FAT$W_MAXREC;    /* maximum record size in bytes     */
    unsigned short int FAT$W_DEFEXT;    /* default extend quantity          */
    unsigned short int FAT$W_GBC;       /* global buffer count              */
    short int FATDEF$$_FILL_1 [4];      /* spare                            */
    unsigned short int FAT$W_VERSIONS;  /* default version limit for directory file  */
    } ;
 
/*** MODULE $HM1DEF ***/
/*+                                                                         */
/*                                                                          */
/* Home block definitions for Files-11 Structure Level 1                    */
/*                                                                          */
/*-                                                                         */
#define HM1$C_LEVEL1 257                /* 401 octal = structure level 1    */
#define HM1$C_LEVEL2 258                /* 402 octal = structure level 1, version 2  */
struct HM1DEF {
    unsigned short int HM1$W_IBMAPSIZE; /* index file bitmap size, blocks   */
    unsigned long int HM1$L_IBMAPLBN;   /* index file bitmap starting LBN   */
    unsigned short int HM1$W_MAXFILES;  /* maximum ! files on volume        */
    unsigned short int HM1$W_CLUSTER;   /* storage bitmap cluster factor    */
    unsigned short int HM1$W_DEVTYPE;   /* disk device type                 */
    unsigned short int HM1$W_STRUCLEV;  /* volume structure level           */
    char HM1$T_VOLNAME [12];            /* volume name (ASCII)              */
    char HM1DEF$$_FILL_1 [4];           /* spare                            */
    unsigned short int HM1$W_VOLOWNER;  /* volume owner UIC                 */
    variant_union  {
        unsigned short int HM1$W_PROTECT; /* volume protection              */
        variant_struct  {
            unsigned HM1$V_SYSPRO : 4;  /* system protection                */
            unsigned HM1$V_OWNPRO : 4;  /* owner protection                 */
            unsigned HM1$V_GROUPPRO : 4; /* group protection                */
            unsigned HM1$V_WORLDPRO : 4; /* world protection                */
            } HM1$R_PROTECT_BITS;
        } HM1$R_PROTECT_OVERLAY;
    unsigned short int HM1$W_VOLCHAR;   /* volume characteristics           */
    unsigned short int HM1$W_FILEPROT;  /* default file protection          */
    char HM1DEF$$_FILL_2 [6];           /* spare                            */
    unsigned char HM1$B_WINDOW;         /* default window size              */
    unsigned char HM1$B_EXTEND;         /* default file extend              */
    unsigned char HM1$B_LRU_LIM;        /* default LRU limit                */
    char HM1DEF$$_FILL_3 [11];          /* spare                            */
    unsigned short int HM1$W_CHECKSUM1; /* first checksum                   */
    char HM1$T_CREDATE [14];            /* volume creation date             */
    char HM1DEF$$_FILL_4 [382];         /* spare                            */
    unsigned long int HM1$L_SERIALNUM;  /* pack serial number               */
    char HM1DEF$$_FILL_5 [12];          /* reserved                         */
    char HM1$T_VOLNAME2 [12];           /* 2nd copy of volume name          */
    char HM1$T_OWNERNAME [12];          /* volume owner name                */
    char HM1$T_FORMAT [12];             /* volume format type               */
    char HM1DEF$$_FILL_6 [2];           /* spare                            */
    unsigned short int HM1$W_CHECKSUM2; /* second checksum                  */
    } ;
 
/*** MODULE $HM2DEF ***/
/*+                                                                         */
/*                                                                          */
/* Home block definitions for Files-11 Structure Level 2                    */
/*                                                                          */
/*-                                                                         */
#define HM2$C_LEVEL1 257                /* 401 octal = structure level 1    */
#define HM2$C_LEVEL2 512                /* 1000 octal = structure level 2   */
#define HM2$M_READCHECK 1
#define HM2$M_WRITCHECK 2
#define HM2$M_ERASE 4
#define HM2$M_NOHIGHWATER 8
#define HM2$M_CLASS_PROT 16
struct HM2DEF {
    unsigned long int HM2$L_HOMELBN;    /* LBN of home (i.e., this) block   */
    unsigned long int HM2$L_ALHOMELBN;  /* LBN of alternate home block      */
    unsigned long int HM2$L_ALTIDXLBN;  /* LBN of alternate index file header  */
    variant_union  {
        unsigned short int HM2$W_STRUCLEV; /* volume structure level        */
        variant_struct  {
            unsigned char HM2$B_STRUCVER; /* structure version number       */
            unsigned char HM2$B_STRUCLEV; /* main structure level           */
            } HM2$R_STRUCLEV_FIELDS;
        } HM2$R_STRUCLEV_OVERLAY;
    unsigned short int HM2$W_CLUSTER;   /* storage bitmap cluster factor    */
    unsigned short int HM2$W_HOMEVBN;   /* VBN of home (i.e., this) block   */
    unsigned short int HM2$W_ALHOMEVBN; /* VBN of alternate home block      */
    unsigned short int HM2$W_ALTIDXVBN; /* VBN of alternate index file header  */
    unsigned short int HM2$W_IBMAPVBN;  /* VBN of index file bitmap         */
    unsigned long int HM2$L_IBMAPLBN;   /* LBN of index file bitmap         */
    unsigned long int HM2$L_MAXFILES;   /* maximum ! files on volume        */
    unsigned short int HM2$W_IBMAPSIZE; /* index file bitmap size, blocks   */
    unsigned short int HM2$W_RESFILES;  /* ! reserved files on volume       */
    unsigned short int HM2$W_DEVTYPE;   /* disk device type                 */
    unsigned short int HM2$W_RVN;       /* relative volume number of this volume  */
    unsigned short int HM2$W_SETCOUNT;  /* count of volumes in set          */
    variant_union  {
        unsigned short int HM2$W_VOLCHAR; /* volume characteristics         */
        variant_struct  {
            unsigned HM2$V_READCHECK : 1; /* verify all read operations     */
            unsigned HM2$V_WRITCHECK : 1; /* verify all write operations    */
            unsigned HM2$V_ERASE : 1;   /* erase all files on delete        */
            unsigned HM2$V_NOHIGHWATER : 1; /* turn off high-water marking  */
            unsigned HM2$V_CLASS_PROT : 1; /* enable classification checks on the volume */
            unsigned HM2$V_fill_10 : 3;
            } HM2$R_VOLCHAR_BITS;
        } HM2$R_VOLCHAR_OVERLAY;
    unsigned long int HM2$L_VOLOWNER;   /* volume owner UIC                 */
    unsigned long int HM2$L_SEC_MASK;   /* volume security mask             */
    unsigned short int HM2$W_PROTECT;   /* volume protection                */
    unsigned short int HM2$W_FILEPROT;  /* default file protection          */
    unsigned short int HM2$W_RECPROT;   /* default file record protection   */
    unsigned short int HM2$W_CHECKSUM1; /* first checksum                   */
    unsigned int HM2$Q_CREDATE [2];     /* volume creation date             */
    unsigned char HM2$B_WINDOW;         /* default window size              */
    unsigned char HM2$B_LRU_LIM;        /* default LRU limit                */
    unsigned short int HM2$W_EXTEND;    /* default file extend              */
    unsigned int HM2$Q_RETAINMIN [2];   /* minimum file retention period    */
    unsigned int HM2$Q_RETAINMAX [2];   /* maximum file retention period    */
    unsigned int HM2$Q_REVDATE [2];     /* volume revision date             */
    variant_struct  {                           /* volume minimum security class    */
        char HM2$B_FILL_2 [20];
        } HM2$R_MIN_CLASS;
    variant_struct  {                           /* volume maximum security class    */
        char HM2$B_FILL_3 [20];
        } HM2$R_MAX_CLASS;
    unsigned short int HM2$W_FILETAB_FID [3]; /* file lookup table FID      */
    variant_union  {
        unsigned short int HM2$W_LOWSTRUCLEV; /* lowest struclev on volume  */
        variant_struct  {
            unsigned char HM2$B_LOWSTRUCVER; /* structure version number    */
            unsigned char HM2$B_LOWSTRUCLEV; /* main structure level        */
            } HM2$R_LOWSTRUCLEV_FIELDS;
        } HM2$R_LOWSTRUCLEV_OVERLAY;
    variant_union  {
        unsigned short int HM2$W_HIGHSTRUCLEV; /* highest struclev on volume */
        variant_struct  {
            unsigned char HM2$B_HIGHSTRUCVER; /* structure version number   */
            unsigned char HM2$B_HIGHSTRUCLEV; /* main structure level       */
            } HM2$R_HIGHSTRUCLEV_FIELDS;
        } HM2$R_HIGHSTRUCLEV_OVERLAY;
    char HM2DEF$$_FILL_1 [310];         /* spare                            */
    unsigned long int HM2$L_SERIALNUM;  /* pack serial number               */
    char HM2$T_STRUCNAME [12];          /* structure (volume set name)      */
    char HM2$T_VOLNAME [12];            /* volume name                      */
    char HM2$T_OWNERNAME [12];          /* volume owner name                */
    char HM2$T_FORMAT [12];             /* volume format type               */
    char HM2DEF$$_FILL_2 [2];           /* spare                            */
    unsigned short int HM2$W_CHECKSUM2; /* second checksum                  */
    } ;
 
/*** MODULE $DIRDEF ***/
/*+                                                                         */
/*                                                                          */
/* Directory entry structure for Files-11 Structure Level 2                 */
/*                                                                          */
/*-                                                                         */
#define DIR$C_FID 0                     /* normal file ID                   */
#define DIR$C_LINKNAME 1                /* symbolic name                    */
#define DIR$K_LENGTH 6                  /* length of directory entry overhead  */
#define DIR$C_LENGTH 6                  /* length of directory entry overhead  */
#define DIR$S_NAME 80                   /* maximum length of name string    */
struct DIRDEF {
    unsigned short int DIR$W_SIZE;      /* size of directory record in bytes  */
    unsigned short int DIR$W_VERLIMIT;  /* maximum number of versions       */
    variant_union  {
        unsigned char DIR$B_FLAGS;      /* status flags                     */
        variant_struct  {
            unsigned DIR$V_TYPE : 3;    /* directory entry type             */
            unsigned DIRDEF$$_FILL_1 : 3; /* reserved                       */
            unsigned DIR$V_NEXTREC : 1; /* another record of same name & type follows  */
            unsigned DIR$V_PREVREC : 1; /* another record of same name & type precedes  */
            } DIR$R_FLAGS_BITS;
/* directory entry type codes                                               */
        } DIR$R_FLAGS_OVERLAY;
    variant_union  {
        unsigned char DIR$B_NAMECOUNT;  /* byte count of name string        */
        variant_struct  {
            char DIRDEF$$_FILL_2;
            char DIR$T_NAME [];         /* name string                      */
/* the version numbers and file ID's follow the                             */
/* variable length name area in the form of a                               */
/* blockvector. Each entry is as follows:                                   */
            } DIR$R_NAMECOUNT_FIELDS;
        } DIR$R_NAMECOUNT_OVERLAY;
    } ;
#define DIR$K_VERSION 8                 /* size of each version entry       */
#define DIR$C_VERSION 8                 /* size of each version entry       */
struct DIRDEF1 {
    short int DIR$W_VERSION;            /* version number                   */
    variant_union  {
        unsigned short int DIR$W_FID [3]; /* file ID                        */
        variant_struct  {
            unsigned short int DIR$W_FID_NUM; /* file number                */
            unsigned short int DIR$W_FID_SEQ; /* file sequence number       */
            variant_union  {
                unsigned short int DIR$W_FID_RVN; /* relative volume number  */
                variant_struct  {
                    unsigned char DIR$B_FID_RVN; /* alternate format RVN    */
                    unsigned char DIR$B_FID_NMX; /* alternate format file number extension  */
                    } DIR$R_FID_RVN_FIELDS;
                } DIR$R_FID_RVN_OVERLAY;
            } DIR$R_FID_FIELDS;
        } DIR$R_FID_OVERLAY;
    } ;
union DIRDEF2 {
    char DIR$T_LINKNAME;                /* symbolic link name (counted string)  */
    } ;
 
/*** MODULE $SCBDEF ***/
/*+                                                                         */
/*                                                                          */
/* Format of storage control block, Files-11 Structure Level 2              */
/*                                                                          */
/*-                                                                         */
#define SCB$C_LEVEL2 512                /* 1000 octal = structure level 2   */
#define SCB$M_MAPDIRTY 1
#define SCB$M_MAPALLOC 2
#define SCB$M_FILALLOC 4
#define SCB$M_QUODIRTY 8
#define SCB$M_HDRWRITE 16
#define SCB$M_CORRUPT 32
#define SCB$M_MAPDIRTY2 1
#define SCB$M_MAPALLOC2 2
#define SCB$M_FILALLOC2 4
#define SCB$M_QUODIRTY2 8
#define SCB$M_HDRWRITE2 16
#define SCB$M_CORRUPT2 32
#define SCB$M_NORMAL 1
#define SCB$M_NEW 2
#define SCB$M_REBLDNG 4
#define SCB$M_VERIFY 8
#define SCB$M_MVBEGUN 16
#define SCB$M_COPYING 32
#define SCB$M_MERGING 64
#define SCB$M_MINIMRG 128
#define SCB$M_CPY_RESET 256
#define SCB$M_BOOTING 512
#define SCB$M_FAILED 32768
#define SCB$M_MBR_FCPY 1
#define SCB$M_MBR_MERGE 2
#define SCB$M_MBR_CIP 4
#define SCB$M_MBR_MASTER 8
#define SCB$M_MBR_ERROR 16
#define SCB$M_MBR_SRC 32
#define SCB$M_MBR_VALID 128
#define SCB$K_LENGTH 512                /*Length of Structure               */
#define SCB$C_LENGTH 512                /*Length of Structure               */
struct SCBDEF {
    variant_union  {
        unsigned short int SCB$W_STRUCLEV; /* file structure level          */
        variant_struct  {
            unsigned char SCB$B_STRUCVER; /* file structure version         */
            unsigned char SCB$B_STRUCLEV; /* principal file structure level  */
            } SCB$R_STRUCLEV_FIELDS;
        } SCB$R_STRUCLEV_OVERLAY;
    unsigned short int SCB$W_CLUSTER;   /* storage map cluster factor       */
    unsigned long int SCB$L_VOLSIZE;    /* volume size in logical blocks    */
    unsigned long int SCB$L_BLKSIZE;    /* number of physical blocks per logical block  */
    unsigned long int SCB$L_SECTORS;    /* number of sectors per track      */
    unsigned long int SCB$L_TRACKS;     /* number of tracks per cylinder    */
    unsigned long int SCB$L_CYLINDER;   /* number of cylinders              */
    variant_union  {
        unsigned long int SCB$L_STATUS; /* volume status flags              */
        variant_struct  {
            unsigned SCB$V_MAPDIRTY : 1; /* storage map is dirty (partially updated)  */
            unsigned SCB$V_MAPALLOC : 1; /* storage map is preallocated (lost blocks)  */
            unsigned SCB$V_FILALLOC : 1; /* file numbers are preallocated (lost header slots)  */
            unsigned SCB$V_QUODIRTY : 1; /* quota file is dirty (partially updated)  */
            unsigned SCB$V_HDRWRITE : 1; /* file headers are write back cached  */
            unsigned SCB$V_CORRUPT : 1; /* file structure is corrupt        */
            unsigned SCB$V_fill_11 : 2;
            } SCB$R_STATUS_BITS;
        } SCB$R_STATUS_OVERLAY;
    variant_union  {
        unsigned long int SCB$L_STATUS2; /* backup status - bits must match those above  */
        variant_struct  {
            unsigned SCB$V_MAPDIRTY2 : 1; /* storage map is dirty (partially updated)  */
            unsigned SCB$V_MAPALLOC2 : 1; /* storage map is preallocated (lost blocks)  */
            unsigned SCB$V_FILALLOC2 : 1; /* file numbers are preallocated (lost header slots)  */
            unsigned SCB$V_QUODIRTY2 : 1; /* quota file is dirty (partially updated)  */
            unsigned SCB$V_HDRWRITE2 : 1; /* file headers are write back cached  */
            unsigned SCB$V_CORRUPT2 : 1; /* file structure is corrupt       */
            unsigned SCB$V_fill_12 : 2;
            } SCB$R_STATUS2_BITS;
        } SCB$R_STATUS2_OVERLAY;
    unsigned short int SCB$W_WRITECNT;  /* count of write access mounters.  */
    char SCB$T_VOLOCKNAME [12];         /* name used for file system serialization on volume. */
    unsigned int SCB$Q_MOUNTTIME [2];   /* time of last initial mount.      */
    unsigned short int SCB$W_BACKREV;   /* BACKUP revision number.          */
    unsigned int SCB$Q_GENERNUM [2];    /* shadow set revision number.      */
    unsigned int SCB$Q_UNIT_ID [2];     /* Virtual Unit specifier           */
    variant_union  {
        unsigned short int SCB$W_SHADOW_STATUS; /* Volume status:           */
        variant_struct  {
            unsigned SCB$V_NORMAL : 1;  /* Shadow set populated and online  */
            unsigned SCB$V_NEW : 1;     /* Newly created, no members yet    */
            unsigned SCB$V_REBLDNG : 1; /* rebuilding shadow set            */
            unsigned SCB$V_VERIFY : 1;  /* This SS needs verification       */
            unsigned SCB$V_MVBEGUN : 1; /* Mount verification initiated     */
            unsigned SCB$V_COPYING : 1; /* Full copy in progreess           */
            unsigned SCB$V_MERGING : 1; /* Merge copy in progress           */
            unsigned SCB$V_MINIMRG : 1; /* Mini-merge in progress           */
            unsigned SCB$V_CPY_RESET : 1; /* Copy mode is reset.            */
            unsigned SCB$V_BOOTING : 1; /* Shadow set in booting state      */
            unsigned SCB$v_filler : 5;  /* Reserve one last field           */
            unsigned SCB$V_FAILED : 1;  /* Shadow set not populated         */
            } SCB$R_SHADOW_STATUS_BITS;
        } SCB$R_SHADOW_STATUS_OVERLAY;
    union  {
        unsigned char SCB$B_MEMBER_STATUS; /* Member status bytes           */
        variant_struct  {
            unsigned SCB$V_MBR_FCPY : 1; /* Member involved in copy         */
            unsigned SCB$V_MBR_MERGE : 1; /* Member being merged            */
            unsigned SCB$V_MBR_CIP : 1; /* Copy (or merge) in progress      */
            unsigned SCB$V_MBR_MASTER : 1; /* Member with guaranteed correct */
/*  SCB. Same member as MEMBERSHIP_LOCK                                     */
            unsigned SCB$V_MBR_ERROR : 1; /* Error processing in progress   */
            unsigned SCB$V_MBR_SRC : 1; /* member can be used for source    */
            unsigned SCB$v_filler : 1;  /* Reserve one last field           */
            unsigned SCB$V_MBR_VALID : 1; /* Status information is valid    */
            } SCB$R_MEMBER_STATUS_BITS;
        } SCB$R_MEMBER_STATUS_OVERLAY [3];
    char SCB$b_filler [3];              /* Reserved for alignment           */
    unsigned int SCB$Q_MEMBER_IDS [2] [3]; /* Unit ID for member            */
    unsigned long int SCB$L_SCB_LBN;    /* Unit Control Block for VU        */
    unsigned char SCB$B_DEVICES;        /* Number of devices in SS          */
    unsigned char SCB$B_MEMBERS;        /* Number of full members           */
    unsigned char SCB$B_MAST_INDX;      /* Array index to master UCB        */
    unsigned char SCB$B_MRG_TARGETS;    /* Merge Copy Targets               */
    unsigned char SCB$B_FC_TARGETS;     /* Full Copy Targets                */
    char SCB$b_shadow_reserved [84];    /* reserved for expansion to 10 units per set */
    char SCB$b_reserved [313];          /* reserved                         */
    unsigned short int SCB$W_CHECKSUM;  /* block checksum                   */
    } ;
 
/*** MODULE $BBMDEF ***/
/*+                                                                         */
/*                                                                          */
/* Bad block map (generated by bad block scan program)                      */
/*                                                                          */
/*-                                                                         */
#define BBM$K_POINTERS 4                /* start of retrieval pointers      */
#define BBM$C_POINTERS 4                /* start of retrieval pointers      */
struct BBMDEF {
    unsigned char BBM$B_COUNTSIZE;      /* retrieval pointer count field size  */
    unsigned char BBM$B_LBNSIZE;        /* retrieval pointer LBN field size  */
    unsigned char BBM$B_INUSE;          /* number of retrieval words in use  */
    unsigned char BBM$B_AVAIL;          /* number of retrieval words available  */
    char BBMDEF$$_FILL_1 [506];         /* pointer space                    */
    unsigned short int BBM$W_CHECKSUM;  /* block checksum                   */
/* retrieval pointer format                                                 */
    } ;
struct BBMDEF1 {
    unsigned char BBM$B_HIGHLBN;        /* high order LBN                   */
    unsigned char BBM$B_COUNT;          /* block count                      */
    unsigned short int BBM$W_LOWLBN;    /* low order LBN                    */
    } ;
struct BBMDEF2 { /* WARNING: aggregate has origin of -4 */
    unsigned char BBM$B_PREVHLBN;
    unsigned char BBM$B_PREVCOUNT;
    unsigned short int BBM$W_PREVLLBN;  /* previous retrieval pointer       */
    char BBMDEF$$_FILL_2;
    } ;
 
/*** MODULE $BBDDEF ***/
/*+                                                                         */
/*                                                                          */
/* Bad block descriptor (generated by formatters for RK06, RM03, et al)     */
/*                                                                          */
/*-                                                                         */
#define BBD$K_DESCRIPT 8                /* start of bad block descriptors   */
#define BBD$C_DESCRIPT 8                /* start of bad block descriptors   */
struct BBDDEF {
    unsigned long int BBD$L_SERIAL;     /* pack serial number               */
    unsigned short int BBD$W_RESERVED;  /* reserved area (MBZ)              */
    unsigned short int BBD$W_FLAGS;     /* pack status flags (zero for normal use)  */
    char BBDDEF$$_FILL_1 [500];
    unsigned long int BBD$L_LASTWORD;   /* last longword of block           */
    } ;
#define BBD$K_ENTRY 4
#define BBD$C_ENTRY 4
union BBDDEF1 {
    unsigned long int BBD$L_BADBLOCK;   /* individual bad block entry       */
    variant_struct  {
        unsigned BBD$V_CYLINDER : 15;   /* cylinder number of bad block     */
        unsigned BBDDEF$$_FILL_2 : 1;
        unsigned BBD$V_SECTOR : 8;      /* sector number of bad block       */
        unsigned BBD$V_TRACK : 7;       /* track number of bad block        */
        unsigned BBD$V_fill_13 : 1;
        } BBD$R_BADBLOCK_BITS;
    } ;
 
/*** MODULE $VSLDEF ***/
/*+                                                                         */
/*                                                                          */
/* Structure of a volume set list file entry. Record 1 contains the volume  */
/* set name. Record n+1 contains the volume label of RVN n in the volume set. */
/*                                                                          */
/*-                                                                         */
#define VSL$K_LENGTH 64
#define VSL$C_LENGTH 64
struct VSLDEF {
    char VSL$T_NAME [12];               /* volume name                      */
    char VSLDEF$$_FILL_1 [52];          /* unused                           */
    } ;
 
/*** MODULE $PBBDEF ***/
/*+                                                                         */
/*                                                                          */
/* Pending bad block file record format. Each record describes a disk block */
/* on which an error has occurred which has not been turned over to the bad */
/* block file.                                                              */
/*                                                                          */
/*-                                                                         */
#define PBB$M_READERR 1
#define PBB$M_WRITERR 2
#define PBB$K_LENGTH 16                 /* length of entry                  */
#define PBB$C_LENGTH 16                 /* length of entry                  */
struct PBBDEF {
    unsigned short int PBB$W_FID [3];   /* File ID of containing file       */
    variant_union  {
        unsigned char PBB$B_FLAGS;      /* status flags                     */
        variant_struct  {
            unsigned PBB$V_READERR : 1; /* read error occurred              */
            unsigned PBB$V_WRITERR : 1; /* write error occurred             */
            unsigned PBB$V_fill_14 : 6;
            } PBB$R_FLAGS_BITS;
        } PBB$R_FLAGS_OVERLAY;
    unsigned char PBB$B_COUNT;          /* error count                      */
    unsigned long int PBB$L_VBN;        /* virtual block in file            */
    unsigned long int PBB$L_LBN;        /* logical block number             */
    } ;
 
/*** MODULE $DQFDEF ***/
/*+                                                                         */
/*                                                                          */
/* Structure of disk quota file record. Each record contains the authorization */
/* and usage of a particular UIC for this volume set.                       */
/*                                                                          */
/*-                                                                         */
#define DQF$M_ACTIVE 1
#define DQF$K_LENGTH 32
#define DQF$C_LENGTH 32
struct DQFDEF {
    variant_union  {
        unsigned long int DQF$L_FLAGS;  /* flags longword, containing...    */
        variant_struct  {
            unsigned DQF$V_ACTIVE : 1;  /* record contains an active entry  */
            unsigned DQF$V_fill_15 : 7;
            } DQF$R_FLAGS_BITS;
        } DQF$R_FLAGS_OVERLAY;
    unsigned long int DQF$L_UIC;        /* UIC of this record               */
    unsigned long int DQF$L_USAGE;      /* number of blocks in use          */
    unsigned long int DQF$L_PERMQUOTA;  /* permanent disk quota             */
    unsigned long int DQF$L_OVERDRAFT;  /* overdraft limit                  */
    long int DQFDEF$$_FILL_1 [3];       /* reserved                         */
    } ;
