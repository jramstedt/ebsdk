/* File:	8242_def.h
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
 * Abstract:	Intel 8242 "Pre-Programed 8242 Keyboard Control" 
 *		  - Diagnostic Header Module.  
 *		  - MultiKey/42 command definitions
 *
 * Author:	D.W. Neale
 *
 * Modifications:
 *
 *	dwn	28-June-1993	Initial Entry
 */
 
/* LOCAL SYMBOLS and Macro definitions
*/
#define i8242_AuxOBF	1	/* i8242 Status Register bit 5, Aux Dev Output Buffer Full */
#define i8242_KbOBF     0	/* i8242 Status Register bit 0, Kb      Output Buffer Full */


/*+
 * ============================================================================
 * = Phoenix MultiKey/42 Version 1.10 Reference Manual			      =
 * ============================================================================
 *
 *    The following is my interpretation of the Phoenix MultiKey/43 Technical
 *    Manual where references can be made.
 *
 *    1.0 Sending MultiKey/42 Commands
 *
 *    	Mk42_Cmdxx		=> Port64
 *    	Cmd  Data		=> Port60
 *
 *        1.1 MultiKey/42 Standard Commands
 *
 *    	Mk42_Cmd00	0x00	Read RAM 20-3Fh and send to system 
 *    	Mk42_Cmd40	0x40	Get byte from system and write RAM Location 20-3Fh 
 *    	Mk42_Cmd20	0x20	Read from RAM 20-3Fh 
 *    	Mk42_Cmd60	0x60	Write to  RAM 20-3Fh 
 *
 *    	1.1.1 MultiKey/42 RAM Map
 *
 *    	    Mk42_Temp0		0x00	Temp Register 
 *    	    Mk42_Temp1		0x01	Temp register 
 *    	    Mk42_KCMISC		0x02	KeyBrd Cntlr Misc Flags 
 *    	    Mk42_KCSTATE	0x03	keyBrk Cntlr State Flags 
 *    	    Mk42_PENDING	0x04	Storage for the OBF Pending Data
 *    	    Mk42_TEMP2		0x05	Temp Register 
 *    	    Mk42_TIMEOUT	0x06	KeyBrd Cntlr Timeout Flags 
 *    	    Mk42_TEMP3		0x07	Temp Register
 *    	    Mk42_STACK		0x08	Program Stack (16 bytes) 8-17h 
 *
 *    	    Mk42_INDEX		0x18	Pheonix Extended Mem Index
 *    	    Mk42_KSTATE1	0x19	Keyboard ScanCode Set, LED State
 *    	    Mk42_KSTATE2	0x1A	Keyboard Typematic Delay and Rate 
 *    	    Mk42_LASTKBD	0x1B	Storage for the Last ScanCode
 *    	    Mk42_TMRATES	0x1C	60us Min Interface inactive time
 *
 *    	    Mk42_KCCB		0x20	KeyBrd Cntlr Command Byte 
 *    	    Mk42_RETRY		0x21	Num times to resend a xmissions
 *    	    Mk42_KBDRSP		0x22	
 *    	    Mk42_KSRSND		0x23	
 *
 *    	    Mk42_TMRATE1	0x27	380us  Timer Value 
 *    	    Mk42_TMRATE2	0x28	2.4ms  Timer Value 
 *    	    Mk42_TMRATE3	0x29	11.7ms Timer Value
 *
 *    	    Mk42_BREAK		0x2D	Break-Code (00h or 80h)
 *
 *    	    Mk42_AUXRSP		0x30	
 *    	    Mk42_ARESND		0x31	
 *
 *    	    Mk42_PWNULL1	0x33	Sent with Password enable
 *    	    Mk42_PWNULL2	0x34	Sent with Password disable
 *
 *    	    Mk42_PWSCAN1	0x36	
 *    	    Mk42_PWSCAN2	0x37	
 *
 *    	    Mk42_PWINDEX 	0x40	Password Index
 *    	    Mk42_PWAREA		0x41	Password Storage (9 bytes)
 *
 *        1.1 (Continued)
 * 
 *    	Mk42_CmdTstPas	0xA4	Test Password 
 *    	Mk42_CmdLdPas	0xA5	Load Password 
 *
 *    	Mk42_CmdEnPas	0xA6	Enable Password 
 *    	Mk42_CmdDisAux	0xA7	Disable Aux Device 
 *    	Mk42_CmdEnAux	0xA8	Enable  Aux Device 
 *    	Mk42_CmdTstAux	0xA9	Test    Aux Device clock and Data 
 *    	Mk42_CmdBist	0xAA	8742 Self Test 
 *    	Mk42_CmdTstKb	0xAB	Test Keyboard Clock and Data lines 
 *    	Mk42_CmdNa	0xAC	The Diag Dump not implemented 
 *    	Mk42_CmdDisKb	0xAD	Disable Keyboard 
 *    	Mk42_CmdEnKb	0xAE	Enable  Keyboard 
 *    	Mk42_CmdRdP1	0xC0	Emulate reading P1 and send data to system 
 *    	Mk42_CmdRdP30	0xC1	Puts P1[3:0] into status Register 
 *    	Mk42_CmdRdP74	0xC2	Puts P1[7:4] into status register 
 *    	Mk42_CmdRdP2	0xD0	Reads P2<7:0>
 *    	Mk42_CmdGateA20	0xD1	Only set/clr GateA20 line based on sys data bit 1 
 *    	Mk42_CmdRdKb	0xD2	Read  Keyboard 
 *    	Mk42_CmdRdAux	0xD3	Read  Aux Device 
 *    	Mk42_CmdWrAux	0xD4	Write Aux Device 
 *    	Mk42_CmdRdTst	0xE0	Read state of test inputs 
 *
 *        1.2 MultiKey/42 Extended i8242 Commands
 *
 *    	Mk42_ExtCmdLdIndex	0xB8	Setup Pheonix ext Mem       access INDEX 
 *    	Mk42_ExtCmdGtIndex	0xB9	Get current Pheonix ext mem access INDEX 
 *    	Mk42_ExtCmdRdIndex	0xBA	Get current Pheonix ext mem ref by INDEX 
 *    	Mk42_ExtCmdWtIndex	0xBB	Write Pheonix ext mem       ref by INDEX 
 *
 *    	Mk42_ExtCmdRdRam	0xBC	READ  RAM @VPointer 
 *    	Mk42_ExtCmdWtRam	0xBD	WRITE RAM @VPointer 
 *
 *    	Mk42_ExtCmdSetP1	0xC7	Sets  P1 bits 
 *    	Mk42_ExtCmdClrP1	0xC8	Clear P1 Bits 
 *    	Mk42_ExtCmdSetP2	0xC9	Sets  p2 Bits 
 *    	Mk42_ExtCmdClrP2	0xCA	Clear P2 bits 
 *
 *    	Mk42_ExtCmdSetFst	0xD3	Set   Fast GateA20 
 *    	Mk42_ExtCmdClrFst	0xD4	Clear Fast GateA20 
 *
 *    	Mk42_ExtCmdRdVer	0xD5	Read Phoenix Version Number 
 *    	Mk42_ExtCmdRdInfo	0xD6	Read Version Info 
 *    	Mk42_ExtCmdRdModel	0xD7	Read Model Number 
 *
 *    	Mk42_ExtCmdSetA20	0xD8	Set   Fast GateA20 
 *    	Mk42_ExtCmdClrA20	0xD9	Clear Fast GateA20 
 *
 *    	Mk42_ExtCmdSetP2x	0xE0	Sets P2.x bits 
 *
 *
 *    2.0 Reading MultiKey/42 Status
 *
 *    	MultiKey/42 Status	<= Port64
 *
 *    	2.1 Port64 Status Bit Definitions
 *
 *    	    KbStat$V_OBF	0x01	8242/Keyboard Output Buffer Full 
 *    	    KbStat$V_IBF	0x02	Input Buffer Full 
 *    	    KbStat$V_F0		0x40	Flag 0, System Flag 
 *    	    KbStat$V_F1		0x80	Flag 1, Command/Data 
 *    	    KbStat$V_KBEN	0x10	Inhibited Switch 
 *    	    KbStat$V_AuxOBF	0x20	Auxiliary Device Output Buffer Full 
 *    	    KbStat$V_GTO	0x40	General Timeout 
 *    	    KbStat$V_PERR	0x80	Parity Error 
 *
 *
 *    3.0 Sending KeyBoard Cmds, CmdData, Returning Data
 *
 *    	KbCmd_xx  		=> Port60
 *    	KeyBrd Cmd    Data	=> Port60
 *    	KeyBrd Return Data	<= Port60
 *
 *    	3.1 KeyBoard Commands
 *
 *    	KbCmd_SetLeds	  0xED	Set LEDs 
 *    	KbCmd_Echo	  0xEE	Echo 
 *    	KbCmd_InvalCmd	  0xEF	Invalid Command 
 *    	KbCmd_SelAltScn   0xF0	Select alternate scan code set 
 *    	KbCmd_RdID	  0xF2	Read ID bytes 
 *    	KbCmd_SetTypDly	  0xF3	Set Typematic delay and rate 
 *    	KbCmd_EnKb	  0xF4	Enable keyboard 
 *    	KbCmd_DisKb	  0xF5	Disable Keyboard and set defaults 
 *    	KbCmd_SetDefs	  0xF6	Set defaults 
 *    	KbCmd_SetTyp	  0xF7	Set all keys Typematic 
 *    	KbCmd_SetMkBrk	  0xF8	Set all keys mark/break 
 *    	KbCmd_SetMk	  0xF9	Set all keys mark only 
 *    	KbCmd_Resend	  0xFE	Re-send the last transmission 
 *    	KbCmd_Reset	  0xFF	BAT, Reset the defaults and buffers 
 *
 *    	
 *    4.0 Sending Mouse Commands
 *
 *    	Mk42_CmdWrAux		=> Port64
 *    	AuxCmd_xx		=> Port60
 *
 *        4.1 Mouse Commands
 *
 *        AuxCmd_RstScal	  0xE6	Reset Scaling 
 *        AuxCmd_SetScal	  0xE7	Set Scaling 
 *        AuxCmd_SetRes	  0xE8	Set Resolution 
 *        AuxCmd_GetStat    0xE9	Status Request 
 *        AuxCmd_SetStmMode 0xEA	Set Stream Mode 
 *    	AuxCmd_RdData	  0xEB	Read Data 
 *    	AuxCmd_RstWrpMode 0xEC	Reset Wrap Mode 
 *    	AuxCmd_InvalCmd	  0xED	Invalid command 
 *    	AuxCmd_SetWrpMode 0xEE	Set Wrap Mode 
 *    	AuxCmd_SetRmtMode 0xF0	Set Remote Mode 
 *    	AuxCmd_RdDevType  0xF2	Read Device Type 
 *    	AuxCmd_SetSmpRate 0xF3	Set Sampling Rate 
 *    	AuxCmd_EnAuxDev	  0xF4	Enable  Auxiliary Device 
 *        AuxCmd_DisAuxDev  0xF5	Disable Auxiliary Device 
 *    	AuxCmd_SetDefVals 0xF6	Set Default Values 
 *    	AuxCmd_Resend	  0xFE	Re-send 
 *    	AuxCmd_Reset	  0xFF	Reset 
 *
 *    5.0 Reading Mouse Return Data
 *
 *    	Mk42_CmdRdAux		=> Port64
 *    	Mouse Return Data	<= Port60
 *
 *
-*/



/* i8242 Keyboard/Mouse Controller Port usage:
/*    Port 60h:
/*	Auxilliary Device Cmds/Data
/*	  - IF     preceded by the command D4h "Output next rcv byte from sys to Auxiliary Device"
/*	    - Writes Auxiliary Command and associated command data
/*	  - Reads  Auxiliary Device Data
/*	Keyboard Cmds/Data
/*	  - IF NOT preceded by the command D4h "Output next rcv byte from sys to Auxiliary Device"
/*	    - Writes Keyboard Commands and associated command Data
/*	    - Reads  Keyboard Data
/*	i8242 Command Data
/*	  - Writes associated i8242 Command Data
/*
/*    Port 64h:
/*	- Writes i8242 commands
/*	- Reads  i8242 status
*/
#define Mk42_Port64 	0x64	/* Intel 8242         Read Status, Write Cmd */
#define Mk42_Port60 	0x60	/* i8242/Aux/KeyBoard Cntlr Read Data,   Write Cmd */


/* Read Port64 status register bit vectors
*/
#define	KbStat$V_OBF	0x01	/* 8242/Keyboard Output Buffer Full */
#define	KbStat$V_IBF	0x02	/* Input Buffer Full */
#define	KbStat$V_F0	0x40	/* Flag 0, System Flag */
#define	KbStat$V_F1	0x80	/* Flag 1, Command/Data */
#define	KbStat$V_KBEN	0x10	/* Inhibited Switch */
#define	KbStat$V_AuxOBF	0x20	/* Auxiliary Device Output Buffer Full */
#define	KbStat$V_GTO	0x40	/* General Timeout */
#define	KbStat$V_PERR	0x80	/* Parity Error */



/* MultiKey/42 Memory Map 
*/
#define Mk42_Temp0	0x00	/* Temp Register */
#define Mk42_Temp1	0x01	/* Temp register */
#define Mk42_KCMISC	0x02	/* KeyBrd Cntlr Misc Flags */
#define Mk42_KCSTATE	0x03	/* keyBrk Cntlr State Flags */
#define Mk42_PENDING	0x04	/* Storage for the OBF Pending Data*/
#define Mk42_TEMP2	0x05	/* Temp Register */
#define Mk42_TIMEOUT	0x06	/* KeyBrd Cntlr Timeout Flags */
#define Mk42_TEMP3	0x07	/* Temp Register*/
#define Mk42_STACK	0x08	/* Program Stack (16 bytes) 8-17h */

#define Mk42_INDEX	0x18	/* Pheonix Extended Mem Index*/
#define Mk42_KSTATE1	0x19	/* Keyboard ScanCode Set, LED State*/
#define Mk42_KSTATE2	0x1A	/* Keyboard Typematic Delay and Rate */
#define Mk42_LASTKBD	0x1B	/* Storage for the Last ScanCode*/
#define Mk42_TMRATES	0x1C	/* 60us Min Interface inactive time*/

#define Mk42_KCCB	0x20	/* KeyBrd Cntlr Command Byte */
#define Mk42_RETRY	0x21	/* Num times to resend a xmissions*/
#define Mk42_KBDRSP	0x22	/* */
#define Mk42_KSRSND	0x23	/* */

#define Mk42_TMRATE1	0x27	/* 380us  Timer Value */
#define Mk42_TMRATE2	0x28	/* 2.4ms  Timer Value */
#define Mk42_TMRATE3	0x29	/* 11.7ms Timer Value*/

#define Mk42_BREAK	0x2D	/* Break-Code (00h or 80h)*/

#define Mk42_AUXRSP	0x30	/* */
#define Mk42_ARESND	0x31	/* */

#define Mk42_PWNULL1	0x33	/* Sent with Password enable*/
#define Mk42_PWNULL2	0x34	/* Sent with Password disable*/

#define Mk42_PWSCAN1	0x36	/* */
#define Mk42_PWSCAN2	0x37	/* */

#define Mk42_PWINDEX 	0x40	/* Password Index*/
#define Mk42_PWAREA	0x41	/* Password Storage (9 bytes) */



/* i8242 CSR reset values 
*/
#define Mk42_ITemp0	0x00	/* Temp Register */
#define Mk42_ITemp1	0x00	/* Temp register */
#define Mk42_IKCMISC	0x00	/* KeyBrd Cntlr Misc Flags */
#define Mk42_IKCSTATE	0x00	/* keyBrk Cntlr State Flags */
#define Mk42_IPENDING	0x00	/* Storage for the OBF Pending Data*/
#define Mk42_ITEMP2	0x00	/* Temp Register */
#define Mk42_ITIMEOUT	0x00	/* KeyBrd Cntlr Timeout Flags */
#define Mk42_ITEMP3	0x00	/* Temp Register*/
#define Mk42_ISTACK	0x00	/* Program Stack (16 bytes) 8-17h */

#define Mk42_IINDEX	0x00	/* Pheonix Extended Mem Index*/
#define Mk42_IKSTATE1	0x00	/* Keyboard ScanCode Set, LED State*/
#define Mk42_IKSTATE2	0x00	/* Keyboard Typematic Delay and Rate */
#define Mk42_ILASTKBD	0x00	/* Storage for the Last ScanCode*/
#define Mk42_ITMRATES	0x00	/* 60us Min Interface inactive time*/

#define Mk42_IKCCB	0x00	/* KeyBrd Cntlr Command Byte */
#define Mk42_IRETRY	0x00	/* Num times to resend a xmissions*/
#define Mk42_IKBDRSP	0x00	/* */
#define Mk42_IKSRSND	0x00	/* */

#define Mk42_ITMRATE1	0x00	/* 380us  Timer Value */
#define Mk42_ITMRATE2	0x00	/* 2.4ms  Timer Value */
#define Mk42_ITMRATE3	0x00	/* 11.7ms Timer Value*/

#define Mk42_IBREAK	0x00	/* Break-Code (00h or 80h)*/

#define Mk42_IAUXRSP	0x00	/* */
#define Mk42_IARESND	0x00	/* */

#define Mk42_IPWNULL1	0x00	/* Sent with Password enable*/
#define Mk42_IPWNULL2	0x00	/* Sent with Password disable*/

#define Mk42_IPWSCAN1	0x00	/* */
#define Mk42_IPWSCAN2	0x00	/* */

#define Mk42_IPWINDEX 	0x00	/* Password Index*/
#define Mk42_IPWAREA	0x00	/* Password Storage (9 bytes) */


/* i8242 CSR Default Program values 
*/
#define Mk42_DTemp0	0x00	/* Temp Register */
#define Mk42_DTemp1	0x00	/* Temp register */
#define Mk42_DKCMISC	0x00	/* KeyBrd Cntlr Misc Flags */
#define Mk42_DKCSTATE	0x00	/* keyBrk Cntlr State Flags */
#define Mk42_DPENDING	0x00	/* Storage for the OBF Pending Data*/
#define Mk42_DTEMP2	0x00	/* Temp Register */
#define Mk42_DTIMEOUT	0x00	/* KeyBrd Cntlr Timeout Flags */
#define Mk42_DTEMP3	0x00	/* Temp Register*/
#define Mk42_DSTACK	0x00	/* Program Stack (16 bytes) 8-17h */

#define Mk42_DINDEX	0x00	/* Pheonix Extended Mem Index*/
#define Mk42_DKSTATE1	0x00	/* Keyboard ScanCode Set, LED State*/
#define Mk42_DKSTATE2	0x00	/* Keyboard Typematic Delay and Rate */
#define Mk42_DLASTKBD	0x00	/* Storage for the Last ScanCode*/
#define Mk42_DTMRATES	0x00	/* 60us Min Interface inactive time*/

#define Mk42_DKCCB	0x00	/* KeyBrd Cntlr Command Byte */
#define Mk42_DRETRY	0x00	/* Num times to resend a xmissions*/
#define Mk42_DKBDRSP	0x00	/* */
#define Mk42_DKSRSND	0x00	/* */

#define Mk42_DTMRATE1	0xF6	/* 380us  Timer Value */
#define Mk42_DTMRATE2	0xC4	/* 2.4ms  Timer Value */
#define Mk42_DTMRATE3	0x00	/* 11.7ms Timer Value*/

#define Mk42_DBREAK	0x00	/* Break-Code (00h or 80h)*/

#define Mk42_DAUXRSP	0x00	/* */
#define Mk42_DARESND	0x00	/* */

#define Mk42_DPWNULL1	0x00	/* Sent with Password enable*/
#define Mk42_DPWNULL2	0x00	/* Sent with Password disable*/

#define Mk42_DPWSCAN1	0x00	/* */
#define Mk42_DPWSCAN2	0x00	/* */

#define Mk42_DPWINDEX 	0x00	/* Password Index*/
#define Mk42_DPWAREA	0x00	/* Password Storage (9 bytes) */


/* MultKey/42 Standard i8242 Commands
/*	The Host WRITES i8242 Cmds                to Port 0x64.
/*	The Host WRITES i8242 Associated Cmd data to Port 0x60.
/*
/*	The Host READS i8242 Command Data         from Port 0x60.
/*	The Host READS i8242 Status               from Port 0x64.
*/

    /* The next four Standard Commands, read/write to/from RAM location 20-3Fh. 
    /* The address of the command, ie 0x02 0x22 0x42 0x62, all write to their
    /* respective RAM location 0x22.  
    /*
    /* Software can add to the RAM locations base with the offset of the 
    /* RAM location to R/W, ie Mk42_StdCmd40+2 == RAM Location 0x22.
    /*
    /* The entire RAM Memory can also be W/R using the extended commands B8-BBh
    */
#define Mk42_Cmd00	0x00	/* Read RAM 20-3Fh and send to system */
#define Mk42_Cmd20	0x20	/* Read from RAM 20-3Fh */
#define Mk42_Cmd40	0x40	/* Get byte from system and write RAM Location 20-3Fh */
#define Mk42_Cmd60	0x60	/* Write to  RAM 20-3Fh */

#define Mk42_CmdTstPas	0xA4	/* Test Password */
#define Mk42_CmdLdPas	0xA5	/* Load Password */

#define Mk42_CmdEnPas	0xA6	/* Enable Password */
#define Mk42_CmdDisAux	0xA7	/* Disable Aux Device */
#define Mk42_CmdEnAux	0xA8	/* Enable  Aux Device */
#define Mk42_CmdTstAux	0xA9	/* Test    Aux Device clock and Data */
#define Mk42_CmdBist	0xAA	/* 8742 Self Test */
#define Mk42_CmdTstKb	0xAB	/* Test Keyboard Clock and Data lines */
#define Mk42_CmdNa	0xAC	/* Test Diag Dump not implemented */
#define Mk42_CmdDisKb	0xAD	/* Disable Keyboard */
#define Mk42_CmdEnKb	0xAE	/* Enable  Keyboard */
#define Mk42_CmdRdP1	0xC0	/* Emulate reading P1 and send data to system */
#define Mk42_CmdRdP30	0xC1	/* Puts P1[3:0] into status Register */
#define Mk42_CmdRdP74	0xC2	/* Puts P1[7:4] into status REgister */
#define Mk42_CmdRdP2	0xD0	/* Reads P2 */
#define Mk42_CmdGateA20	0xD1	/* Only set/clr GateA20 line based on sys data bit 1 */
#define Mk42_CmdLpKb	0xD2	/* LoopBack Keyboard */
#define Mk42_CmdLpAux	0xD3	/* LoopBack Aux Device */
#define Mk42_CmdWrAux	0xD4	/* Write Aux Device */
#define Mk42_CmdRdTst	0xE0	/* Read state of test inputs */


/* MultKey/42 Extended i8242 Commands
/*	The Host WRITES i8242 Ext Cmds                to Port 0x64.
/*	The Host WRITES i8242 Associated Ext Cmd data to Port 0x60.
/*
/*	The Host READS i8242 Ext Command Data         from Port 0x60.
/*	The Host READS i8242 Status                   from Port 0x64.
*/
#define Mk42_ExtCmdLdIndex	0xB8	/* Setup Pheonix ext Mem       access INDEX */
#define Mk42_ExtCmdGtIndex	0xB9	/* Get current Pheonix ext mem access INDEX */
#define Mk42_ExtCmdRdIndex	0xBA	/* Get current Pheonix ext mem ref by INDEX */
#define Mk42_ExtCmdWtIndex	0xBB	/* Write Pheonix ext mem       ref by INDEX */

#define Mk42_ExtCmdRdRam	0xBC	/* READ  RAM @VPointer */
#define Mk42_ExtCmdWtRam	0xBD	/* WRITE RAM @VPointer */

#define Mk42_ExtCmdSetP1	0xC7	/* Sets  P1 bits */
#define Mk42_ExtCmdClrP1	0xC8	/* Clear P1 Bits */
#define Mk42_ExtCmdSetP2	0xC9	/* Sets  p2 Bits */
#define Mk42_ExtCmdClrP2	0xCA	/* Clear P2 bits */

#define Mk42_ExtCmdSetFst	0xD3	/* Set   Fast GateA20 */
#define Mk42_ExtCmdClrFst	0xD4	/* Clear Fast GateA20 */

#define Mk42_ExtCmdRdVer	0xD5	/* Read Phoenix Version Number */
#define Mk42_ExtCmdRdInfo	0xD6	/* Read Version Info */
#define Mk42_ExtCmdRdModel	0xD7	/* Read Model Number */

#define Mk42_ExtCmdSetA20	0xD8	/* Set   Fast GateA20 */
#define Mk42_ExtCmdClrA20	0xD9	/* Clear Fast GateA20 */

#define Mk42_ExtCmdSetP2x	0xE0	/* Sets P2.x bits */


/* KeyBoard Commands
/*	The Host WRITES KeyBoard Cms and Associated cmd data to Port 0x60.
/*
/*	The Host READS KeyBoard data from Port 0x60.
*/
#define KbCmd_SetLeds	0xED	/* Set LEDs */
#define KbCmd_Echo	0xEE	/* Echo */
#define KbCmd_InvalCmd	0xEF	/* Invalid Command */
#define KbCmd_SelAltScn 0xF0	/* Select alternate scan code set */
#define KbCmd_RdID	0xF2	/* Read ID bytes */
#define KbCmd_SetTypDly	0xF3	/* Set typematic delay and rate */
#define KbCmd_EnKb	0xF4	/* Enable keyboard */
#define KbCmd_DisKb	0xF5	/* Disable Keyboard and set defaults */
#define KbCmd_SetDefs	0xF6	/* Set defaults */
#define KbCmd_SetTyp	0xF7	/* Set all keys typematic */
#define KbCmd_SetMkBrk	0xF8	/* Set all keys mark/break */
#define KbCmd_SetMk	0xF9	/* Set all keys mark only */
#define KbCmd_Resend	0xFE	/* Resend the last transmission */
#define KbCmd_Reset	0xFF	/* BAT, Reset the defaults and buffers */


/* Auxiliary Device Commands
/*	The Host WRITES Auxiliary Cmds and Associated Cmd data by:
/*	  - first issue an i8242    Cmd 0xD4 to port 0x64.
/*	  - next  issue an Aux Dev Cmd/Data to port 0x60.
/*
/*	The Host READS Auxiliary Device data from Port 0x60.
*/
#define AuxCmd_RstScal	  0xE6	/* Reset Scalling */
#define AuxCmd_SetScal	  0xE7	/* Set Scaling */
#define AuxCmd_SetRes	  0xE8	/* Set Resolution */
#define AuxCmd_GetStat    0xE9	/* Status Request */
#define AuxCmd_SetStmMode 0xEA	/* Set Stream Mode */
#define AuxCmd_RdData	  0xEB	/* Read Data */
#define AuxCmd_RstWrpMode 0xEC	/* Reset Wrap Mode */
#define AuxCmd_InvalCmd	  0xED	/* Invalid command */
#define AuxCmd_SetWrpMode 0xEE	/* Set Wrap Mode */
#define AuxCmd_SetRmtMode 0xF0	/* Set Remote Mode */
#define AuxCmd_RdDevType  0xF2	/* Read Device Type */
#define AuxCmd_SetSmpRate 0xF3	/* Set Sampling Rate */
#define AuxCmd_EnAuxDev	  0xF4	/* Enable Auxiliary Device */
#define AuxCmd_DisAuxDev  0xF5	/* Disable Auxiliary Device */
#define AuxCmd_SetDefVals 0xF6	/* Set Default Values */
#define AuxCmd_Resend	  0xFE	/* Resend */
#define AuxCmd_Reset	  0xFF	/* Reset */

