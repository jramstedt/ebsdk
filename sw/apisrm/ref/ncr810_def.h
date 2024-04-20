/* File:	ncr810_def.h
 *
 * Copyright (C) 1993 by
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
 * Abstract:	NCR 53C810 PCI-SCSI IO Processor header definitions 
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dch	27-SEP-1993	Conditionalized for Medulla.
 *
 *	dwn	21-May-1993	Initial Entry
 */
 
/* LOCAL SYMBOLS and Macro definitions
*/
#define INTERNAL_LOOPBACK 1	/* selects internal loopback testing */
#define EXTERNAL_LOOPBACK 0	/* selects external loopback testing */
#define SCSI_TO		  100000 /* selects external loopback testing */

/* MNCR53810 CUTTHROAT Chip CSR PCI offset definitions
*/
#if SABLE
#define NCR810_PCI_BASE  0x21000    /* ncr810 pci base 2nd 64k block */
#define NCR810_REG_INC   0x01       /* ncr810 register increment */
#endif

#if MEDULLA
#define NCR810_PCI_BASE  0x200000   /* ncr810 pci base in memory space */
#define NCR810_REG_INC   0x01       /* ncr810 register increment */
#endif

/* PCI Register offsets
*/
#define NCR810_OSCNTL0  0x00	/* SCNTL0 SCSI Control 0 */
#define NCR810_OSCNTL1  0x01	/* SCNTL1 SCSI Control 1 */ 
#define NCR810_OSCNTL2  0x02	/* SCNTL2 SCSI Control 2 */ 
#define NCR810_OSCNTL3  0x03	/* SCNTL3 SCSI Control 3 */ 
#define NCR810_OSCID    0x04	/* SCID   SCSI Chip ID   */ 
#define NCR810_OSXFER   0x05	/* SXFER  SCSI Transfer */ 
#define NCR810_OSDID    0x06	/* SDID   SCSI Destination ID */ 
#define NCR810_OGPREG   0x07	/* GPREG  General Purpose Bits */ 
#define NCR810_OSFBR    0x08	/* SFBR   SCSI First Byte Receiver */ 
#define NCR810_OSOCL    0x09	/* SOCL   SCSI Output Control Latch */ 
#define NCR810_OSSID    0x0A	/* SSID   SCSI Selector ID */ 
#define NCR810_OSBCL    0x0B	/* SBCL   SCSI Bus Control Lines */ 
#define NCR810_ODSTAT   0x0C	/* DSTAT  DMA Status */ 
#define NCR810_OSSTAT0  0x0D	/* SSTAT0 SCSI Status 0 */ 
#define NCR810_OSSTAT1  0x0E	/* SSTAT1 SCSI Status 1 */ 
#define NCR810_OSSTAT2  0x0F	/* SSTAT2 SCSI Status 2 */ 
#define NCR810_ODSA     0x10	/* DSA    Data Structure Address LW Aligned */
#define NCR810_OISTAT   0x14	/* ISTAT  Interrupt Status */ 
#define NCR810_OCTEST0  0x18	/* CTEST0 Chip Test 0 */ 
#define NCR810_OCTEST1  0x19	/* CTEST1 Chip Test 1 */ 
#define NCR810_OCTEST2  0x1A	/* CTEST2 Chip Test 2 */ 
#define NCR810_OCTEST3  0x1B	/* CTEST3 Chip Test 3 */ 
#define NCR810_OTEMP    0x1C	/* TEMP   Temporary Stack LW Aligned */
#define NCR810_ODFIFO   0x20	/* DFIFO  DMA FIFO */ 
#define NCR810_OCTEST4  0x21	/* CTEST4 Chip Test 4 */ 
#define NCR810_OCTEST5  0x22	/* CTEST5 Chip Test 5 */ 
#define NCR810_OCTEST6  0x23	/* CTEST6 Chip Test 6 */ 
#define NCR810_ODBC     0x24	/* DBC    DMA Byte Counter */ 
#define NCR810_ODCMD    0x27	/* DCMD   DMA Command */ 
#define NCR810_ODNAD    0x28	/* DNAD   DMA Next Adrs for Data LW Aligned */
#define NCR810_ODSP     0x2C	/* DSP    DMA SCRIPTS Pointer LW Aligned */
#define NCR810_ODSPS    0x30	/* DSPS   DMA SCRIPTS Pointer Save LW Aligned */ 
#define NCR810_OSCRTHA  0x34	/* SCRTHA General Purpose Scratch Pad A LW Aligned */
#define NCR810_ODMODE   0x38	/* DMODE  DMA Mode */ 
#define NCR810_ODIEN    0x39	/* DIEN   DMA Interrupt Enable */ 
#define NCR810_ODWT     0x3A	/* DWT    DMA Watchdog Timer */ 
#define NCR810_ODCNTL   0x3B	/* DCNTL  DMA Control */ 
#define NCR810_OADDER   0x3C	/* ADDER  Sum output of internal adrs LW Aligned */
#define NCR810_OSIEN0   0x40	/* SIEN0  SCSI Interrupt Enable 0 */ 
#define NCR810_OSIEN1   0x41	/* SIEN1  SCSI Interrupt Enable 1 */ 
#define NCR810_OSIST0   0x42	/* SIST0  SCSI Interrupt Status 0 */ 
#define NCR810_OSIST1   0x43	/* SIST1  SCSI Interrupt Status 1 */ 
#define NCR810_OSLPAR   0x44	/* SLPAR  SCSI longitudinal Parity */ 
    				/* 45 rsvd */
#define NCR810_OMACNTL  0x46	/* MACNTL Memory Access control */ 
#define NCR810_OGPCNTL  0x47	/* GPCNTL General Purpose Control */ 
#define NCR810_OSTIME0  0x48	/* STIME0 SCSI Timer 0 */ 
#define NCR810_OSTIME1  0x49	/* STIME1 SCSI Timer 1 */ 
#define NCR810_ORESPID  0x4A	/* RESPID Response ID */ 
    				/* 4B rsvd */
#define NCR810_OSTEST0  0x4C	/* STEST0 SCSI Test 0 */ 
#define NCR810_OSTEST1  0x4D	/* STEST1 SCSI Test 1 */ 
#define NCR810_OSTEST2  0x4E	/* STEST2 SCSI Test 2 */ 
#define NCR810_OSTEST3  0x4F	/* STEST3 SCSI Test 3 */ 
#define NCR810_OSIDL    0x50	/* SIDL   SCSI Input Data Latch */ 
    				/* 51-53 rsvd */
#define NCR810_OSODL    0x54	/* SODL   SCSI Output Data Latch */ 
    				/* 55-57 rsvd */
#define NCR810_OSBDL    0x58	/* SBDL   SCSI Bus Data Lines B */ 
    				/* 59-5B rsvd */
#define NCR810_OSCRTHB0 0x5C	/* SCRTHB General Purpose Scratch Pad B */ 
#define NCR810_OSCRTHB1 0x5D	/* SCRTHB General Purpose Scratch Pad B */ 
#define NCR810_OSCRTHB2 0x5E	/* SCRTHB General Purpose Scratch Pad B */ 
#define NCR810_OSCRTHB3 0x5F	/* SCRTHB General Purpose Scratch Pad B */ 



/* PCI Register access macros
*/
#define NCR810_SCNTL0  (NCR810_PCI_BASE+ NCR810_OSCNTL0)
#define NCR810_SCNTL1  (NCR810_PCI_BASE+ NCR810_OSCNTL1)
#define NCR810_SCNTL2  (NCR810_PCI_BASE+ NCR810_OSCNTL2)
#define NCR810_SCNTL3  (NCR810_PCI_BASE+ NCR810_OSCNTL3)
#define NCR810_SCID    (NCR810_PCI_BASE+ NCR810_OSCID)
#define NCR810_SXFER   (NCR810_PCI_BASE+ NCR810_OSXFER)
#define NCR810_SDID    (NCR810_PCI_BASE+ NCR810_OSDID)
#define NCR810_GPREG   (NCR810_PCI_BASE+ NCR810_OGPREG)
#define NCR810_SFBR    (NCR810_PCI_BASE+ NCR810_OSFBR)
#define NCR810_SOCL    (NCR810_PCI_BASE+ NCR810_OSOCL)
#define NCR810_SSID    (NCR810_PCI_BASE+ NCR810_OSSID)
#define NCR810_SBCL    (NCR810_PCI_BASE+ NCR810_OSBCL)
#define NCR810_DSTAT   (NCR810_PCI_BASE+ NCR810_ODSTAT)
#define NCR810_SSTAT0  (NCR810_PCI_BASE+ NCR810_OSSTAT0)
#define NCR810_SSTAT1  (NCR810_PCI_BASE+ NCR810_OSSTAT1)
#define NCR810_SSTAT2  (NCR810_PCI_BASE+ NCR810_OSSTAT2)
#define NCR810_DSA     (NCR810_PCI_BASE+ NCR810_ODSA)
#define NCR810_ISTAT   (NCR810_PCI_BASE+ NCR810_OISTAT)
#define NCR810_CTEST0  (NCR810_PCI_BASE+ NCR810_OCTEST0)
#define NCR810_CTEST1  (NCR810_PCI_BASE+ NCR810_OCTEST1)
#define NCR810_CTEST2  (NCR810_PCI_BASE+ NCR810_OCTEST2)
#define NCR810_CTEST3  (NCR810_PCI_BASE+ NCR810_OCTEST3)
#define NCR810_TEMP    (NCR810_PCI_BASE+ NCR810_OTEMP)
#define NCR810_DFIFO   (NCR810_PCI_BASE+ NCR810_ODFIFO)
#define NCR810_CTEST4  (NCR810_PCI_BASE+ NCR810_OCTEST4)
#define NCR810_CTEST5  (NCR810_PCI_BASE+ NCR810_OCTEST5)
#define NCR810_CTEST6  (NCR810_PCI_BASE+ NCR810_OCTEST6)
#define NCR810_DBC     (NCR810_PCI_BASE+ NCR810_ODBC)
#define NCR810_DCMD    (NCR810_PCI_BASE+ NCR810_ODCMD)
#define NCR810_DNAD    (NCR810_PCI_BASE+ NCR810_ODNAD)
#define NCR810_DSP     (NCR810_PCI_BASE+ NCR810_ODSP)
#define NCR810_DSPS    (NCR810_PCI_BASE+ NCR810_ODSPS)
#define NCR810_SCRTHA  (NCR810_PCI_BASE+ NCR810_OSCRTHA)
#define NCR810_DMODE   (NCR810_PCI_BASE+ NCR810_ODMODE)
#define NCR810_DIEN    (NCR810_PCI_BASE+ NCR810_ODIEN)
#define NCR810_DWT     (NCR810_PCI_BASE+ NCR810_ODWT)
#define NCR810_DCNTL   (NCR810_PCI_BASE+ NCR810_ODCNTL)
#define NCR810_ADDER   (NCR810_PCI_BASE+ NCR810_OADDER)
#define NCR810_SIEN0   (NCR810_PCI_BASE+ NCR810_OSIEN0)
#define NCR810_SIEN1   (NCR810_PCI_BASE+ NCR810_OSIEN1)
#define NCR810_SIST0   (NCR810_PCI_BASE+ NCR810_OSIST0)
#define NCR810_SIST1   (NCR810_PCI_BASE+ NCR810_OSIST1)
#define NCR810_SLPAR   (NCR810_PCI_BASE+ NCR810_OSLPAR)
#define NCR810_MACNTL  (NCR810_PCI_BASE+ NCR810_OMACNTL)
#define NCR810_GPCNTL  (NCR810_PCI_BASE+ NCR810_OGPCNTL)
#define NCR810_STIME0  (NCR810_PCI_BASE+ NCR810_OSTIME0)
#define NCR810_STIME1  (NCR810_PCI_BASE+ NCR810_OSTIME1)
#define NCR810_RESPID  (NCR810_PCI_BASE+ NCR810_ORESPID)
#define NCR810_STEST0  (NCR810_PCI_BASE+ NCR810_OSTEST0)
#define NCR810_STEST1  (NCR810_PCI_BASE+ NCR810_OSTEST1)
#define NCR810_STEST2  (NCR810_PCI_BASE+ NCR810_OSTEST2)
#define NCR810_STEST3  (NCR810_PCI_BASE+ NCR810_OSTEST3)
#define NCR810_SIDL    (NCR810_PCI_BASE+ NCR810_OSIDL)
#define NCR810_SODL    (NCR810_PCI_BASE+ NCR810_OSODL)
#define NCR810_SBDL    (NCR810_PCI_BASE+ NCR810_OSBDL)
#define NCR810_SCRTHB0 (NCR810_PCI_BASE+ NCR810_OSCRTHB0)
#define NCR810_SCRTHB1 (NCR810_PCI_BASE+ NCR810_OSCRTHB1)
#define NCR810_SCRTHB2 (NCR810_PCI_BASE+ NCR810_OSCRTHB2)
#define NCR810_SCRTHB3 (NCR810_PCI_BASE+ NCR810_OSCRTHB3)



/* NCR 53C810 CSR Write Mask definitions
/*    RW   bits have an associated 1
/*    RO   bits have an associated 0
/*    rsvd bits have an associated 0
*/
#define NCR810_WMSCNTL0 0xDB	/* SCNTL0 SCSI Control 0 */
#define NCR810_WMSCNTL1 0xE4	/* SCNTL1 SCSI Control 1 */ 
#define NCR810_WMSCNTL2 0x80	/* SCNTL2 SCSI Control 2 */ 
#define NCR810_WMSCNTL3 0x77	/* SCNTL3 SCSI Control 3 */ 
#define NCR810_WMSCID   0x67	/* SCID   SCSI Chip ID   */ 
#define NCR810_WMSXFER  0xEF	/* SXFER  SCSI Transfer */ 
#define NCR810_WMSDID   0x07	/* SDID   SCSI Destination ID */ 
#define NCR810_WMGPREG  0x00	/* GPREG  General Purpose Bits */ 
#define NCR810_WMSFBR   0x00	/* SFBR   SCSI First Byte Receiver */ 
#define NCR810_WMSOCL   0x00	/* SOCL   SCSI Output Control Latch */ 
#define NCR810_WMSSID   0x00	/* SSID   SCSI Selector ID */ 
#define NCR810_WMSBCL   0x00	/* SBCL   SCSI Bus Control Lines */ 
#define NCR810_WMDSTAT  0x00	/* DSTAT  DMA Status */ 
#define NCR810_WMSSTAT0 0x00	/* SSTAT0 SCSI Status 0 */ 
#define NCR810_WMSSTAT1 0x00	/* SSTAT1 SCSI Status 1 */ 
#define NCR810_WMSSTAT2 0x00	/* SSTAT2 SCSI Status 2 */ 
#define NCR810_WMDSA    0xFF	/* DSA    Data Structure Address LW Aligned */
#define NCR810_WMISTAT  0x20	/* ISTAT  Interrupt Status */ 
#define NCR810_WMCTEST0 0xFF	/* CTEST0 Chip Test 0 */ 
#define NCR810_WMCTEST1 0x00	/* CTEST1 Chip Test 1 */ 
#define NCR810_WMCTEST2 0x00	/* CTEST2 Chip Test 2 */ 
#define NCR810_WMCTEST3 0x00	/* CTEST3 Chip Test 3 */ 
#define NCR810_WMTEMP   0xFF	/* TEMP   Temporary Stack LW Aligned */
#define NCR810_WMDFIFO  0x00	/* DFIFO  DMA FIFO */ 
#define NCR810_WMCTEST4 0x00	/* CTEST4 Chip Test 4 */ 
#define NCR810_WMCTEST5 0x10	/* CTEST5 Chip Test 5 */ 
#define NCR810_WMCTEST6 0x00	/* CTEST6 Chip Test 6 */ 
#define NCR810_WMDBC    0xFF	/* DBC    DMA Byte Counter TRI Byte aligned */ 
#define NCR810_WMDCMD   0xFF	/* DCMD   DMA Command */ 
#define NCR810_WMDNAD   0xFF	/* DNAD   DMA Next Adrs for Data LW Aligned */
#define NCR810_WMDSP    0x00	/* DSP    DMA SCRIPTS Pointer LW Aligned */
#define NCR810_WMDSPS   0xFF	/* DSPS   DMA SCRIPTS Pointer Save LW Aligned */ 
#define NCR810_WMSCRTHA 0xFF	/* SCRTHA General Purpose Scratch Pad A LW Aligned */
#define NCR810_WMDMODE  0xF9	/* DMODE  DMA Mode */ 
#define NCR810_WMDIEN   0x7D	/* DIEN   DMA Interrupt Enable */ 
#define NCR810_WMDWT    0xFF	/* DWT    DMA Watchdog Timer */ 
#define NCR810_WMDCNTL  0x1A	/* DCNTL  DMA Control */ 
#define NCR810_WMADDER  0x00	/* ADDER  Sum output of internal adrs LW Aligned */
#define NCR810_WMSIEN0  0xFF	/* SIEN0  SCSI Interrupt Enable 0 */ 
#define NCR810_WMSIEN1  0x07	/* SIEN1  SCSI Interrupt Enable 1 */ 
#define NCR810_WMSIST0  0x00	/* SIST0  SCSI Interrupt Status 0 */ 
#define NCR810_WMSIST1  0x00	/* SIST1  SCSI Interrupt Status 1 */ 
#define NCR810_WMSLPAR  0xFF	/* SLPAR  SCSI longitudinal Parity */ 
#define NCR810_WMMACNTL 0x0F	/* MACNTL Memory Access control */ 
#define NCR810_WMGPCNTL 0xC3	/* GPCNTL General Purpose Control */ 
#define NCR810_WMSTIME0 0xFF	/* STIME0 SCSI Timer 0 */ 
#define NCR810_WMSTIME1 0x0F	/* STIME1 SCSI Timer 1 */ 
#define NCR810_WMRESPID 0xFF	/* RESPID Response ID */ 
#define NCR810_WMSTEST0 0x00	/* STEST0 SCSI Test 0 */ 
#define NCR810_WMSTEST1 0x00	/* STEST1 SCSI Test 1 */ 
#define NCR810_WMSTEST2 0xDB	/* STEST2 SCSI Test 2 */ 
#define NCR810_WMSTEST3 0xF7	/* STEST3 SCSI Test 3 */ 
#define NCR810_WMSIDL   0x00	/* SIDL   SCSI Input Data Latch */ 
#define NCR810_WMSODL   0xFF	/* SODL   SCSI Output Data Latch */ 
#define NCR810_WMSBDL   0x00	/* SBDL   SCSI Bus Data Lines B */ 
#define NCR810_WMSCRTHB 0xFF	/* SCRTHB General Purpose Scratch Pad B */ 


/* NCR 53C810 CSR "X/rsvd" Mask definitions
/*    bits having known state      1 
/*    x    bits have an associated 0
/*    rsvd bits have an associated 0
*/
#define NCR810_XSCNTL0 0xFB	/* SCNTL0 SCSI Control 0 */
#define NCR810_XSCNTL1 0xFF	/* SCNTL1 SCSI Control 1 */ 
#define NCR810_XSCNTL2 0x80	/* SCNTL2 SCSI Control 2 */ 
#define NCR810_XSCNTL3 0x77	/* SCNTL3 SCSI Control 3 */ 
#define NCR810_XSCID   0x67	/* SCID   SCSI Chip ID   */ 
#define NCR810_XSXFER  0xEF	/* SXFER  SCSI Transfer */ 
#define NCR810_XSDID   0x07	/* SDID   SCSI Destination ID */ 
#define NCR810_XGPREG  0x00	/* GPREG  General Purpose Bits */ 
#define NCR810_XSFBR   0x00	/* SFBR   SCSI First Byte Receiver */ 
#define NCR810_XSOCL   0xFF	/* SOCL   SCSI Output Control Latch */ 
#define NCR810_XSSID   0x87	/* SSID   SCSI Selector ID */ 
#define NCR810_XSBCL   0x00	/* SBCL   SCSI Bus Control Lines */ 
#define NCR810_XDSTAT  0x00	/* DSTAT  DMA Status */ 
#define NCR810_XSSTAT0 0xFE	/* SSTAT0 SCSI Status 0 */ 
#define NCR810_XSSTAT1 0xF0	/* SSTAT1 SCSI Status 1 */ 
#define NCR810_XSSTAT2 0x02	/* SSTAT2 SCSI Status 2 */ 
#define NCR810_XDSA    0xFF	/* DSA    Data Structure Address LW Aligned */
#define NCR810_XISTAT  0xFF	/* ISTAT  Interrupt Status */ 
#define NCR810_XCTEST0 0xFF	/* CTEST0 Chip Test 0 */ 
#define NCR810_XCTEST1 0x00	/* CTEST1 Chip Test 1 */ 
#define NCR810_XCTEST2 0x00	/* CTEST2 Chip Test 2 */ 
#define NCR810_XCTEST3 0x0E	/* CTEST3 Chip Test 3 */ 
#define NCR810_XTEMP   0xFF	/* TEMP   Temporary Stack LW Aligned */
#define NCR810_XDFIFO  0x00	/* DFIFO  DMA FIFO */ 
#define NCR810_XCTEST4 0xFF	/* CTEST4 Chip Test 4 */ 
#define NCR810_XCTEST5 0xD8	/* CTEST5 Chip Test 5 */ 
#define NCR810_XCTEST6 0x00	/* CTEST6 Chip Test 6 */ 
#define NCR810_XDBC    0xFF	/* DBC    DMA Byte Counter TRI Byte aligned */ 
#define NCR810_XDCMD   0xFF	/* DCMD   DMA Command */ 
#define NCR810_XDNAD   0xFF	/* DNAD   DMA Next Adrs for Data LW Aligned */
#define NCR810_XDSP    0x00	/* DSP    DMA SCRIPTS Pointer LW Aligned */
#define NCR810_XDSPS   0xFF	/* DSPS   DMA SCRIPTS Pointer Save LW Aligned */ 
#define NCR810_XSCRTHA 0xFF	/* SCRTHA General Purpose Scratch Pad A LW Aligned */
#define NCR810_XDMODE  0xF9	/* DMODE  DMA Mode */ 
#define NCR810_XDIEN   0x7D	/* DIEN   DMA Interrupt Enable */ 
#define NCR810_XDWT    0xFF	/* DWT    DMA Watchdog Timer */ 
#define NCR810_XDCNTL  0x1F	/* DCNTL  DMA Control */ 
#define NCR810_XADDER  0x00	/* ADDER  Sum output of internal adrs LW Aligned */
#define NCR810_XSIEN0  0xFF	/* SIEN0  SCSI Interrupt Enable 0 */ 
#define NCR810_XSIEN1  0x07	/* SIEN1  SCSI Interrupt Enable 1 */ 
#define NCR810_XSIST0  0xFF	/* SIST0  SCSI Interrupt Status 0 */ 
#define NCR810_XSIST1  0x07	/* SIST1  SCSI Interrupt Status 1 */ 
#define NCR810_XSLPAR  0xFF	/* SLPAR  SCSI longitudinal Parity */ 
#define NCR810_XMACNTL 0x0F	/* MACNTL Memory Access control */ 
#define NCR810_XGPCNTL 0xC3	/* GPCNTL General Purpose Control */ 
#define NCR810_XSTIME0 0xFF	/* STIME0 SCSI Timer 0 */ 
#define NCR810_XSTIME1 0x0F	/* STIME1 SCSI Timer 1 */ 
#define NCR810_XRESPID 0xFF	/* RESPID Response ID */ 
#define NCR810_XSTEST0 0x0B	/* STEST0 SCSI Test 0 */ 
#define NCR810_XSTEST1 0x80	/* STEST1 SCSI Test 1 */ 
#define NCR810_XSTEST2 0xDF	/* STEST2 SCSI Test 2 */ 
#define NCR810_XSTEST3 0xF7	/* STEST3 SCSI Test 3 */ 
#define NCR810_XSIDL   0x00	/* SIDL   SCSI Input Data Latch */ 
#define NCR810_XSODL   0xFF	/* SODL   SCSI Output Data Latch */ 
#define NCR810_XSBDL   0x00	/* SBDL   SCSI Bus Data Lines B */ 
#define NCR810_XSCRTHB 0xFF	/* SCRTHB General Purpose Scratch Pad B */ 

#define NCR810_RSVD    0x00	/* reserved byte */


/* NCR 53C810 CSR reset values 
*/
#define NCR810_ISCNTL0 0xC0	/* SCNTL0 SCSI Control 0 */
#define NCR810_ISCNTL1 0x00	/* SCNTL1 SCSI Control 1 */ 
#define NCR810_ISCNTL2 0x40	/* SCNTL2 SCSI Control 2 */ 
#define NCR810_ISCNTL3 0x00	/* SCNTL3 SCSI Control 3 */ 
#define NCR810_ISCID   0x00	/* SCID   SCSI Chip ID   */ 
#define NCR810_ISXFER  0x00	/* SXFER  SCSI Transfer */ 
#define NCR810_ISDID   0x00	/* SDID   SCSI Destination ID */ 
#define NCR810_IGPREG  0x00	/* GPREG  General Purpose Bits */ 
#define NCR810_ISFBR   0x00	/* SFBR   SCSI First Byte Receiver */ 
#define NCR810_ISOCL   0x00	/* SOCL   SCSI Output Control Latch */ 
#define NCR810_ISSID   0x00	/* SSID   SCSI Selector ID */ 
#define NCR810_ISBCL   0x00	/* SBCL   SCSI Bus Control Lines */ 
#define NCR810_IDSTAT  0x00	/* DSTAT  DMA Status */ 
#define NCR810_ISSTAT0 0x00	/* SSTAT0 SCSI Status 0 */ 
#define NCR810_ISSTAT1 0x00	/* SSTAT1 SCSI Status 1 */ 
#define NCR810_ISSTAT2 0x02	/* SSTAT2 SCSI Status 2 */ 
#define NCR810_IDSA    0x00	/* DSA    Data Structure Address LW Aligned */
#define NCR810_IISTAT  0x00	/* ISTAT  Interrupt Status */ 
#define NCR810_ICTEST0 0x00	/* CTEST0 Chip Test 0 */ 
#define NCR810_ICTEST1 0xF0	/* CTEST1 Chip Test 1 */ 
#define NCR810_ICTEST2 0x01	/* CTEST2 Chip Test 2 */ 
#define NCR810_ICTEST3 0x00	/* CTEST3 Chip Test 3 */ 
#define NCR810_ITEMP   0x00	/* TEMP   Temporary Stack LW Aligned */
#define NCR810_IDFIFO  0x00	/* DFIFO  DMA FIFO */ 
#define NCR810_ICTEST4 0x00	/* CTEST4 Chip Test 4 */ 
#define NCR810_ICTEST5 0x00	/* CTEST5 Chip Test 5 */ 
#define NCR810_ICTEST6 0x00	/* CTEST6 Chip Test 6 */ 
#define NCR810_IDBC    0x00	/* DBC    DMA Byte Counter TRI Byte aligned */ 
#define NCR810_IDCMD   0x00	/* DCMD   DMA Command */ 
#define NCR810_IDNAD   0x00	/* DNAD   DMA Next Adrs for Data LW Aligned */
#define NCR810_IDSP    0x00	/* DSP    DMA SCRIPTS Pointer LW Aligned */
#define NCR810_IDSPS   0x00	/* DSPS   DMA SCRIPTS Pointer Save LW Aligned */ 
#define NCR810_ISCRTHA 0x00	/* SCRTHA General Purpose Scratch Pad A LW Aligned */
#define NCR810_IDMODE  0x00	/* DMODE  DMA Mode */ 
#define NCR810_IDIEN   0x00	/* DIEN   DMA Interrupt Enable */ 
#define NCR810_IDWT    0x00	/* DWT    DMA Watchdog Timer */ 
#define NCR810_IDCNTL  0x00	/* DCNTL  DMA Control */ 
#define NCR810_IADDER  0x00	/* ADDER  Sum output of internal adrs LW Aligned */
#define NCR810_ISIEN0  0x00	/* SIEN0  SCSI Interrupt Enable 0 */ 
#define NCR810_ISIEN1  0x00	/* SIEN1  SCSI Interrupt Enable 1 */ 
#define NCR810_ISIST0  0x00	/* SIST0  SCSI Interrupt Status 0 */ 
#define NCR810_ISIST1  0x00	/* SIST1  SCSI Interrupt Status 1 */ 
#define NCR810_ISLPAR  0x00	/* SLPAR  SCSI longitudinal Parity */ 
#define NCR810_IMACNTL 0x00	/* MACNTL Memory Access control */ 
#define NCR810_IGPCNTL 0x03	/* GPCNTL General Purpose Control */ 
#define NCR810_ISTIME0 0x00	/* STIME0 SCSI Timer 0 */ 
#define NCR810_ISTIME1 0x00	/* STIME1 SCSI Timer 1 */ 
#define NCR810_IRESPID 0x00	/* RESPID Response ID */ 
#define NCR810_ISTEST0 0x03	/* STEST0 SCSI Test 0 */ 
#define NCR810_ISTEST1 0x00	/* STEST1 SCSI Test 1 */ 
#define NCR810_ISTEST2 0x00	/* STEST2 SCSI Test 2 */ 
#define NCR810_ISTEST3 0x00	/* STEST3 SCSI Test 3 */ 
#define NCR810_ISIDL   0x00	/* SIDL   SCSI Input Data Latch */ 
#define NCR810_ISODL   0x00	/* SODL   SCSI Output Data Latch */ 
#define NCR810_ISBDL   0x00	/* SBDL   SCSI Bus Data Lines B */ 
#define NCR810_ISCRTHB 0x00	/* SCRTHB General Purpose Scratch Pad B */ 

#define NCR810_RSVD    0x00	/* reserved byte */

/* NCR 53C810 Default Program values 
*/
#define NCR810_DSCNTL0 0xDF	/* SCNTL0 SCSI Control 0 */
#define NCR810_DSCNTL1 0xE4	/* SCNTL1 SCSI Control 1 */ 
#define NCR810_DSCNTL2 0xFF	/* SCNTL2 SCSI Control 2 */ 
#define NCR810_DSCNTL3 0xFF	/* SCNTL3 SCSI Control 3 */ 
#define NCR810_DSCID   0xFF	/* SCID   SCSI Chip ID   */ 
#define NCR810_DSXFER  0xFF	/* SXFER  SCSI Transfer */ 
#define NCR810_DSDID   0xFF	/* SDID   SCSI Destination ID */ 
#define NCR810_DGPREG  0xFF	/* GPREG  General Purpose Bits */ 
#define NCR810_DSFBR   0x00	/* SFBR   SCSI First Byte Receiver */ 
#define NCR810_DSOCL   0x00	/* SOCL   SCSI Output Control Latch */ 
#define NCR810_DSSID   0x00	/* SSID   SCSI Selector ID */ 
#define NCR810_DSBCL   0x00	/* SBCL   SCSI Bus Control Lines */ 
#define NCR810_DDSTAT  0x00	/* DSTAT  DMA Status */ 
#define NCR810_DSSTAT0 0x00	/* SSTAT0 SCSI Status 0 */ 
#define NCR810_DSSTAT1 0x00	/* SSTAT1 SCSI Status 1 */ 
#define NCR810_DSSTAT2 0x00	/* SSTAT2 SCSI Status 2 */ 
#define NCR810_DDSA    0xFF	/* DSA    Data Structure Address LW Aligned */
#define NCR810_DISTAT  0x20	/* ISTAT  Interrupt Status */ 
#define NCR810_DCTEST0 0xFF	/* CTEST0 Chip Test 0 */ 
#define NCR810_DCTEST1 0x00	/* CTEST1 Chip Test 1 */ 
#define NCR810_DCTEST2 0x00	/* CTEST2 Chip Test 2 */ 
#define NCR810_DCTEST3 0x00	/* CTEST3 Chip Test 3 */ 
#define NCR810_DTEMP   0xFF	/* TEMP   Temporary Stack LW Aligned */
#define NCR810_DDFIFO  0x00	/* DFIFO  DMA FIFO */ 
#define NCR810_DCTEST4 0x00	/* CTEST4 Chip Test 4 */ 
#define NCR810_DCTEST5 0x10	/* CTEST5 Chip Test 5 */ 
#define NCR810_DCTEST6 0x00	/* CTEST6 Chip Test 6 */ 
#define NCR810_DDBC    0xFF	/* DBC    DMA Byte Counter TRI Byte aligned */ 
#define NCR810_DDCMD   0xFF	/* DCMD   DMA Command */ 
#define NCR810_DDNAD   0xFF	/* DNAD   DMA Next Adrs for Data LW Aligned */
#define NCR810_DDSP    0x00	/* DSP    DMA SCRIPTS Pointer LW Aligned */
#define NCR810_DDSPS   0xFF	/* DSPS   DMA SCRIPTS Pointer Save LW Aligned */ 
#define NCR810_DSCRTHA 0xFF	/* SCRTHA General Purpose Scratch Pad A LW Aligned */
#define NCR810_DDMODE  0x2F	/* DMODE  DMA Mode */ 
#define NCR810_DDIEN   0x3F	/* DIEN   DMA Interrupt Enable */ 
#define NCR810_DDWT    0xFF	/* DWT    DMA Watchdog Timer */ 
#define NCR810_DDCNTL  0xDA	/* DCNTL  DMA Control */ 
#define NCR810_DADDER  0x00	/* ADDER  Sum output of internal adrs LW Aligned */
#define NCR810_DSIEN0  0xFF	/* SIEN0  SCSI Interrupt Enable 0 */ 
#define NCR810_DSIEN1  0xFF	/* SIEN1  SCSI Interrupt Enable 1 */ 
#define NCR810_DSIST0  0x00	/* SIST0  SCSI Interrupt Status 0 */ 
#define NCR810_DSIST1  0x00	/* SIST1  SCSI Interrupt Status 1 */ 
#define NCR810_DSLPAR  0xFF	/* SLPAR  SCSI longitudinal Parity */ 
#define NCR810_DMACNTL 0x0F	/* MACNTL Memory Access control */ 
#define NCR810_DGPCNTL 0xC3	/* GPCNTL General Purpose Control */ 
#define NCR810_DSTIME0 0xFF	/* STIME0 SCSI Timer 0 */ 
#define NCR810_DSTIME1 0xFF	/* STIME1 SCSI Timer 1 */ 
#define NCR810_DRESPID 0xFF	/* RESPID Response ID */ 
#define NCR810_DSTEST0 0x00	/* STEST0 SCSI Test 0 */ 
#define NCR810_DSTEST1 0x00	/* STEST1 SCSI Test 1 */ 
#define NCR810_DSTEST2 0xDB	/* STEST2 SCSI Test 2 */ 
#define NCR810_DSTEST3 0xF7	/* STEST3 SCSI Test 3 */ 
#define NCR810_DSIDL   0x00	/* SIDL   SCSI Input Data Latch */ 
#define NCR810_DSODL   0x00	/* SODL   SCSI Output Data Latch */ 
#define NCR810_DSBDL   0x00	/* SBDL   SCSI Bus Data Lines B */ 
#define NCR810_DSCRTHB 0xFF	/* SCRTHB General Purpose Scratch Pad B */ 

#define NCR810_RSVD    0x00	/* reserved byte */



/* NCR810 PCI CONFIGURATION REGISTER offset definitions
*/
#define NCR810_CONFIG_INC    0x4 	/* NCR810  config register incremenet */
#define NCR810_DCBIO_IO      0x00000001	
#define NCR810_DCBIO_MEM     0x00000000

/* PCI Register offsets
*/
#define NCR810_CFID      0x00    /* Configuration ID */
#define NCR810_CFCS      0x04    /* Configuration Command Status */
#define NCR810_CFRV      0x08    /* Configuration Revision */
#define NCR810_CFLT      0x0C    /* Configuration Latency Timer */
#define NCR810_CBIO      0x10    /* Configuration Base IO  Address */
#define NCR810_CBMEM     0x14    /* Configuration Base MEM Address */
/* rsvd 14h-3Ch */

/* NCR810 PCI CONFIGURATION REGISTER mask definitions
*/
#define NCR810_MCFID      0x00000000 	/* Configuration DID VID */
#define NCR810_MCFCS      0x00000147	/* Configuration Command Status */
#define NCR810_MCFRV      0x00000000	/* Configuration Revision */
#define NCR810_MCFLT      0x0000FF00	/* Configuration Latency Timer */
#define NCR810_MCBIO      0xFFFFFC00	/* Configuration Base IO  Address */
#define NCR810_MCBMEM     0xFFFFFC00	/* Configuration Base MEM Address */

/* NCR810 PCI CONFIGURATION REGISTER init values
*/
#define NCR810_ICFID      0x00000000	/* Configuration DID VID*/
#define NCR810_ICFCS      0x00000000	/* Configuration CommandStatus */
#define NCR810_ICFRV      0x00000001	/* Configuration ClassCode Revision */
#define NCR810_ICFLT      0x00000000	/* Configuration HeaderType/LatencyTimer */
#define NCR810_ICBIO      0x00021000	/* Configuration Base IO  Address */
#define NCR810_ICBMEM     0x00000000	/* Configuration Base MEM Address */

/* NCR810 PCI CONFIGURATION REGISTER default program values 
*/                       
#define NCR810_DCFID      0x00011000	/* Configuration ID */
#if MEDULLA  /* Use memory access for now to avoid problems with Tulip. */
#define NCR810_DCFCS      0x00000146	/* Configuration Command Status */
#else
#define NCR810_DCFCS      0x02000145	/* Configuration Command Status */
#endif
#define NCR810_DCFRV      0x00000001	/* Configuration RevisionID*/
#define NCR810_DCFLT      0x00000800	/* Configuration HeaderType/LatencyTimer 8 Ticks */
#define NCR810_DCBIO     (NCR810_PCI_BASE + 1)  /* Map to  PCI IO  Adrs space response */
#if MEDULLA  /* Use memory access for now to avoid problems with Tulip. */
#define NCR810_DCBMEM	  NCR810_PCI_BASE /* Enable PCI Mem Adrs space response */
#else
#define NCR810_DCBMEM	  0x00000000	  /* Disable PCI Mem Adrs space response */
#endif

/* Defines for SCNTL1 
*/
#define SCNTL1_EXC        0x80  /* Extra CLK of Data Setup  */
#define SCNTL1_ADB        0x40  /* Assert SCSI Data Bus     */
#define SCNTL1_ESR        0x20  /* Enable Selection&ReSelct */
#define SCNTL1_FCON       0x10  /* Connected                */
#define SCNTL1_SRST       0x08  /* Assert SCSI RST/ Signal  */
#define SCNTL1_AESP       0x04  /* Assert Even SCSI Parity  */
#define SCNTL1_SND        0x02  /* Start SCSI Send Operation*/
#define SCNTL1_RCV        0x01  /* Start SCSI Rcv Operation */

/* Defines for CTEST4 
/*
*/
#define CTEST4_MUX       0x80  /* Host Bus Multiplex Mode  */
#define CTEST4_ZMOD      0x40  /* High Impedance Mode      */
#define CTEST4_SZM       0x20  /* SCSI High Impedance Mode */
#define CTEST4_SLBE      0x10  /* SCSI Loopback Enable     */
#define CTEST4_SFWR      0x08  /* SCSI FIFO Write Enable   */
#define CTEST4_FFBL_DSBL 0x00  /* FIFO Byte Lane Disable   */
#define CTEST4_FFBL_0    0x04  /* FIFO Byte Lane 0         */
#define CTEST4_FFBL_1    0x05  /* FIFO Byte Lane 1         */
#define CTEST4_FFBL_2    0x06  /* FIFO Byte Lane 2         */
#define CTEST4_FFBL_3    0x07  /* FIFO Byte Lane 3         */
   
