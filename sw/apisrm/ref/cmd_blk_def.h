/*
 * Copyright (C) 1992 by
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
 * Abstract:	AHA1742 SCSI adapter command block definitions
 *
 * Author:	Judy Gold
 *
 * Modifications:
 *
 */


#define aha_reg_base 0xC80

struct aha1742_regs {
	unsigned char hid1;
	unsigned char hid2;
	unsigned char hid3;
	unsigned char hid4;
	unsigned char ebctrl;
	unsigned char aha1742_b_fill1 [59];
	unsigned char portaddr;
	unsigned char biosaddr;
	unsigned char intdef;
	unsigned char scsidef;
	unsigned char busdef;
	unsigned char aha1742_b_fill2 [11];
	unsigned char mboxout0;
	unsigned char mboxout1;
	unsigned char mboxout2;
	unsigned char mboxout3;
	unsigned char attn;
	unsigned char g2cntrl;
	unsigned char g2intst;
	unsigned char g2stat;
	unsigned char mboxin0;
	unsigned char mboxin1;
	unsigned char mboxin2;
	unsigned char mboxin3;
        unsigned char g2stat2;
 };


/* STATUS BLOCK    */

struct scsi_stat{
  short stat;			/* status word */
  char hstat;			/* host adapter status */
  char tstat;			/* target status */
  int rescnt;			/* residual byte count */
  int resaddr;			/* residual buffer address */
  short addstln;		/* additional status length */
  char snsln;			/* sense length */
  char res0;			/* reserved */
  long res1;			/* reserved */
  char cdb[6];			/* target mode CDB */
}; 

volatile struct stat_stat {
  unsigned stat_don  :  1;   	/* command done */
  unsigned stat_du  :  1;     	/* data underrun */
  unsigned stat_reserved0  :  1;/* unused */
  unsigned stat_qf  :  1;	/* adapter queue full */
  unsigned stat_sc  :  1;	/* specification check -- unsupported op code */
  unsigned stat_do  :  1;       /* data overrun */
  unsigned stat_ch  :  1;	/* chaining halted */
  unsigned stat_int :  1;	/* interrupt issued */
  unsigned stat_asa :  1;	/* additional status available */
  unsigned stat_sns :  1;       /* sense information stored */
  unsigned stat_reserved1  :  1;/* unused */
  unsigned stat_ini :  1;	/* initialization required */
  unsigned stat_me  :  1;  	/* major error or exception */
  unsigned stat_reserved2 :  1;/* unused */
  unsigned stat_eca :  1;       /* extended contigent allegiance
  unsigned stat_reserved3 :  1;/* unused */
  };


/* CONTROL BLOCK  */
				
volatile struct scsi_ccb{
  short command;		/* cb command */
  short flag1;			/* flag bits word 1 */
  short flag2;			/* flag bits word 2 */
  short res0;			/* reserved */
  int bufptr;			/* data buffer pointer */
  int bufln;			/* data buffer length */
  int sblkptr;			/* status block pointer */
  short chnaddr;		/* chain address */
  short res1;			/* reserved 1 */
  int res2;			/* reserved 2 */
  int snsiptr;			/* sense information pointer */
  char snsln;			/* sense length */
  char cdbln;			/* cdb length */
  short res3;			/* reserved 3 */
  char cdb[12];			/* cdb commands */
}; 

volatile struct ccb_flag1 {
  unsigned flag1_cne  :  1;   	/* chain no error */
  unsigned flag1_reserved0 : 6; /* unused */
  unsigned flag1_di  :  1;	/* disable interrupt */
  unsigned flag1_reserved1 : 2; /* unused */
  unsigned flag1_ses  :  1;     /* suppress error on underrun */
  unsigned flag1_reserved2 : 1; /* unused */
  unsigned flag1_sg  : 1;       /* scatter/gather */
  unsigned flag1_reserved3 : 1; /* unused */
  unsigned flag1_dsb  :  1;     /* disable status block */
  unsigned flag1_ars  :  1;     /* automatic request sense */
  };

volatile struct ccb_flag2 {
  unsigned flag2_lun  :  3;	/* logical unit number */
  unsigned flag2_tag  :  1;	/* tagged queuing */
  unsigned flag2_tt   :  1;	/* tag type */
  unsigned flag2_nd   :  1;     /* no disconnect */
  unsigned flag2_reserved0  :  1;/* unused */
  unsigned flag2_dat  :  1;	/* check direction of data transfer */
  unsigned flag2_dir  :  1;	/* direction of transfer */
  unsigned flag2_st   :  1;	/* suppress transfer to host memory */
  unsigned flag2_chk  :  1;	/* calculate checksum on data */
  unsigned flag2_reserved1  : 2;/* unused */
  unsigned flag2_rec  :  1;	/* error recovery */
  unsigned flag2_nrb  :  1;     /* no retry on busy status */
  };

