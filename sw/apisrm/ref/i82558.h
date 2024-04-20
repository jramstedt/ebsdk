/*
 * Copyright (C) 1998 by
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
 * Abstract:	Intel 82558 (EtherExpress Pro/100+) Fast Ethernet Definitions
 *
 * Author:	David Golden
 *
 * Modifications:
 *
 */


/* Subsystem ID definitions */

#define XP_SUBSYS_ID_NC3121     0xB0D7         /* Subsystem ID Curveball */
#define XP_SUBSYS_ID_NC3123	0xB144         /* Subsystem ID Hascal */
#define XP_SUBSYS_ID_NC3131     0xB0DD         /* Subsystem ID Tornado */
#define XP_SUBSYS_ID_NC3132     0xB0DE         /* Subsystem ID Thunder */
#define XP_SUBSYS_ID_NC3133     0xB0E1         /* Subsystem ID El Nino */

/* Control and Status Register offsets */

#define XP_SCB_STATUS   0x0	/* rx and cmd unit status */
#define XP_SCB_CMD	0x2	/* rx and cmd unit commands */
#define XP_SCB_GENPTR	0x4	/* cmd specific address */
#define	XP_PORT		0x8	/* misc chip commands */
#define	XP_FLASH_CTL	0xc	/* flash control register */
#define	XP_EEPROM_CTL	0xe	/* eeprom control register */
#define	XP_MDI_CTL	0x10	/* Management Data Interface control */
#define	XP_RXDMA_CNT	0x14	/* Receive DMA byte count */
#define	XP_EARLYRX_INT	0x18	/* early receive interrupt count */
#define	XP_FC_THRESH	0x19	/* threshold before flow control begins */
#define	XP_FC_XONXOFF	0x1a	/* flow control reg */
#define	XP_PMDR		0x1b	/* wakeup interrupt indicator */


/* System Control Block (SCB) Register Definitions		*/
/* The SCB consists of the SCB Status Word, SCB Command Word,	*/
/* and SCB General Pointer					*/

/* SCB Status Word */

/* Receive Unit Status (bits <5:2> of the status reg) */
/* all other values in the RUS are RESERVED */

#define	XP_SCB_RUS_MASK		    0x3c

#define XP_SCB_RUS_IDLE		    (0x0 << 2)
#define XP_SCB_RUS_SUSPENDED	    (0x1 << 2)
#define XP_SCB_RUS_NORESOURCES	    (0x2 << 2)
#define XP_SCB_RUS_READY	    (0x4 << 2)
#define XP_SCB_RUS_SUSP_NORBDS	    (0x9 << 2)
#define XP_SCB_RUS_NORES_NORBDS	    (0xa << 2)
#define XP_SCB_RUS_READY_NORBDS     (0xc << 2)


/* command unit status (bits <7:6> of the status reg) */
/* all other values for the CUS are RESERVED */

#define XP_SCB_CUS_MASK		    0xc0

#define XP_SCB_CUS_IDLE		    (0 << 6)
#define XP_SCB_CUS_SUSPENDED	    (1 << 6)
#define XP_SCB_CUS_ACTIVE	    (2 << 6)

/* status acknowledge bits (bits <15:8> of the status reg) */

#define	XP_SCB_STATACK_MASK	    0xff00

#define	XP_SCB_STATACK_FCP	    0x0100	    /* Flow control pause */
#define	XP_SCB_STATACK_ER	    0x0200	    /* Early rx intr */
#define XP_SCB_STATACK_SWI	    0x0400	    /* software generated int */
#define XP_SCB_STATACK_MDI	    0x0800	    /* MDI r/w cycle done */
#define XP_SCB_STATACK_RNR	    0x1000	    /* RU is not ready */
#define XP_SCB_STATACK_CNA	    0x2000	    /* CU is not active */
#define XP_SCB_STATACK_FR	    0x4000	    /* Finished rx frame */
#define XP_SCB_STATACK_CX	    0x8000	    /* cmd executed w/intr */

/* SCB Command Word */

/* Interrupt Masks (bits <15-8> of the SCB Command Word) */

#define	XP_SCB_CMD_I_M		    0x0100	    /* disable ALL interrupts */
#define XP_SCB_CMD_I_SWI	    0x0200	    /* generate s/w int */
#define	XP_SCB_CMD_I_FCP	    0x0400	    /* Flow control pause */
#define	XP_SCB_CMD_I_ER		    0x0800	    /* Early rx intr */
#define XP_SCB_CMD_I_RNR	    0x1000	    /* RU is not ready */
#define XP_SCB_CMD_I_CNA	    0x2000	    /* CU is not active */
#define	XP_SCB_CMD_I_FR		    0x4000	    /* Finished rx frame */
#define	XP_SCB_CMD_I_CX		    0x8000	    /* cmd execution complete */

#define	XP_SCB_CMD_I_MASK	    (0xff00 & ~XP_SCB_CMD_I_SWI)
						    /* all ints except SWI */

/* Command Unit opcodes (bits <7:4> of the SCB Command Word) */

#define	XP_SCB_CMD_CU_MASK	0xf0

#define XP_SCB_CMD_CU_NOP	0x00
#define XP_SCB_CMD_CU_START	0x10    /* start processing cmd block lis */
#define XP_SCB_CMD_CU_RESUME    0x20    /* resume processing */
#define XP_SCB_CMD_CU_DUMP_ADR  0x40    /* set statistics dump address */
#define XP_SCB_CMD_CU_DUMP	0x50    /* dump the statistics counters */
#define XP_SCB_CMD_CU_BASE	0x60    /* set command base address */
#define XP_SCB_CMD_CU_DUMPRESET 0x70    /* dump counters and reset */
#define	XP_SCB_CMD_CU_SRESUME	0xa0	/* CU static resume */

/* Receive Unit opcodes (bits <2:0> of the SCB Command Word) */

#define	XP_SCB_CMD_RU_MASK	0x7

#define XP_SCB_CMD_RU_NOP	0
#define XP_SCB_CMD_RU_START	1	    /* Start receiving frames */
#define XP_SCB_CMD_RU_RESUME    2	    /* Resume receive */
#define	XP_SCB_CMD_RU_RDMAREDIR 3	    /* Resume the rcv DMA */
#define XP_SCB_CMD_RU_ABORT	4	    /* Abort receive */
#define XP_SCB_CMD_RU_LOADHDS   5	    /* load header data size */
#define XP_SCB_CMD_RU_BASE	6	    /* set receive cmd base address */
#define XP_SCB_CMD_RU_RBDRESUME 7	    /* Resume frame rx */


/* Port Register */

/* port commands */

#define	XP_PORT_SOFTW_RST   0	/* software reset */
#define	XP_PORT_SELFTEST    1	/* perform selftest */
#define	XP_PORT_SEL_RST	    2	/* selective reset */
#define	XP_PORT_DUMP	    3	/* dump? */

/* Serial EEPROM Control Register */

#define XP_EEPROM_EESK	    0x01    /* shift clock */
#define XP_EEPROM_EECS	    0x02    /* chip select */
#define XP_EEPROM_EEDI	    0x04    /* data in */
#define XP_EEPROM_EEDO	    0x08    /* data out */

/* Serial EEPROM opcodes, including start bit */

#define XP_EEPROM_OPC_ERASE 0x4
#define XP_EEPROM_OPC_WRITE 0x5
#define XP_EEPROM_OPC_READ  0x6


/* Management Data Interface (MDI) Control Register */

/* MDI data */

#define	XP_MDI_DATA_MASK    (0xffff)

/* PHY register address */

#define	XP_MDI_PHY_REG_MASK  (0x1f0000)

/* PHY address */

#define	XP_MDI_PHY_ADR_MASK (0x3e00000)
#define XP_MDI_PHY_REG_MASK (0x01f0000)

/* Management Data Interface opcodes */

#define	XP_MDI_OP_MASK	    (0x3 << 26)

#define XP_MDI_OP_WRITE	    (0x1 << 26)
#define XP_MDI_OP_READ	    (0x2 << 26)

#define	XP_MDI_READY_MASK   (0x10000000)    /* MDI Ready */
#define	XP_MDI_INT_MASK	    (0x20000000)    /* MDI intr enable */

/* PHY device types (as defined by the eeprom spec) */

#define XP_PHY_NONE	    0
#define XP_PHY_82553A	    1
#define XP_PHY_82553C	    2
#define XP_PHY_82503	    3
#define XP_PHY_DP83840	    4
#define XP_PHY_80C240	    5
#define XP_PHY_80C24	    6
#define XP_PHY_82555	    7
#define XP_PHY_DP83840A    10
#define XP_PHY_82555B	    11

#define	XP_PHY_MAX_ADDR	    0x1f

/* PHY Control Register (0) */

#define XP_PHY_CR		0x0	    /* register number */
#define	XP_PHY_CR_RESET		0x8000	    /* reset control */
#define	XP_PHY_CR_LOOPBACK	0x4000	    /* loopback mode */
#define XP_PHY_CR_SPEED_100M	0x2000	    /* 10 mb = 0, 100 mb = 1 */
#define XP_PHY_CR_AUTOEN	0x1000	    /* Auto negotiation enable */
#define	XP_PHY_CR_PWRDOWN	0x0800	    /* Low power mode */
#define	XP_PHY_CR_ISOLATE	0x0400	    /* isolate PHY from MII */
#define	XP_PHY_CR_RSTAUTO	0x0200	    /* restart auto configuration */
#define	XP_PHY_CR_FULLDUP	0x0100	    /* full duplex mode */
#define	XP_PHY_CR_COLLDET	0x0080	    /* collision detect enabled */

/* PHY Status Register (1) */

#define	XP_PHY_SR		0x1
#define	XP_PHY_SR_TXFD		0x4000	    /* TX full duplex capable */
#define	XP_PHY_SR_TXHD		0x2000	    /* TX half duplex capable */
#define	XP_PHY_SR_10FD		0x1000	    /* 10BASE-T full duplex capable */
#define	XP_PHY_SR_10HD		0x0800	    /* 10BASE-T half duplex capable */
#define	XP_PHY_SR_AUTOCPLT	0x0020	    /* auto negotiation complete */
#define	XP_PHY_SR_RFAULT	0x0010	    /* remote fault detect enabled */
#define	XP_PHY_SR_AUTO		0x0008	    /* auto negotiate enabled */
#define	XP_PHY_SR_LINK		0x0004	    /* link status (1 = up) */
#define	XP_PHY_SR_JABBER	0x0002	    /* jabber condition detected */
#define	XP_PHY_SR_EXT		0x0001	    /* extended register capabilities */

/* PHY Identifier Registers 1 & 2 (2, 3) */

#define	XP_PHY_IR1		0x2
#define XP_PHY_IR2		0x3
#define	XP_PHY_MODEL_REVID_MASK	0xfff0ffff

/* Auto negotiation advertisement register (4) */

#define	XP_PHY_AR		0x4
#define	XP_PHY_AR_ACK		0x4000	    /* auto negotiation ack'd */
#define	XP_PHY_AR_RF		0x2000	    /* remote fault */
#define	XP_PHY_AR_100BASET4	0x0200	    /* 100BASE-T4 supported */
#define	XP_PHY_AR_FASTFD	0x0100	    /* 100BASE-Tx full dup supported */
#define	XP_PHY_AR_FAST		0x0080	    /* 100BASE-Tx supported */
#define	XP_PHY_AR_FD		0x0040	    /* 10BASE-Tx full dup supported */
#define	XP_PHY_AR_10BT		0x0020	    /* 10BASE-Tx supported */
#define	XP_PHY_AR_SELMASK	0x001f	    /* selector field mask */
#define	XP_PHY_AR_TAMASK	0x03e0	    /* Technology Ability mask */

/* Auto negotiation link partner ability register (5) */

#define	XP_PHY_LP		0x5
#define	XP_PHY_LP_ACK		0x4000	    /* received partner's code word */
#define	XP_PHY_LP_RF		0x2000	    /* remote fault */
#define	XP_PHY_LP_100BASET4	0x0200	    /* 100BASE-T4 on partner */
#define	XP_PHY_LP_FASTFD	0x0100	    /* 100BASE-Tx full dup supported */
#define	XP_PHY_LP_FAST		0x0080	    /* 100BASE-Tx supported */
#define	XP_PHY_LP_FD		0x0040	    /* 10BASE-Tx full dup supported */
#define	XP_PHY_LP_10BT		0x0020	    /* 10BASE-Tx supported */
#define	XP_PHY_LP_SELMASK	0x001f	    /* selector field mask */
#define	XP_PHY_LP_TAMASK	0x03e0	    /* Technology Ability mask */

/* Auto negotiation Expansion Register (6) */

#define	XP_PHY_ER		0x6
#define	XP_PHY_ER_PFAULT	0x0010	    /* parallel detection fault */
#define	XP_PHY_ER_LPNEXT	0x0008	    /* partner is Next Page able */
#define	XP_PHY_ER_NEXT		0x0004	    /* local dev is Next Page able */
#define	XP_PHY_ER_PAGEREC	0x0002	    /* page received */
#define	XP_PHY_ER_LPAUTO	0x0001	    /* partner is Auto Negotiable */

/* DP84830 PHY, PCS Configuration Register */

#define XP_DP83840_PCR		    0x17
#define XP_DP83840_PCR_LED4_MODE    0x0002  /* 1 = LED4 always indicates full duplex */
#define XP_DP83840_PCR_F_CONNECT    0x0020  /* 1 = force link disconnect function bypass */
#define XP_DP83840_PCR_BIT8	    0x0100
#define XP_DP83840_PCR_BIT10	    0x0400
#define	XP_DP83840_PHY		    0x5c002000	/* ID reg 3 / ID reg 2 */

/* Intel PHY 100 / 82555 Extended Register 0 */

#define	XP_PHY100_SCR		    0x10
#define	XP_PHY100_SCR_FLOW	    0x8000  /* PHY Base flow control */
#define	XP_PHY100_SCR_CSDC	    0x2000  /* Disconnect function */
#define	XP_PHY100_SCR_TXFC	    0x1000  /* Tx flow control */
#define	XP_PHY100_SCR_RDSI	    0x0800  /* Rx Deserializer In Sync */
#define	XP_PHY100_SCR_100PD	    0x0400  /* 100 Base-Tx power down */
#define	XP_PHY100_SCR_10PD	    0x0200  /* 10 Base-Tx power down */
#define	XP_PHY100_SCR_POLARITY	    0x0100  /* 10 Base-T polarity */
#define	XP_PHY100_SCR_T4	    0x0004  /* 100Base-T4 */
#define	XP_PHY100_SCR_FAST	    0x0002  /* 10/100 Mbps indication */
#define	XP_PHY100_SCR_FD	    0x0001  /* full duplex flag */
#define	XP_PHY100_PHY		    0x015002a8	/* ID reg 3 / ID reg 2 */

/* Level One PHY LXT970A Configuration Register */

#define	XP_LXT970_CR		    0x13
#define	XP_LXT970_CR_TXTST	    0x4000  /* Transmit test */
#define	XP_LXT970_CR_RPTMODE	    0x2000  /* Repeater Mode */
#define	XP_LXT970_CR_MDIO_INT	    0x1000  /* Enable interrupts on MDIO */
#define	XP_LXT970_CR_TPLPBK	    0x0800  /* 10bt TP loopback disable */
#define	XP_LXT970_CR_SQE	    0x0400  /* Enable SQE (1) */
#define	XP_LXT970_CR_JABBER	    0x0200  /* Disable jabber (1) */
#define	XP_LXT970_CR_LINKTST	    0x0100  /* Disable 10bt link test (1) */
#define	XP_LXT970_CR_LEDCMASK	    0x00c0  /* LEDC control mask 10 = activ */
#define	XP_LXT970_CR_ADVTXCLK	    0x0020  /* TX clock advanced */
#define	XP_LXT970_CR_5BSYMBOL	    0x0010  /* 5-bit symbol mode */
#define	XP_LXT970_CR_SCRAMBLER	    0x0008  /* Bypass tx/rx scramblers */
#define	XP_LXT970_CR_100BASEFX	    0x0004  /* enable fiber interface */
#define	XP_LXT970_CR_TXDISC	    0x0001  /* disconnect tp tx from line */
#define	XP_LXT970_CR_LEDCACTIV	    0x0080  /* mask to enable activity led */

#define	XP_LXT970_PHY		    0x00007810	/* ID reg 3 / ID reg 2 */


/*  Memory Structures */

/*
 * Control Block (CB) definitions
 */

/* General Action Command */

volatile struct xp_cb {
    volatile unsigned short int cb_status;
    unsigned short int cb_command;
    unsigned int cb_link;
    unsigned char params[1];
};

/* Transmit Command */

volatile struct xp_txcb {
    volatile unsigned short int cb_status;  /* command status */
    unsigned short int cb_command;	    /* command opcode & flags */
    unsigned int cb_link;		    /* link to next command */
    unsigned int cb_tbd_array_ptr;	    /* pointer to the descr. array */
    unsigned short cb_txcb_cnt;		    /* data cnt contained in this pkt */
    unsigned char cb_txthresh;		    /* tx fifo threshold */
    unsigned char cb_tbd_num;		    /* # of transmit buffers in array */

    /* NOTE: in this design, a TBD array is appended directly to the TxCB */
    /* This way we don't need to allocate another block of little structures */
    /* We always have one transmit buffer, thus this array is one entry long */
    /* cb_tbd_num = 1							    */
    /* cb_tbd_array_ptr = &cb_txb0_addr					    */
    /* cb_txcb_cnt = 0							    */

    unsigned int cb_txb0_addr;	    /* Transmit buffer #0 address */
    unsigned short cb_txb0_size;    /* size -- set bit 15 for end-o-list */
    unsigned short cb_txb0_pad;	    /* pad to a longword */

    /* this is used to track which command block this is */
    /* and index the timer queue array */
    unsigned int index;
    unsigned int pad;	    /* just to make 8 longwords */
};

/* configure command */
volatile struct xp_config_cb {
    volatile unsigned short int cb_status;
    unsigned short int cb_command;
    unsigned int cb_link;
    unsigned char config_data[22];
};

/* defaults for the configure command */
/* See the Intel 82558 manual for the details */

#define	XP_CFIG_DEFAULT_BYTE0	22	    /* byte count */
#define	XP_CFIG_DEFAULT_BYTE1	0x88	    /* fifo limits */
#define XP_CFIG_DEFAULT_BYTE2	0	    /* adaptive IFS */
#define XP_CFIG_DEFAULT_BYTE3	0	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE4	0	    /* RX dma min count */
#define XP_CFIG_DEFAULT_BYTE5	0	    /* tx dma min count & enable */
#define XP_CFIG_DEFAULT_BYTE6	0x32	    /* stanardard txcb & stats*/
					    /* discard bad frames & overrun */
#define XP_CFIG_DEFAULT_BYTE7	0x03	    /* discard short frames, */
					    /* underrun retry */
#define XP_CFIG_DEFAULT_BYTE8	0x01	    /* MII default */
#define XP_CFIG_DEFAULT_BYTE9	0	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE10	0x2e	    /* scr addr insertion, preamble */
					    /* loopback default */
#define XP_CFIG_DEFAULT_BYTE11	0x0	    /* linear priority */
#define XP_CFIG_DEFAULT_BYTE12	0x60	    /* IFS, linear priority mode */
#define XP_CFIG_DEFAULT_BYTE13	0	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE14	0xf2	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE15	0xea	    /* no promisc, no broadc, use u/l */
#define XP_CFIG_DEFAULT_BYTE16	0	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE17	0x40	    /* reserved */
#define XP_CFIG_DEFAULT_BYTE18	0xf2	    /* stripping, padding, crc in mem */
#define XP_CFIG_DEFAULT_BYTE19	0x80	    /* Auto-fdx based on int. PHY */
#define XP_CFIG_ALT_BYTE19	0x40	    /* forced full duplex regardless */
#define XP_CFIG_DEFAULT_BYTE20	0x3f	    /* multi-ia */
#define XP_CFIG_DEFAULT_BYTE21	0x05	    /* multicast all */

#define	XP_CFIG_ILOOPBACK_BYTE10 (XP_CFIG_DEFAULT_BYTE10 | 0x40)
#define	XP_CFIG_XLOOPBACK_BYTE10 (XP_CFIG_DEFAULT_BYTE10 | 0x80)

/* Individual Address Setup Command */

volatile struct xp_ias_cb {
    volatile unsigned short int cb_status;
    unsigned short int cb_command;
    unsigned int cb_link;
    unsigned char iaddr[8];	/* only first 6 are used */
};

/* Multicast Setup Command */

volatile struct xp_mcs_cb {
    volatile unsigned short int cb_status;
    unsigned short int cb_command;
    unsigned int cb_link;
    unsigned short int mc_count;	/* count of multicast addr bytes */
    unsigned char mc_addr[6];		/* first mc address */
};

/* Status Word */

#define XP_CB_STATUS_OK		0x2000
#define XP_CB_STATUS_C		0x8000

/* Command Word */

/* commands */

#define	XP_CB_CMD_MASK	    0x7

#define XP_CB_CMD_NOP	    0x0
#define XP_CB_CMD_IAS	    0x1	/* Individual Address Setup */
#define XP_CB_CMD_CONFIG    0x2	/* Configure the chip */
#define XP_CB_CMD_MCAS	    0x3	/* Multicast setup */
#define XP_CB_CMD_XMIT	    0x4	/* Transmit */
#define XP_CB_CMD_RESRV	    0x5	/* */
#define XP_CB_CMD_DUMP	    0x6	/* Dump internal registers */
#define XP_CB_CMD_DIAG	    0x7	/* Selftest */

/* command flags */

#define XP_CB_CMD_SF   0x0008  /* simple/flexible mode */
#define XP_CB_CMD_I    0x2000  /* generate interrupt on completion */
#define XP_CB_CMD_S    0x4000  /* suspend on completion */
#define XP_CB_CMD_EL   0x8000  /* end of list */

/*
 * Receive Frame Area (RFA) definitions
 */

#define	I82558_NULL	0xffffffff	    /* Intel's idea of a NULL pointer */

/* Receive Frame Descriptor (RFD) */

volatile struct xp_rfd {
    volatile unsigned short int rfd_status;	/* results of rx operation */
    volatile unsigned short int rfd_control;	/* control fields */
    volatile unsigned int link_addr;		/* offset to next descr. */
    volatile unsigned int rbd_ptr;		/* data buffer address */
    volatile unsigned short int actual_size;	/* # of bytes written to buf */
    volatile unsigned short int size;		/* total size of the buffer */

};

#define XP_RFD_STAT_RCOL	0x0001  /* receive collision */
#define XP_RFD_STAT_IAMATCH	0x0002	/* 0 = matches station address */
#define XP_RFD_STAT_S4		0x0010  /* receive error from PHY */
#define XP_RFD_STAT_TYPELEN	0x0020  /* type/length */
#define XP_RFD_STAT_FTS		0x0080  /* frame too short */
#define XP_RFD_STAT_OVERRUN	0x0100	/* DMA overrun */
#define XP_RFD_STAT_RNR		0x0200  /* no resources */
#define XP_RFD_STAT_ALIGN	0x0400	/* alignment error */
#define XP_RFD_STAT_CRC		0x0800  /* CRC error */
#define XP_RFD_STAT_OK		0x2000  /* packet received okay */
#define XP_RFD_STAT_CPLT	0x8000  /* packet reception complete */
#define XP_RFD_CONTROL_SF	0x08    /* simple/flexible memory mode */
#define XP_RFD_CONTROL_HDR	0x10    /* header RFD */
#define XP_RFD_CONTROL_SUS	0x4000  /* suspend after reception */
#define XP_RFD_CONTROL_EL	0x8000  /* end of list */

#define	XP_RFD_COUNT_F		0x4000	/* done updating actual size */
#define	XP_RFD_COUNT_EOF	0x8000	/* done writing to the buffer */

/* Receive Buffer Descriptor (RBD) */

volatile struct xp_rbd {
    volatile unsigned short actual_size;    /* # of bytes written to buffer */
    volatile unsigned short pad1;
    volatile unsigned int next_rbd_adr;	    /* pointer to next RBD in list */
    volatile unsigned int rcv_buf_adr;	    /* pointer to the data buffer */
    volatile unsigned short size;	    /* full size of the data buffer */
    volatile unsigned short pad2;

};

#define XP_RBD_SIZE_EL		0x8000	/* end of RBD list */
#define	XP_RBD_COUNT_F		0x4000	/* done updating actual size */
#define	XP_RBD_COUNT_EOF	0x8000	/* done writing to the buffer */

/*
 * Statistics dump area definitions
 */
volatile struct xp_stats {
    volatile unsigned int tx_good;
    volatile unsigned int tx_maxcols;
    volatile unsigned int tx_latecols;
    volatile unsigned int tx_underruns;
    volatile unsigned int tx_lostcrs;
    volatile unsigned int tx_deferred;
    volatile unsigned int tx_single_collisions;
    volatile unsigned int tx_multiple_collisions;
    volatile unsigned int tx_total_collisions;
    volatile unsigned int rx_good;
    volatile unsigned int rx_crc_errors;
    volatile unsigned int rx_alignment_errors;
    volatile unsigned int rx_rnr_errors;
    volatile unsigned int rx_overrun_errors;
    volatile unsigned int rx_cdt_errors;
    volatile unsigned int rx_shortframes;
#if 0
    volatile unsigned int fc_xmt_pause;	/* only if extended statistics are on */
    volatile unsigned int fc_rcv_pause;
    volatile unsigned int fc_rcv_unsupported;
#endif
    volatile unsigned int completion_status;
};
#define XP_STATS_DUMP_COMPLETE	0xa005
#define XP_STATS_DR_COMPLETE	0xa007


/* Intel 82558 128 byte EEPROM format */
/* See Application note AP-369 */

#define	XP_EEPROM_CONN_MII	    0x8
#define XP_EEPROM_CONN_AUI	    0x4
#define XP_EEPROM_CONN_BNC	    0x2
#define	XP_EEPROM_CONN_RJ45    0x1
#define	XP_EEPROM_SIZE	128
#define	XP_EEPROM_PHY_DMASK	    0x3f	/* PHY device type bits */

struct eeprom_fmt {
    unsigned char ia[6];	/* Unique address */
    unsigned char compat0;	/* compatibility byte 0 */
    unsigned char compat1;	/* compatibility byte 1 (bit0 = OEM) */
    unsigned short fill1;
    unsigned char connectors;	/* connectors, MII, AUI, BNC, RJ45 */
    unsigned char type;		/* controller type, 82557, 82558, other */
    unsigned char phy1_adr;	/* address of first PHY */
    unsigned char phy1_dev;	/* bits <5:0> are the PHY type, <7:6> flags */
    unsigned char phy2_adr;	/* address of second PHY */
    unsigned char phy2_dev;	/* bits <5:0> are the PHY type, <7:6> flags */
    unsigned char pwa_label[4];	/* board rev */
    unsigned short fill2;
    unsigned short subsys_id;	/* subsystem ID (OEM) */
    unsigned short vendor_id;	/* vendor ID (OEM) */
    unsigned short fill3[50];
    unsigned short chksum;	/* total of all words = 0xbaba */
};

