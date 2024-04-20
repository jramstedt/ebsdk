/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: TU.H,v $
 * Revision 1.2  2001/01/13 05:23:56  chris
 * #61
 *
 * fixes for EV68 in show config.
 *
 * Revision 1.1.1.1  2001/01/03 16:07:01  Chris.Gearing
 * no message
 *
 * 
 * $EndLog$
 */
  
/*
 * Digital TULIP NI Adapter. This module handles architectures which
 * implement Network Interfaces by a direct programmatic interface to
 * the DIGITAL TULIP chip set. 
 */
  
/*General Definitions*/
#define u_short unsigned short int
#define u_long unsigned long int

/*
 * TULIP ring descriptors
 */
typedef struct _TU_RING  {
   u_long	tu_flags;	/* rcv/xmt status flags */
   u_long	tu_info;	/* rcv/xmt buffer size and info*/
   u_long	tu_bfaddr1;	/* buffer address 1*/
   u_long	tu_bfaddr2;	/* buffer address 1*/
} TU_RING,*PTU_RING;

/* 
 * TULIP Own bit (RDES0<31> & TDES0<31>)  
 */
#define TU_OWN		0x80000000	/* Own bit=1 TULIP owns ,=0 host owns */

/*
 * TULIP Buffer size 1 (RDES1<10:0> & TDES1<10:0>)
 */
#define TU_RDES_BS1_MSK 0x000007ff 	/* Buffer size 1 mask		*/
#define TU_TDES_BS1_MSK 0x000007ff 	/* Buffer size 1 mask		*/

/*
 * TULIP Buffer size 2 (RDES1<21:11> & TDES1<21:11>)
 */
#define TU_RDES_BS2_MSK 0x003ff800 	/* Buffer size 2 mask		*/
#define TU_RDES_BS2_SFT 11		/* Buffer size 2 shift		*/
#define TU_TDES_BS2_MSK 0x003ff800 	/* Buffer size 2 mask		*/
#define TU_TDES_BS2_SFT 11		/* Buffer size 2 shift		*/

/*
 * TULIP receive status (RDES0<00:15>)
 */
#define TU_OF 		0x00000001	/* Overflow                         */
#define TU_CE 		0x00000002	/* CRC Error                        */
#define TU_DB 		0x00000004	/* Dribbling Bit                    */
#define TU_RJ 		0x00000010	/* Receive Watchdog                 */
#define TU_FT 		0x00000020	/* Frame Type                       */
#define TU_CS 		0x00000040	/* Collision Seen                   */
#define TU_TL 		0x00000080	/* Frame too long                   */
#define TU_RDES_LS	0x00000100	/* Last Descriptor                  */
#define TU_RDES_FS	0x00000200	/* First Descriptor                 */
#define TU_MF 		0x00000400	/* Multicast Frame                  */
#define TU_RF 		0x00000800	/* Runt Frame                       */
#define TU_LE 		0x00004000	/* Length Error                     */
#define TU_RDES_ES  	0x00008000	/* Error Summary                    */

/*
 * TULIP Recieve frame length (RDES0<16:30>)
 */
#define TU_RDES_FL_MSK 0x7fff0000 	/* Frame length mask		    */
#define TU_RDES_FL_SFT 16		/* Frame length shift		    */


/*
 * TULIP Recieve info (RDES1<24:25>)
 */
#define TU_RCH		0x01000000	/* Second address Chained           */
#define TU_RER		0x02000000	/* Receive End of Ring              */

/*
 * SGEC transmit status (TDES0<00:15>)
 */
#define TU_DE		0x00000001	/* Deferred                         */
#define TU_UF		0x00000002	/* Underflow error                  */
#define TU_LF		0x00000004	/* Link Fail                        */
#define TU_HF		0x00000080	/* Heartbeat Fail                   */
#define TU_EC		0x00000100	/* Excessive Collisions             */
#define TU_LC		0x00000200	/* Late Collisions                  */
#define TU_NC		0x00000400	/* No Carrier                       */
#define TU_LO		0x00000800	/* Loss of Carrier                  */
#define TU_TO		0x00004000	/* Transmit Jabber Time-out         */
#define TU_TDES_ES	0x00008000	/* Error Summary                    */

/*  
 * TULIP Collision Count (TDES0<6:3>)
 */

#define TU_CC_MSK	0x00000078	/* Collision Count Mask             */
#define TU_CC_SFT 	3         	/* Collision Count Shift            */

/*  
 * TULIP Transmit info (TDES1<22:31>)
 */
#define TU_HP		0x00400000 	/* Hash/Perfect Filtering           */
#define TU_DPD		0x00800000 	/* Disable Padding                  */
#define TU_TCH		0x01000000 	/* Second Address Chained           */
#define TU_TER		0x02000000 	/* Transmit End of Ring             */
#define TU_AC		0x04000000 	/* Add CRC Disable                  */
#define TU_SET		0x08000000 	/* Setup Packet                     */
#define TU_IV		0x10000000 	/* Inverse Filtering                */
#define TU_TDES_FS	0x20000000 	/* First Segment                    */
#define TU_TDES_LS	0x40000000 	/* Last Segment                     */
#define TU_IC		0x80000000 	/* Interrupt on Completion          */

/* 
 * TULIP packet Data type 
 */
#define TU_DT		0x00000300	/* Mask for the Data type      	    */

  /* Transmit Data Type */
#define TU_DT_TNOR	0x00000000   	/* Normal Transmit Frame Data      */
#define TU_DT_TSET	0x00002000   	/* Setup Frame	                    */
#define TU_DT_TDIA	0x00003000   	/* Diagnostic Frame                */

  /* Receive Data Type */
#define TU_DT_RSRF	0x00000000  	/* Serial Received Frame            */
#define TU_DT_RILF	0x00001000  	/* Internally Looped back Frame     */
#define TU_DT_RELF	0x00002000  	/* External looped back Frame       */

/*
 * TULIP Bus Mode register (CSR0)
 */
#define TU_CSR0_SWR	0x00000001	/* Software reset                   */
#define TU_CSR0_BAR	0x00000002	/* Bus arbitration                  */
#define TU_CSR0_DSL	0x0000007c	/* Descriptor Skip Length           */
#define TU_CSR0_BLE	0x00000080	/* Big/Little Endian                */
#define TU_CSR0_PBL1	0x00000100	/* Programmable Burst Length (1)    */
#define TU_CSR0_PBL2	0x00000200	/* Programmable Burst Length (8)    */
#define TU_CSR0_PBL4	0x00000400	/* Programmable Burst Length (8)    */
#define TU_CSR0_PBL8	0x00000800	/* Programmable Burst Length (8)    */
#define TU_CSR0_CAL1	0x00004000	/* Cache Alignment (1)              */


/* Programmable Burst Lengths */
#define TU_PBL_0	(0 << 8)	/* */
#define TU_PBL_1	(1 << 8)	/* */
#define TU_PBL_2	(2 << 8)	/* */
#define TU_PBL_4	(4 << 8)	/* */
#define TU_PBL_8	(8 << 8)	/* */
#define TU_PBL_16	(16 << 8)	/* */
#define TU_PBL_32	(32 << 8)	/* */

/* Cache Alignment values */
#define TU_CAL_0	(0 << 14)	/* No address alignment	        */
#define TU_CAL_8	(1 << 14)	/* 8 lw address alignment       */
#define TU_CAL_16	(2 << 14)	/* 16 lw address alignment      */
#define TU_CAL_32	(3 << 14)	/* 32 lw address alignment      */

/* Transmit Auto Polling intervals */
#define	TU_TAP_0	(0 << 17)	/* No Auto polling		*/
#define	TU_TAP_200	(1 << 17)	/* Auto polling every 200 usec 	*/
#define	TU_TAP_800	(2 << 17)	/* Auto polling every 800 usec 	*/
#define	TU_TAP_1600	(3 << 17)	/* Auto polling every 1600 usec	*/

#define	TU_CSR0_INIT	(TU_CSR0_BAR | TU_PBL_32 | TU_CAL_32)

/* 
 * TULIP Transmit Polling Demand (CSR1)
 */
#define TU_CSR1_PD	0x00000001	/* Tx Polling Demand            */ 

/* 
 * TULIP Receive Polling Demand (CSR2)
 */
#define TU_CSR2_PD	0x00000001	/* Rx Polling Demand 		*/

/*
 * TULIP Descriptor List addresses
 *	CSR3 - Start of receive list
 *	CSR4 - Start of send list
 *  Initially, CSR3 and CSR4 must be written before the start command
 *  These are physical addresses (??).
 */


/* 
 * TGEC Status Register (CSR5)
 */
#define TU_CSR5_TI	0x00000001	/* Transmit Interrupt               */
#define TU_CSR5_TPS	0x00000002	/* Transmit Process Stopped         */
#define TU_CSR5_TU	0x00000004	/* Transmit Buffer Unavailable      */
#define TU_CSR5_ANC	0x00000010	/* Link pass/Autonegotiation complet*/
#define TU_CSR5_TJT	0x00000008	/* Transmit Jabber Time-Out         */
#define TU_CSR5_UNF	0x00000020	/* Transmit Underflow               */
#define TU_CSR5_RI	0x00000040	/* Recieve Interrupt                */
#define TU_CSR5_RU	0x00000080	/* Recieve Buffer Unavailable       */
#define TU_CSR5_RPS	0x00000100	/* Receive Process Stopped          */
#define TU_CSR5_RWT	0x00000200	/* Recieve Watchdog Time-Out        */
#define TU_CSR5_AT	0x00000400	/* AUI/TP Switch                    */
#define TU_CSR5_FD	0x00000800	/* Full Duplex Short Frame Received */
#define TU_CSR5_LNF	0x00001000	/* Link Fail                        */
#define TU_CSR5_SE	0x00002000	/* System Error                     */
#define TU_CSR5_AIS	0x00008000	/* Abnormal Interrupt Summary       */
#define TU_CSR5_NIS	0x00010000	/* Normal Interrupt Summary         */

/* Reception process State */
#define TU_CSR5_RS	0x000e0000	/* Receive Process State            */
#define TU_RS_STP	0x00000000	/* Stoppped		            */
#define TU_RS_FTC 	0x00020000	/* Fetch Rx descriptor	            */
#define TU_RS_WT 	0x00060000	/* Wait for recieve packet          */
#define TU_RS_SUP	0x00080000	/* As result of Rx buf unavailable  */
#define TU_RS_CLS	0x000a0000	/* Close rx descriptor	            */
#define TU_RS_FLS	0x000c0000	/* Flush the current frame          */
#define TU_RS_DEQ	0x000e0000	/* Deque the rx frame RxFIFO->rxbuf */

/* Transmit process State */
#define TU_CSR5_TS	0x00700000	/* Transmission Process State       */
#define TU_TS_STP	0x00000000	/* Stoppped		            */
#define TU_TS_FTC	0x00100000	/* Fetch Tx descriptor	            */
#define TU_TS_WT 	0x00200000	/* Wait for end of transmission     */
#define TU_TS_ENQ	0x00300000	/* Read buf from mem and enqueue    */
#define TU_TS_SET	0x00500000	/* Running setup packet	            */
#define TU_TS_SUP	0x00600000	/* Tx buf unavailable or Underflow  */
#define TU_TS_CLS	0x00700000	/* Close Tx descriptor              */

/* Error Bits */
#define TU_CSR5_EB	0x03800000	/* Error Bits                       */
#define TU_EB_PE	0x00000000	/* Parity Error		            */
#define TU_EB_MA	0x00800000	/* Master Abort       	            */
#define TU_EB_TA 	0x01000000	/* Target Abort                     */

/*
 * TGEC Serial Command Register (CSR6)
 */
#define TU_CSR6_HP	0x00000001	/* Hash/Perfect Recieve Filtering Mode*/
#define TU_CSR6_SR	0x00000002	/* Start/Stop Recieve               */
#define TU_CSR6_PB	0x00000008	/* Pass Bad Frames                  */
#define TU_CSR6_IF	0x00000010	/* Inverse Filtering                */
#define TU_CSR6_SB	0x00000020	/* Start/Stop Backoff Counter       */
#define TU_CSR6_PR	0x00000040	/* Promiscuous Mode                 */
#define TU_CSR6_PM	0x00000080	/* Pass All Multicast               */
#define TU_CSR6_FKD	0x00000100	/* Flaky Oscillator Disable         */
#define TU_CSR6_FD	0x00000200	/* Full Duplex Operating Mode       */
#define TU_CSR6_FC	0x00001000	/* Force Collision Mode             */
#define TU_CSR6_ST	0x00002000	/* Start Transmission Command       */


/* DECchip 21140A - FasterNet definitions  */
#define TU_CSR6_MBO	0x02000000	/* bit<25>must be one  */
#define TU_CSR6_MDRA    0x02440000      /* bit<25>must be one, MII/SYM Data Rate */
#define TU_CSR6_FASTA	(TU_CSR6_PS | TU_CSR6_SF | TU_CSR6_HBD | TU_CSR6_MBO )
#define TU_CSR6_SLOWA   (TU_CSR6_MDRA | TU_CSR6_SF)

/* DECchip 21140 - FasterNet definitions only */
#define TU_CSR6_PS	0x00040000	/* Port Select(10/100)		*/
#define TU_CSR6_HBD	0x00080000	/* HeartBeat Disable		*/
#define TU_CSR6_IMM	0x00100000	/* Immediate Transmission	*/
#define TU_CSR6_SF	0x00200000	/* Store & Forward		*/
#define TU_CSR6_TTM	0x00400000	/* MII/SYM Data Rate		*/
#define TU_CSR6_MDR	0x00400000	/* MII/SYM Data Rate		*/
#define TU_CSR6_PCS	0x00800000	/* Port CS Mode			*/
#define TU_CSR6_SCR	0x01000000	/* Scrambler Mode		*/
/* DECchip 21041 only */
#define TU_CSR6_SC 	0x80000000	/* Special Capture Effect	*/

#define	TU_CSR6_SLOW	(TU_CSR6_MDR | TU_CSR6_SF)
#define	TU_CSR6_FAST	(TU_CSR6_PS | TU_CSR6_HBD | TU_CSR6_PCS | TU_CSR6_SCR)

/* Operation Mode */
#define TU_CSR6_OM 	0x00000c00	/* Operating Mode                   */
#define TU_OM6_NOR	0x00000000	/* Normal Operation Mode            */
#define TU_OM6_INL	0x00000400	/* Internal Loopback                */
#define TU_OM6_EXL	0x00000800	/* External Loopback                */
#define TU_OM6_DIA	0x00000c00	/* Diagnostic Mode	            */

/* Threshold control bits */
#define TU_CSR6_TR	0x0000c000	/* Threshold Control Bits           */
#define TU_TC_72	0x00000000	/* 72 bytes			    */
#define TU_TC_96	0x00004000	/* 96 bytes			    */
#define TU_TC_128	0x00008000	/* 128 bytes			    */
#define TU_TC_160	0x0000c000	/* 160 bytes			    */

#define	TU_CSR6_INIT	(TU_TC_160)	/* MUST be MAX for 21040 to	*/
#define TU_CSR6_STOP	0xffffdffd	/* and with CSR6 to stop xmit and rcv */

/*
 * Interrupt Mask Register (CSR7)
 */
#define TU_CSR7_TIM	0x00000001	/* Transmit Interrupt Mask          */
#define TU_CSR7_TSM	0x00000002	/* Transmit Stopped Mask            */
#define TU_CSR7_TUM	0x00000004	/* Transmit Buffer Unavailable Mask */
#define TU_CSR7_TJM	0x00000008	/* Transmit Jabber Time-Out Mask    */
#define TU_CSR7_UNM	0x00000020	/* Underflow Mask                   */
#define TU_CSR7_RIM	0x00000040	/* Recieve Interrupt Mask           */
#define TU_CSR7_RUM	0x00000080	/* Recieve Buffer Unavailable Mask  */
#define TU_CSR7_RSM	0x00000100	/* Receive Stopped Mask             */
#define TU_CSR7_RWM	0x00000200	/* Recieve Watchdog Time-Out Mask   */
#define TU_CSR7_ATM	0x00000400	/* AUI/TP Switch Mask               */
#define TU_CSR7_FDM	0x00000800	/* Full Duplex Mask                 */
#define TU_CSR7_LFM	0x00001000	/* Link Fail Mask                   */
#define TU_CSR7_SEM	0x00002000	/* System Error Mask                */
#define TU_CSR7_AIM	0x00008000	/* Abnormal Interrupt Summary Mask  */
#define TU_CSR7_NIM	0x00010000	/* Normal Interrupt Summary Mask    */

/*
 * Missed Frame Register  (CSR8)
 */
#define TU_CSR8_MFC	0x0000ffff      /* Missed Frame counter             */
#define TU_CSR8_MFO	0x00010000	/* Missed Frame Overflow            */

/*
 * Address Mode and Diagnostic Register (CSR9)
 */
#define TU_CSR9_DN	0x80000000	/* Data not valid                   */
#define TU_CSR9_DV_MAX	1000		/* Data not valid max wait	    */
#define TU_CSR9_DV_WT 	1		/* Data not valid wait msec	    */


/* 
 * CSR12 21140 
 */
#define	TU_CSR12_PINS_INIT	0x0000011F	/* Gen Purpose Pin directions */
#define	TU_CSR12_INIT_FAST_HD	0x00000009	/* Half-Duplex, no loopback */
#define	TU_CSR12_INIT_SLOW_HD	0x00000008	/* As above, but for 10 Mbps */
#define	TU_CSR12_INIT_FAST_FD	0x00000001	/* Full-Duplex, no loopback */
#define	TU_CSR12_INIT_SLOW_FD	0x00000000	/* As above, but for 10 Mbps */
#define TU_CSR12_INIT_BNC	0x00000018	/* BNC mode on p2se+	    */
#define	TU_CSR12_SYM_LINK	0x00000040	/* Reset if 100Mpbs link ok */
#define	TU_CSR12_LNK_10		0x00000080	/* Reset if 10 Mbps link ok */

/*
 * Diagnostic Data Register (CSR10)
 */

/*
 * Full Duplex Register (CSR11)
 */
#define	TU_CSR11_FD	0x0000ffff	/* Full Duplex autoconfiguration value*/

/*
 * SIA Status Register (CSR12)
 */
#define TU_CSR12_PAUI	0x00000001	/* PIN AUI TP indication            */
#define TU_CSR12_NCR	0x00000002	/* Network Connection Error         */
#define TU_CSR12_LKF	0x00000004	/* Linkfail status                  */
#define TU_CSR12_APS	0x00000008	/* Auto Polarity State              */
#define TU_CSR12_DSD	0x00000010	/* DPLL self test done              */
#define TU_CSR12_DSP	0x00000020	/* DPLL self test pass              */
#define TU_CSR12_DAZ	0x00000040	/* DPLL all zero                    */
#define TU_CSR12_DAO	0x00000080	/* DPLL all one                     */



/*
 * SIA connectivity Register (CSR13)
 */                         
#define TU_CSR13_SRL	0x00000001	/* SIA reset                        */
#define TU_CSR13_PS	0x00000002	/* PIN AUI/TP Selection             */
#define TU_CSR13_CAC	0x00000004	/* CSR Auto Configuration           */
#define TU_CSR13_AUI	0x00000008	/* 10BaseT or AUI                   */
#define TU_CSR13_EDP	0x00000010	/* SIA DPLL External input enable   */
#define TU_CSR13_ENI	0x00000020	/* Encoder Input Mux                */
#define TU_CSR13_SIM	0x00000040	/* Serial Interface Mux             */
#define TU_CSR13_SEL	0x00000f00      /* External Port Output Mux Select  */
#define TU_CSR13_IE	0x00001000	/* Input Enable                     */
#define TU_CSR13_OE13	0x00002000	/* Output Enable 1 3                */
#define TU_CSR13_OE24	0x00004000	/* Output Enable 2 4                */
#define TU_CSR13_OE57	0x00008000	/* Output Enable 5 6 7              */



/*
 * pass3 tulip (21041) defines
 */
#define TU_CSR13_UTP_INIT_PG3	0xEF01
#define TU_CSR13_FDX_INIT_PG3	0xEF01
#define TU_CSR13_BNC_INIT_PG3	0xEF09
#define TU_CSR13_AUI_INIT_PG3	0xEF09

#define TU_CSR14_UTP_INIT_PG3	0x7B3D
#define TU_CSR14_FDX_INIT_PG3	0x7F3D
#define TU_CSR14_BNC_INIT_PG3	0x0705
#define TU_CSR14_AUI_INIT_PG3	0x0705

#define TU_CSR15_UTP_INIT_PG3	0x0008
#define TU_CSR15_FDX_INIT_PG3	0x0008
#define TU_CSR15_BNC_INIT_PG3	0x0006
#define TU_CSR15_AUI_INIT_PG3	0x000E


/*
 * SIA Transmit Receive Register (CSR14)
 */
#define TU_CSR14_ECEN	0x00000001	/* Encoder Enable                   */
#define TU_CSR14_LBK	0x00000002	/* Loopback Enable                  */
#define TU_CSR14_DREN	0x00000004	/* Driver Enable                    */
#define TU_CSR14_LSE	0x00000008	/* Link Pulse Send Enable           */
#define TU_CSR14_CPEN	0x00000030	/* Compensation Enable              */
#define TU_CSR14_RSQ	0x00000100      /* Receive Squelch Enable           */
#define TU_CSR14_CSQ	0x00000200	/* Collision Squelch Enable         */
#define TU_CSR14_CLD	0x00000400	/* Collision Detect Enable          */
#define TU_CSR14_SQE	0x00000800	/* Signal Quality Generate Enable   */
#define TU_CSR14_LTE	0x00001000	/* Link Test Enable                 */
#define TU_CSR14_APE	0x00002000	/* Auto Polarity Enable             */
#define TU_CSR14_SPP	0x00004000	/* Set Polarity Plus                */

/*
 Configuration ID Register
 */
#define TU_CFID_VENDOR_ID	0x0000ffff/* Vendor id                       */
#define TU_CFID_DEVICE_ID	0xffff0000/* Device id                       */

/*
 * General Configuration Register
 */
#define TU_CFCS_IOA	0x00000001	/* I/O Space Access                 */
#define TU_CFCS_MSA	0x00000002	/* Memory Space Access              */
#define TU_CFCS_MO	0x00000004	/* Master Operation                 */
#define TU_CFCS_PER	0x00000040	/* Parity Error response            */
#define TU_CFCS_DT	0x06000000	/* DEVSEL timing                    */
#define TU_CFCS_RTA	0x10000000	/* Received Target Abort            */
#define TU_CFCS_RMA	0x20000000	/* Received Master Abort            */
#define TU_CFCS_DPE	0x80000000	/* Detected Parity Error            */

/*
 * Configuration Revision Register
 */
#define TU_CFRV_ID	0x000000ff	/* Revision ID                      */

/*
 * Configuration Latency Timer Register
 */
#define TU_CFLT_CLT	    0x0000ff00	/* Configuration Latency Timer      */
#define TU_CFLT_VALUE       0x0000ff00  /* Latency Timer Value              */
/*
 * Configuration base io address
 */
#define TU_CBIO_MIO	0x00000001	/* Memory I/O                       */
#define TU_CBIO_CBIO	0xfffffc00	/* Configuration base io address    */
/*
 * Configuration driver area register
 */
#define TU_CFDA_STDIO	0x00000100	/* Sable STD I/O Tulip              */

/*
 * Expansion Board Control Register
 */
#define TU_EBC_ENA	0x00000001	/* Expansion Board Enable           */

/*
 * EISA Register 0
 */
#define TU_EREG0_ITP	0x01    	/* Interrupt Type                   */
#define TU_EREG0_IRQ0	0x00            /* Interrupt Request 0              */
#define TU_EREG0_IRQ1	0x02    	/* Interrupt Request 1              */
#define TU_EREG0_IRQ2	0x04    	/* Interrupt Request 2              */
#define TU_EREG0_IRQ3	0x06	        /* Interrupt Request 3              */
#define TU_EREG0_ILT	0x08    	/* Interrupt Latched                */
#define TU_EREG0_LT	0x10	        /* Latched interrupt                */
#define TU_EREG0_PR	0x20    	/* PREEMPT Time                     */
#define TU_EREG0_BM	0x40	        /* Bus Master data size             */
#define TU_EREG0_SIO	0x80    	/* Slave I/O data size              */

/*
 * EISA Register 1
 */
#define TU_EREG1_US0	0x00000001	/* User Pin 0                       */
#define TU_EREG1_US1	0x00000002	/* User Pin 1                       */
#define TU_EREG1_US2	0x00000004	/* User Pin 2                       */
#define TU_EREG1_US3	0x00000008	/* User Pin 3                       */
#define TU_EREG1_IOA	0x00000010	/* Isa Address                      */
#define TU_EREG1_IDIS	0x000000e0	/* Isa Address disabled             */

/*
 * EISA Register 2
 */
#define TU_EREG2_BP13	0x00000004	/* BIOS PROM Address Bit 13         */
#define TU_EREG2_BP14	0x00000008	/* BIOS PROM Address Bit 14         */
#define TU_EREG2_BP15	0x00000010	/* BIOS PROM Address Bit 15         */
#define TU_EREG2_BP16	0x00000020	/* BIOS PROM Address Bit 16         */
#define TU_EREG2_BPSZ	0x000000c0	/* BIOS PROM Size/Disable           */

/*
 * EISA Register 3
 */
#define TU_EREG3_SR	0x00000002	/* Software Reset                   */
#define TU_EREG3_BR	0x00000004	/* Burst Read                       */
#define TU_EREG3_BW	0x00000040	/* Burst Write                      */

/*
 * EISA Expansion Board Control, 
 */
#define TU_EBC_ADDR		0x0c84	/* EBC Register			    */

/*
 * EISA Configuration registers
 */

#define TU_REG0_ADDR		0x0c88	/*                                  */
#define TU_REG1_ADDR		0x0c89	/*                                  */
#define TU_REG2_ADDR		0x0c8a	/*                                  */
#define TU_REG3_ADDR		0x0c8f	/*                                  */

/*
 * EISA ID registers
 */
#define TU_EISA_ID_ADDR		0x0c80	/*                                  */

/*
 * TULIP Evaluation Board mask and ID
 */
#define TU_EISA_ID_MASK         0xffffffff
#define TU_EISA_ID              0x30a310

/*
 * Station address rom
 */
#define TU_SA_ADDR		0x0c90	/* Tulip Station address ROM 	*/

/*
 * Register offsets
 */

/*
 * Tulip Registers
 */

#define TU_CSR0_ADDR		0x0000	/* Bus Mode Register                */
#define TU_CSR1_ADDR		0x0008	/* Transmit Poll Register           */
#define TU_CSR2_ADDR		0x0010	/* Recieve Poll Register            */
#define TU_CSR3_ADDR		0x0018	/* Receive Base Register            */
#define TU_CSR4_ADDR		0x0020	/* Transmit Base Register           */
#define TU_CSR5_ADDR		0x0028	/* Status Register                  */
#define TU_CSR6_ADDR		0x0030	/* Serial Command Register          */
#define TU_CSR7_ADDR		0x0038	/* Interrupt Mask Register          */
#define TU_CSR8_ADDR		0x0040	/* Missed Frame Counter Register    */
#define TU_CSR9_ADDR		0x0048	/* Diag address                     */
#define TU_CSR10_ADDR		0x0050	/* Diag data                        */
#define TU_CSR11_ADDR		0x0058	/* Full Duplex                      */
#define TU_CSR12_ADDR		0x0060	/* SIA Status                       */
#define TU_CSR13_ADDR		0x0068	/* SIA Connectivity                 */
#define TU_CSR14_ADDR		0x0070	/* SIA Transmit Receive Register    */
#define TU_CSR15_ADDR		0x0078	/* SIA General Register             */

/*
 * PCI Configuration registers EISA/TULIP
 */
#define TU_CFID_ADDR		0x0008	/*                                  */
#define TU_CFCS_ADDR		0x000C	/*                                  */
#define TU_CFRV_ADDR		0x0018	/*                                  */
#define TU_CFLT_ADDR		0x001C	/*                                  */
#define TU_CBIO_ADDR		0x0028	/*                                  */
#define TU_CFDA_ADDR		0x0088	/*                                  */

/*
 * PCI Configuration registers PCI/TULIP
 */
#define TU_PCFRV_ADDR		0x0008	/*                                  */
#define TU_PCFDA_ADDR		0x0040	/*                                  */

/*
 * Misc
 */
#define TU_SETUP_FRAME_LEN	192	/* Setup frame length		*/

/* The following #defines are needed for using the MII on fast ethernet cards.
 * Currently used for DE500-AA only.
 */
#define TU_MII_MDI	0x00080000	/* MII Management Data In */
#define TU_MII_MDO	0x00060000	/* MII Management Mode/Data Out */
#define TU_MII_MRD	0x00040000	/* MII Management Define Read Mode */
#define TU_MII_MWR	0x00000000	/* MII Management Define Write Mode */
#define TU_MII_MDT	0x00020000	/* MII Management Data Out */
#define TU_MII_MDC	0x00010000	/* MII Management Clock */
#define TU_MII_RD	0x00004000	/* Read from MII */
#define TU_MII_WR	0x00002000	/* Write to MII */
#define TU_MII_SEL	0x00000800	/* Select MII when RESET */

#define TU_MII_PREAMBLE 0xffffffff	/* MII Management Preamble - 34 bits */
#define TU_MII_STARTRD  0x06		/* Start of Frame+Read Op Code: */
#define TU_MII_STARTWR  0x0A		/* Start of Frame+Write Op Code: */

/*
 * MII Registers
 */
#define TU_MII_CR	0x00		/* MII Management Control Register */
#define TU_MII_SR	0x01		/* MII Management Status Register */
#define TU_MII_ID	0x02		/* PHY Identifier Register (0) */
#define TU_MII_AR	0x04		/* Nway Advertisement Register */
#define TU_MII_PR	0x05		/* Nway Link Partner Ability Register */
#define TU_MII_ER	0x06		/* Nway Expansion Register */

/*
 * MII Control register
 */
#define TU_MII_CR_RST	0x8000		/* RESET the PHY chip */
#define TU_MII_CR_LPBK	0x4000		/* Loopback enable */
#define TU_MII_CR_10	0x0000		/* Set 10Mb/s */
#define TU_MII_CR_100	0x2000		/* Set 100Mb/s */
#define TU_MII_CR_ANE	0x1000		/* Auto Negotiation Enable */
#define TU_MII_CR_PD	0x0800		/* Power Down */
#define TU_MII_CR_ISOL	0x0400		/* Isolate Mode */
#define TU_MII_CR_RAN	0x0200		/* Restart Auto Negotiation */
#define TU_MII_CR_FDX	0x0100		/* Full Duplex Mode */
#define TU_MII_CR_CTE	0x0080		/* Collision Test Enable */

/*
 * MII Status register
 */
#define TU_MII_SR_T4C	0x8000		/* 100BASE-T4 capable */
#define TU_MII_SR_TXFD	0x4000		/* 100BASE-TX Full Duplex capable */
#define TU_MII_SR_TXHD	0x2000		/* 100BASE-TX Half Duplex capable */
#define TU_MII_SR_TFD	0x1000		/* 10BASE-T Full Duplex capable */
#define TU_MII_SR_THD	0x0800		/* 10BASE-T Half Duplex capable */
#define TU_MII_SR_TAF	0xf800		/* Technology Ability Field */
#define TU_MII_SR_ANC	0x0020		/* Auto Negotiation Complete*/
#define TU_MII_SR_RFD	0x0010		/* Remote Fault Detected */
#define TU_MII_SR_LKS	0x0004		/* Link Status */
#define TU_MII_SR_JABD	0x0002		/* Jabber Detect */

/*   Controller types (since each handles things different even though using
 *   same chip.)
 */

#define TU_EISA  1
#define TU_21040 2
#define TU_21041 3
#define TU_21142 4
#define DE500XA  5
#define DE500AA  6
#define P2SEPLUS 7
#define DE500BA  8
#define TU_21143 9
#define DE500FA  10
