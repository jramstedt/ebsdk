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
 * Abstract:	AHA1742 SCSI adapter chip definitions
 *
 * Author:	Judith E. Gold
 *
 * Modifications:
 *
 */


int adaptec_slot;

#define BASE adaptec_slot		/* base address (slot specific) */

/*--------------------- board configuration registers ---------------------*/

#define HID      (BASE|0xC80)		/* hardware id */
#define EBCTRL   (BASE|0xC84)		/* expansion board control */
#define PORTADDR (BASE|0xCC0)		/* port address */
#define BIOSADDR (BASE|0xCC1)		/* bios address */
#define INTDEF   (BASE|0xCC2)		/* interrupt definition */
#define SCSIDEF  (BASE|0xCC3)		/* SCSI definition */
#define BUSDEF   (BASE|0xCC4)		/* bus definition */


/*--------------------- register bit definitions --------------------------*/

/* EBCTRL -- Expansion Board Control */

#define CDEN   BIT0
#define HAERR  BIT1
#define ERRST  BIT2


/* PORTADDR   */

#define ADR    BIT0|BIT1|BIT2
#define CONFIGURE  BIT6
#define ENHANC_IFACE BIT7


/* BIOSADDR  */

#define BIOSSEL BIT0|BIT1|BIT2|BIT3
#define RAMEN	BIT5
#define BIOSEN  BIT6
#define WRTPRT  BIT7


/* INTDEF -- Interrupt Definition */

#define INTSEL  BIT0|BIT1|BIT2
#define INTHIGH	BIT3
#define INTEN   BIT4
/* interrupt channel */
#define AIRQ_9 0
#define AIRQ_10 1
#define AIRQ_11 2
#define AIRQ_12 3
#define AIRQ_14 5
#define AIRQ_15 6


/* SCSIDEF -- SCSI Definition */

#define HSCSIID BIT0|BIT1|BIT2|BIT3
#define RSTPWR	BIT4


/* BUSDEF -- Bus Definition */

#define BUSON    BIT0|BIT1
#define DMA_CHAN BIT2|BIT3


/*--------------------- mailbox registers ---------------------------------*/

#define MBOXOUT0 (BASE|0xCD0)		/* mailbox out 0 */
#define MBOXOUT1 (BASE|0xCD1)		/* mailbox out 1 */
#define MBOXOUT2 (BASE|0xCD2)		/* mailbox out 2 */
#define MBOXOUT3 (BASE|0xCD3)		/* mailbox out 3 */
#define MBOXIN0  (BASE|0xCD8)		/* mailbox in 0 */
#define MBOXIN1  (BASE|0xCD9)		/* mailbox in 1 */
#define MBOXIN2  (BASE|0xCDA)		/* mailbox in 2 */
#define MBOXIN3  (BASE|0xCDB)		/* mailbox in 3 */
 
/*--------------------- control registers ---------------------------------*/

#define ATTN     (BASE|0xCD4)		/* attention */
#define G2CNTRL  (BASE|0xCD5)		/* enhanced mode control */
#define G2INTST  (BASE|0xCD6)		/* enhanced mode interrupt status */
#define G2STAT   (BASE|0xCD7)		/* enhanced mode status */
#define G2STAT2  (BASE|0xCDC)		/* enhanced mode status 2 */

/*--------------------- register bit definitions --------------------------*/

/* ATTN */

#define TGT_ID BIT0|BIT1|BIT2  
#define OPCODE BIT4|BIT5|BIT6|BIT7
/* Command opcodes */
#define OPICMD 1
#define OPSTRT 4
#define OPABRT 5

/* G2CNTRL */

#define SETRDY	BIT5
#define CLRINT  BIT6
#define HRESET  BIT7


/* G2INTST */

#define STAT_ID  BIT0|BIT1|BIT2
#define INT_STAT BIT4|BIT5|BIT6|BIT7


/* G2STAT */

#define XBUSY   BIT0
#define IPEND   BIT1
#define MOE     BIT2


/* G2STAT2 */

#define HRDY    BIT0



#define ADP_SCSI 0x0009004

#define SCSI_INQUIRY 0
#define SCSI_READ 1
#define SCSI_VERSION 2
#define SCSI_START 3
#define SCSI_UNIT_READY 4
#define SCSI_REQUEST_SENSE 5

#define ID_MASK 0x00ffffff
#define MAXSLOT 6

int sniff_eisa(),init_card();
void display_card_info();
int make_command();
#define xWAIT x=0x1000000;while(--x);

struct _config {
  int inserted;
  int numdrvs;
  int unit[8];
};

int num_cards;
struct _config cards[MAXSLOT];



