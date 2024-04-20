/*
 * Copyright (C) 1992 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
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
 * Abstract:    Intel 82357 ISP (Integrated System Peripheral) Definitions
 *
 * Author:      Judith Gold
 *
 * Modifications:
 *
 *      jeg     01-Dec-1992     Initial entry.
 */

/* ISP CSRs */

struct isp_regs {
						/* 0x00 - 0xDE */
	unsigned char dma1_ch0_curadd;		
	unsigned char dma1_ch0_curcount;
	unsigned char dma1_ch1_curadd;
	unsigned char dma1_ch1_curcount;
	unsigned char dma1_ch2_curadd;
	unsigned char dma1_ch2_curcount;
	unsigned char dma1_ch3_curadd;
	unsigned char dma1_ch3_curcount;
	unsigned char dma1_stat_cmd;
	unsigned char dma1_write_req;
	unsigned char dma1_write_smask;
	unsigned char dma1_write_mode;
	unsigned char dma1_clear_ptr_ff;
	unsigned char dma1_master_clear;
	unsigned char dma1_clear_mask;
	unsigned char dma1_all_mask;
	unsigned char reserved0[16];
	unsigned char int1_control;
	unsigned char int1_mask;
	unsigned char reserved1[30];
	unsigned char interval_timer_1;
	unsigned char refresh_request;
	unsigned char speaker_tone;
	unsigned char command_mode0;
	unsigned char reserved2[4];
	unsigned char interval_timer_2;
	unsigned char reserved3;
	unsigned char cpu_speed;
	unsigned char command_mode1;
	unsigned char reserved4[21];
	unsigned char nmi_status;
	unsigned char reserved5[14];
	unsigned char nmi_enable;
	unsigned char reserved6[15];
	unsigned char reserved7;
	unsigned char dmapage_ch2;
	unsigned char dmapage_ch3;
	unsigned char dmapage_ch1;
	unsigned char reserved8[3];
	unsigned char dmapage_ch0;
	unsigned char reserved9;
	unsigned char dmapage_ch6;
	unsigned char dmapage_ch7;
	unsigned char dmapage_ch5;
	unsigned char reserveda[3];
	unsigned char dmapage_refresh;
	unsigned char reservedb[16];
	unsigned char int2_control;
	unsigned char int2_mask;
	unsigned char reservedc[30];
	unsigned char dma2_ch0_curadd;
  	unsigned char reservedd;
  	unsigned char dma2_ch0_curcount;
  	unsigned char reservede;
  	unsigned char dma2_ch1_curadd;
  	unsigned char reservedf;
  	unsigned char dma2_ch1_curcount;
  	unsigned char reserved10;
  	unsigned char dma2_ch2_curadd;
  	unsigned char reserved11;
  	unsigned char dma2_ch2_curcount;
  	unsigned char reserved12;
  	unsigned char dma2_ch3_curadd;
  	unsigned char reserved13;
  	unsigned char dma2_ch3_curcount;
  	unsigned char reserved14;
  	unsigned char dma2_stat_cmd;
  	unsigned char reserved15;
	unsigned char dma2_write_req;
  	unsigned char reserved16;
	unsigned char dma2_write_smask;
  	unsigned char reserved17;
	unsigned char dma2_write_mode;
  	unsigned char reserved18;
	unsigned char dma2_clear_ptr_ff;
  	unsigned char reserved19;
	unsigned char dma2_master_clear;
  	unsigned char reserved1a;
	unsigned char dma2_clear_mask;
  	unsigned char reserved1b;
	unsigned char dma2_all_mask;

	unsigned char reserve1c[801];

	unsigned char reserved1d;  		/* 0x400 - 0x4ff */
	unsigned char dma1_ch0_curcount_h;
	unsigned char reserved1e;
	unsigned char dma1_ch1_curcount_h;
	unsigned char reserved1f;
	unsigned char dma1_ch2_curcount_h;
	unsigned char reserved20;
	unsigned char dma1_ch3_curcount_h;
	unsigned char reserved21[2];
	unsigned char dma1_set_chain;
	unsigned char dma1_ext_write_mode;
	unsigned char chain_buffer_control;
	unsigned char stepping_level;
	unsigned char reserved22[2];
	unsigned char reserved23[81];
	unsigned char extd_nmi_reset;
	unsigned char nmi_io_interrupt;
	unsigned char reserved24; 	
	unsigned char last_bus_master;
	unsigned char reserved25[27];
	unsigned char reserved26;
	unsigned char dma_highpage_ch2;
	unsigned char dma_highpage_ch3;
	unsigned char dma_highpage_ch1;
	unsigned char reserved27[3];
	unsigned char dma_highpage_ch0;
	unsigned char reserved28;
	unsigned char dma_highpage_ch6;
	unsigned char dma_highpage_ch7;
	unsigned char dma_highpage_ch5;
	unsigned char reserved29[3];
	unsigned char dma_highpage_refresh;
	unsigned char reserved2a[54];
	unsigned char dma2_ch5_curcount_h;
	unsigned char reserved2b[3];
	unsigned char dma2_ch6_curcount_h;
	unsigned char reserved2c[3];
	unsigned char dma2_ch7_curcount_h;
	unsigned char reserved2d;
	unsigned char int1_edge_level;
	unsigned char int2_edge_level;
	unsigned char reserved2e[2];
	unsigned char dma2_set_chain;
	unsigned char reserved2f;
	unsigned char dma2_ext_write_mode;
	unsigned char reserved30[9];
	unsigned char ch0_stop7_2;
	unsigned char ch0_stop15_8;
	unsigned char ch0_stop23_16;
	unsigned char reserved31;
	unsigned char ch1_stop7_2;
	unsigned char ch1_stop15_8;
	unsigned char ch1_stop23_16;
	unsigned char reserved32;
	unsigned char ch2_stop7_2;
	unsigned char ch2_stop15_8;
	unsigned char ch2_stop23_16;
	unsigned char reserved33;
	unsigned char ch3_stop7_2;
	unsigned char ch3_stop15_8;
	unsigned char ch3_stop23_16;
	unsigned char reserved34[5];
	unsigned char ch4_stop7_2;
	unsigned char ch4_stop15_8;
	unsigned char ch4_stop23_16;
	unsigned char reserved35;
	unsigned char ch5_stop7_2;
	unsigned char ch5_stop15_8;
	unsigned char ch5_stop23_16;
	unsigned char reserved36;
	unsigned char ch6_stop7_2;
	unsigned char ch6_stop15_8;
	unsigned char ch6_stop23_16;
	unsigned char reserved37;
 };
