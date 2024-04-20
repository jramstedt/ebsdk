/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:07:21 by OpenVMS SDL EV1-31     */
/* Source:   6-OCT-1995 11:18:52 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]MBX_DEF.SDL; */
/********************************************************************************************************************************/
/* file:	mbx_def.sdl                                                 */
/*                                                                          */
/* Copyright (C) 1990, 1991, 1992 by                                        */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without noticec   */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Mailbox data structure definitions for                      */
/*		Alpha firmware.                                             */
/*                                                                          */
/* Author:	K LeMieux                                                   */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                           */
/*	jds	27-Sep-1991	Adjusted wait, queue timeout constants	     */
/*                                                                          */
/*** MODULE $mbxdef ***/
#ifndef MBX_DEF_H
#define MBX_DEF_H
#define MBX_XMI 0
#define MBX_FB 1
#define MBX_FB_UNMASKED 0
#define MBX_FB_MASKED 2
#define MBX_FB_READ 0
#define MBX_FB_WRITE 1
#define MBX_FB_32 0
#define MBX_FB_64 1
#define MBX_FB_128 2
#define MBX_FB_256 3
#define MBX_XMI_READ 1
#define MBX_XMI_WRITE 7
#define MBX_XMI_MASK 240
#define MBX_MAX_HOSE 2
#define MBX_DEFAULT_QUEUE_TIME 200000000 /* 2 secs                          */
#define MBX_DEFAULT_WAIT_TIME 200000000 /* 2 secs                           */
struct MBPR {
    unsigned int stqc;
    unsigned int stqc_complete;
    unsigned int stqc_delay_time;
    unsigned int operation_delay_time;
    unsigned int queue_attempts;
    unsigned int wait_attempts;
    unsigned int xmi_csr1_value;
    unsigned int xmi_csr2_value;
    unsigned int fb_csr1_value;
    unsigned int fb_csr2_value;
    struct MBX *mbx;
    } ;
struct MBX_HOSE {
    char bus_type;
    } ;
struct MBX_STATE {
    struct MBX_HOSE config [2];
    } ;
#define m_transaction 0xF
#define m_write 0x10
#define m_data_width 0x60
#define m_address_width 0x80
#define m_unused 0xFF00
#define m_f_diag 0x7F0000
#define m_c_diag 0x3F800000
#define m_special_cmd 0x40000000
#define m_write_access 0x80000000
#define m_msk 0xFF
#define m_sbz0 0xFF00
#define m_hose 0xFF0000
#define m_sbz1 0xFF000000
#define m_done 0x1
#define m_error 0x2
#define m_info 0xFFFFFFFC
struct MBX {
    struct MBX *flink;                  /* forward link                     */
    struct MBX *blink;                  /* backward link                    */
    struct MBPR *mbpr;                  /* address of MBPR (local I/O) space */
    unsigned int queue_time [2];        /* timeout value for STQ_C to MBPR  */
    unsigned int wait_time [2];         /* timeout value for wait for DON   */
    unsigned int flags [2];             /* flags                            */
    int iopslot;                        /* TLSB slot of the IOP             */
    unsigned int filler0 [2];           /* miscellaneous                    */
    unsigned int filler1 [2];           /* miscellaneous                    */
    unsigned int filler2 [2];           /* miscellaneous                    */
    struct  {                           /* Hardware I/O mailbox must be 64 byte aligned */
        unsigned transaction : 4;       /* transaction code                 */
        unsigned write : 1;             /* write/read                       */
        unsigned data_width : 2;        /* data width                       */
        unsigned address_width : 1;     /* address width                    */
        unsigned unused : 8;            /* unused                           */
        unsigned f_diag : 7;            /* f_diag                           */
        unsigned c_diag : 7;            /* c_diag                           */
        unsigned special_cmd : 1;       /* special command                  */
        unsigned write_access : 1;      /* wrie access                      */
        } ctr_l;
/*                                                                          */
    struct  {                           /* Hardware I/O mailbox must be 64 byte aligned */
        unsigned msk : 8;               /* mask of bytes to be transferred  */
        unsigned sbz0 : 8;              /* should be zero                   */
        unsigned hose : 8;              /* hose number	                    */
        unsigned sbz1 : 8;              /* should be zero	            */
        } ctr_h;
/*                                                                          */
    int *rbadr;                         /* Remote bus address - PA of CSR to be accessed	 */
    int *rbadr1;
    unsigned int wdata [2];             /* Data to be written to CSR on write operation */
    unsigned int u0 [2];                /* unpredictable                    */
    unsigned int rdata;                 /* Data returned from read operation	 */
    unsigned int u3;                    /* unpredictable 		    */
    struct  {                           /*                                  */
        unsigned done : 1;              /* status indicating operation completed */
        unsigned error : 1;             /* error - queue                    */
        unsigned info : 30;             /* operation completion information (valid only if done set) */
        } status_l;
/*                                                                          */
    struct  {                           /*                                  */
        unsigned int info;              /* operation completion information (valid only if done set) */
        } status_h;
/*                                                                          */
    unsigned int u1 [2];                /* unpredictable 	            */
    unsigned int u2 [2];                /* unpredictable 		    */
    } ;
#endif
 
