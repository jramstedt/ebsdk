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
 * Abstract:	SII Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	30-May-1990	Initial entry.
 */
#include "cp$src:dssi_def.h"
 
#define sii_k_max_dma 8192

#define sii_k_buffer 131072

#define dat_out_offset 0
#define dat_in_offset dat_out_offset+sii_k_max_dma
#define cmd_offset dat_in_offset+sii_k_max_dma
#define sts_offset cmd_offset+sii_k_max_dma
#define msg_out_offset sts_offset+sii_k_max_dma
#define msg_in_offset msg_out_offset+sii_k_max_dma

volatile struct sii1_csr {
    unsigned short int sdb;
    unsigned short int sc1;
    unsigned short int sc2;
    unsigned short int csr;
    unsigned short int id;
    unsigned short int slcsr;
    unsigned short int destat;
    unsigned short int dstmo;
    unsigned short int data;
    unsigned short int dmctrl;
    unsigned short int dmlotc;
    unsigned short int dmaddrl;
    unsigned short int dmaddrh;
    unsigned short int dmabyte;
    unsigned short int stlp;
    unsigned short int ltlp;
    unsigned short int ilp;
    unsigned short int dsctrl;
    unsigned short int cstat;
    unsigned short int dstat;
    unsigned short int comm;
    unsigned short int dictrl;
    unsigned short int clock;
    unsigned short int bhdiag;
    unsigned short int sidiag;
    unsigned short int dmdiag;
    unsigned short int mcdiag;
    unsigned short int dmacsr;
    unsigned short int qbar;
    unsigned short int lbar;
    unsigned short int wc;
    unsigned short int vec;
    } ;

volatile struct sii2_csr {
    unsigned short int sdb;
    unsigned short int fill1;
    unsigned short int sc1;
    unsigned short int fill2;
    unsigned short int sc2;
    unsigned short int fill3;
    unsigned short int csr;
    unsigned short int fill4;
    unsigned short int id;
    unsigned short int fill5;
    unsigned short int slcsr;
    unsigned short int fill6;
    unsigned short int destat;
    unsigned short int fill7;
    unsigned short int dstmo;
    unsigned short int fill8;
    unsigned short int data;
    unsigned short int fill9;
    unsigned short int dmctrl;
    unsigned short int fill10;
    unsigned short int dmlotc;
    unsigned short int fill11;
    unsigned short int dmaddrl;
    unsigned short int fill12;
    unsigned short int dmaddrh;
    unsigned short int fill13;
    unsigned short int dmabyte;
    unsigned short int fill14;
    unsigned short int stlp;
    unsigned short int fill15;
    unsigned short int ltlp;
    unsigned short int fill16;
    unsigned short int ilp;
    unsigned short int fill17;
    unsigned short int dsctrl;
    unsigned short int fill18;
    unsigned short int cstat;
    unsigned short int fill19;
    unsigned short int dstat;
    unsigned short int fill20;
    unsigned short int comm;
    unsigned short int fill21;
    unsigned short int dictrl;
    unsigned short int fill22;
    unsigned short int clock;
    unsigned short int fill23;
    unsigned short int bhdiag;
    unsigned short int fill24;
    unsigned short int sidiag;
    unsigned short int fill25;
    unsigned short int dmdiag;
    unsigned short int fill26;
    unsigned short int mcdiag;
    unsigned short int fill27;
    } ;

#define sii_csr_ie 1
#define sii_csr_pce 2
#define sii_csr_sle 4
#define sii_csr_rse 8
#define sii_csr_hpm 16

volatile struct sii_csr_csr {
    unsigned csr_ie : 1;
    unsigned csr_pce : 1;
    unsigned csr_sle : 1;
    unsigned csr_rse : 1;
    unsigned csr_hpm : 1;
    unsigned csr_fill1 : 11;
    } ;

#define sii_cstat_fill1 1
#define sii_cstat_lst 2
#define sii_cstat_sip 4
#define sii_cstat_swa 8
#define sii_cstat_tgt 16
#define sii_cstat_dst 32
#define sii_cstat_con 64
#define sii_cstat_sch 128
#define sii_cstat_ldn 256
#define sii_cstat_buf 512
#define sii_cstat_tz 1024
#define sii_cstat_obc 2048
#define sii_cstat_ber 4096
#define sii_cstat_rst 8192
#define sii_cstat_di 16384
#define sii_cstat_ci 32768

volatile struct sii_csr_cstat {
    unsigned cstat_fill1 : 1;
    unsigned cstat_lst : 1;
    unsigned cstat_sip : 1;
    unsigned cstat_swa : 1;
    unsigned cstat_tgt : 1;
    unsigned cstat_dst : 1;
    unsigned cstat_con : 1;
    unsigned cstat_sch : 1;
    unsigned cstat_ldn : 1;
    unsigned cstat_buf : 1;
    unsigned cstat_tz : 1;
    unsigned cstat_obc : 1;
    unsigned cstat_ber : 1;
    unsigned cstat_rst : 1;
    unsigned cstat_di : 1;
    unsigned cstat_ci : 1;
    } ;

#define sii_dstat_io 1
#define sii_dstat_cd 2
#define sii_dstat_msg 4
#define sii_dstat_atn 8
#define sii_dstat_mis 16
#define sii_dstat_fill1 224
#define sii_dstat_obb 256
#define sii_dstat_ipe 512
#define sii_dstat_ibf 1024
#define sii_dstat_tbe 2048
#define sii_dstat_tcz 4096
#define sii_dstat_dne 8192
#define sii_dstat_di 16384
#define sii_dstat_ci 32768

volatile struct sii_csr_dstat {
    unsigned dstat_io : 1;
    unsigned dstat_cd : 1;
    unsigned dstat_msg : 1;
    unsigned dstat_atn : 1;
    unsigned dstat_mis : 1;
    unsigned dstat_fill1 : 3;
    unsigned dstat_obb : 1;
    unsigned dstat_ipe : 1;
    unsigned dstat_ibf : 1;
    unsigned dstat_tbe : 1;
    unsigned dstat_tcz : 1;
    unsigned dstat_dne : 1;
    unsigned dstat_di : 1;
    unsigned dstat_ci : 1;
    } ;

#define sii_comm_io 1
#define sii_comm_cd 2
#define sii_comm_msg 4
#define sii_comm_atn 8
#define sii_comm_tgt 16
#define sii_comm_dst 32
#define sii_comm_con 64
#define sii_comm_cmd_reset 128
#define sii_comm_cmd_disc 256
#define sii_comm_cmd_reqd 512
#define sii_comm_cmd_sel 1024
#define sii_comm_cmd_xfer 2048
#define sii_comm_rsl 4096
#define sii_comm_fill1 8192
#define sii_comm_bus_rst 16384
#define sii_comm_dma 32768

volatile struct sii_csr_comm {
    unsigned comm_io : 1;
    unsigned comm_cd : 1;
    unsigned comm_msg : 1;
    unsigned comm_atn : 1;
    unsigned comm_tgt : 1;
    unsigned comm_dst : 1;
    unsigned comm_con : 1;
    unsigned comm_cmd_reset : 1;
    unsigned comm_cmd_disc : 1;
    unsigned comm_cmd_reqd : 1;
    unsigned comm_cmd_sel : 1;
    unsigned comm_cmd_xfer : 1;
    unsigned comm_rsl : 1;
    unsigned comm_fill1 : 1;
    unsigned comm_bus_rst : 1;
    unsigned comm_dma : 1;
    } ;

#define sii_dictrl_tst 1
#define sii_dictrl_dia 2
#define sii_dictrl_pre 4
#define sii_dictrl_lpb 8
#define sii_dictrl_fill1 65520

volatile struct sii_csr_dictrl {
    unsigned dictrl_tst : 1;
    unsigned dictrl_dia : 1;
    unsigned dictrl_pre : 1;
    unsigned dictrl_lpb : 1;
    unsigned dictrl_fill1 : 12;
    } ;

#define sii_k_dat_out 0
#define sii_k_dat_in 1
#define sii_k_cmd 2
#define sii_k_sts 3
#define sii_k_bad 4
#define sii_k_msg_out 6
#define sii_k_msg_in 7
#define sii_k_mask 7

#define sii_comm_cmd 3968
#define sii_comm_ini_pio 2112
#define sii_comm_tgt_pio 2160
#define sii_comm_ini_dma 34880
#define sii_comm_tgt_dma 34928

#define out_offset 8
#define in_offset ((out_offset+sizeof(struct sii_out)+7)&~7)
#define num_in 

volatile struct sii_out {
    unsigned short int thread;
    unsigned short int status;
    unsigned short int command;
    struct dssi_header dssi_header;
    unsigned short int size;
    unsigned short int link;
    unsigned char text [dssi_k_max_pkt];
    } ;

volatile struct sii_in {
    struct QUEUE sii_in;
    unsigned short int thread;
    unsigned short int status;
    unsigned short int command;
    struct dssi_header dssi_header;
    unsigned char text [dssi_k_max_pkt];
    } ;
