/* file:	chstate.h
 *
 * Copyright (C) 1992, 1993 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *
 *	This module implements the CH_STATE structure.
 *  This structure is used to keep the state for
 *  the character and screen functions.
 *
 *  AUTHORS:
 *
 *      J. A.  DeNisco
 *
 *  CREATION DATE:
 *
 *      13-Aug-1993
 *
 *  MODIFICATION HISTORY:
 *
 *	jrk	 6-Oct-1994	Align structures.
 *
 *	jad	13-aug-1993	Initial Entry.
 *
 *--
 */

/*Function codes*/
#define ENA_CURSOR 0
#define DIS_CURSOR 1

/*ch_state structure*/
struct CH_STATE {
	int *pb;		/* Back pointer to the port block	*/
	void (*putc)();		/* Pointer to the putchar routine	*/
	void (*getc)();		/* Pointer to the getchar routine	*/
	void (*ioctl)();	/* Pointer to the i/o control routine	*/
	int pci;		/* indicates PCI card is present */
	int row;
	int col;
	int scroll_in_prog;
	int scroll_jump;
	int bell_in_prog;
	ULONG VideoCardType;	/* Type of video */
	USHORT PushedXPosition;
	USHORT PushedYPosition;
	unsigned char CurrentAttribute;	/* Encoded as follows:	*/
				/*				*/
				/* 7 6 5 4 | 3 2 1 0 		*/
				/* B b b b   f f f f		*/
				/*				*/
				/* Where B is the blink enable bit. */
				/*  b is the 3-bit backround color (0-7) */
				/*  f is the 4-bit foreground color (0-15) */
	unsigned char pci_slot;	/* PCI slot number */
	unsigned char rsvd1;
	unsigned char rsvd2;
	ARC_DISPLAY_STATUS ds;	/* Display State			*/
	struct ANSI as;		/* Ansi state                           */
};

/*Pointer to the void function definition*/
typedef void (*PVF) (void);

/*Pointer to the paputchar function definition*/
typedef char (*PPF) (struct CH_STATE *chs,char ch,unsigned char attribute,
							 int xpos,int ypos);

/*Pointer to the pagetchar function definition*/
typedef char (*PGF) (struct CH_STATE *chs,char *ch,unsigned char *attribute,
							 int xpos,int ypos);

/*Pointer to the i/o control function definition*/
typedef void (*PICF) (int function);
