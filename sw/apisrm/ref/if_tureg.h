/*
 * @DEC_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: if_tureg.h,v $
 * Revision 1.1.27.2  1996/04/12  15:23:11  Rajul_Shah
 * 	Update with MII definitions for initial support of DE500-AA.
 * 	[1996/04/11  21:18:14  Rajul_Shah]
 *
 * Revision 1.1.21.2  1995/11/06  17:45:34  Rajul_Shah
 * 	Use 16 Longword Burst size and Cache Alignment for DECchips
 * 	21040/1 to workaround a data-corruption problem.
 * 	[1995/11/03  20:06:31  Rajul_Shah]
 * 
 * Revision 1.1.18.2  1995/03/27  18:39:04  Rajul_Shah
 * 	Merge of revision 1.1.16.2 from HW4 BL3 and various changes
 * 	for supporting Pass3 Tulip (de450) and Fast Ethernet (de500).
 * 	[1995/03/27  18:33:19  Rajul_Shah]
 * 
 * Revision 1.1.16.2  1995/03/09  20:59:44  Rajul_Shah
 * 	Added defines for explicit setup of the SIA registers (CSRs
 * 	13, 14, and 15). This is needed to support SW selection of
 * 	the 10Base2 vs. 10Base5. Also added define for TU_EISA_CFDA.
 * 	[1995/03/09  18:05:43  Rajul_Shah]
 * 
 * Revision 1.1.7.2  1994/11/04  21:21:33  Rajul_Shah
 * 	Added definitions to support the new DECchip 21140 (Fast
 * 	Ethernet based on Tulip).
 * 	[1994/11/02  20:43:17  Rajul_Shah]
 * 
 * Revision 1.1.5.4  1994/05/11  19:27:51  Stuart_Hollander
 * 	merge of agoshw2 bl6 to gold pre-bl11
 * 	Revision 1.1.2.7  1994/04/30  18:14:59  Rajul_Shah
 * 	Deleted definition for TU_SIO_ERA --- no longer needed.
 * 	[1994/04/26  17:09:37  Rajul_Shah]
 * 
 * Revision 1.1.5.3  1994/04/19  21:59:06  Stuart_Hollander
 * 	merge agoshw2 bl5 to gold bl10
 * 	Revision 1.1.2.6  1994/04/01  20:05:30  Rajul_Shah
 * 	Deleted definitions for CFCS and CFLT --- these are no
 * 	longer needed.
 * 	[1994/03/21  22:39:06  Rajul_Shah]
 * 
 * Revision 1.1.5.2  1994/01/23  21:15:20  Stuart_Hollander
 * 	Merge from agoshw2 to goldbl8
 * 	[1994/01/19  16:59:56  Stuart_Hollander]
 * 
 * Revision 1.1.4.2  1994/01/18  18:38:41  Rajul_Shah
 * 	Defined TU_SETUP_OK, TU_CFDA_SIO, and TU_SIO_ERA. Helps
 * 	to prevent the usage of actual values in the source code!
 * 
 * Revision 1.1.2.4  1994/01/07  17:13:08  Rajul_Shah
 * 	Eliminated definitions for EISA-Config0 register --- no
 * 	longer needed. Also added some defines for Pass 2 features.
 * 	[1994/01/03  15:20:20  Rajul_Shah]
 * 
 * Revision 1.1.2.3  1993/12/28  21:34:56  Rajul_Shah
 * 	Added define for CSR12 Link-Fail status bit. And removed
 * 	enabling of AUI port in the initial value for CSR13.
 * 	[1993/12/23  21:09:25  Rajul_Shah]
 * 
 * Revision 1.1.2.2  1993/12/17  22:03:48  Rajul_Shah
 * 	Header file for the Tulip Ethernet driver - initial version.
 * 	[1993/12/17  21:42:19  Rajul_Shah]
 * 
 * $EndLog$
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

#define TU_MII_PREAMBLE 0xFFFFFFFF	/* MII Management Preamble - 34 bits */
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


