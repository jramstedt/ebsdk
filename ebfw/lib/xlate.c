
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: xlate.c,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $";
#endif

/*
 * $Log: xlate.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:26  fdh
 * Initial revision
 *
 */



#include "xlate.h"

/*
 * Index by scan code from a 101 key
 * AT style keyboard. The keyboard driver maps the
 * keycodes from the 84 key keyboard into the
 * keycodes from the 101 key keyboard.
 */

xlate_t xlate[] = {
 /* unshifted  shifted  type    */	/* Scan, key number		*/
 { 	0,	0,	UNKNOWN	},	/* 0x00, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x01, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x02, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x03, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x04, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x05, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x06, Key num 0 		*/
 { 	F1,	F1,	FUNCTION},	/* 0x07, Key num 112 		*/
 { 	033,	033,	ASCII	},	/* 0x08, Key num 110, ESC 	*/
 { 	0,	0,	UNKNOWN	},	/* 0x09, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x0a, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x0b, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x0c, Key num 0 		*/
 { 	'\t',	'\t',	ASCII	},	/* 0x0D, Key num 16 		*/
 { 	'`',	'~',	ASCII	},	/* 0x0E, Key num 1 		*/
 { 	F2,	F2,	FUNCTION},	/* 0x0F, Key num 113 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x10, Key num 0 		*/
 { 	0,	0,	LCTRL	},	/* F0 0x11, Key num 58 		*/
 { 	0,	0,	LSHIFT	},	/* F0 0x12, Key num 44 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x13, Key num 0 		*/
 { 	0,	0,	CAPS	},	/* F0 0x14, Key num 30 		*/
 { 	'q',	'Q',	ASCII	},	/* 0x15, Key num 17 		*/
 { 	'1',	'!',	ASCII	},	/* 0x16, Key num 2 		*/
 { 	F3,	F3,	FUNCTION},	/* 0x17, Key num 114 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x18, Key num 0 		*/
 { 	0,	0,	ALT	},	/* F0 0x19, Key num 60 		*/
 { 	'z',	'Z',	ASCII	},	/* 0x1A, Key num 46 		*/
 { 	's',	'S',	ASCII	},	/* 0x1B, Key num 32 		*/
 { 	'a',	'A',	ASCII	},	/* 0x1C, Key num 31 		*/
 { 	'w',	'W',	ASCII	},	/* 0x1D, Key num 18 		*/
 { 	'2',	'@',	ASCII	},	/* 0x1E, Key num 3 		*/
 { 	F4,	F4,	FUNCTION},	/* 0x1F, Key num 115 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x20, Key num 0 		*/
 { 	'c',	'C',	ASCII	},	/* 0x21, Key num 48 		*/
 { 	'x',	'X',	ASCII	},	/* 0x22, Key num 47 		*/
 { 	'd',	'D',	ASCII	},	/* 0x23, Key num 33 		*/
 { 	'e',	'E',	ASCII	},	/* 0x24, Key num 19 		*/
 { 	'4',	'$',	ASCII	},	/* 0x25, Key num 5 		*/
 { 	'3',	'#',	ASCII	},	/* 0x26, Key num 4 		*/
 { 	F5,	F5,	FUNCTION},	/* 0x27, Key num 116 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x28, Key num 0 		*/
 { 	' ',	' ',	ASCII	},	/* 0x29, Key num 61 		*/
 { 	'v',	'V',	ASCII	},	/* 0x2A, Key num 49 		*/
 { 	'f',	'F',	ASCII	},	/* 0x2B, Key num 34 		*/
 { 	't',	'T',	ASCII	},	/* 0x2C, Key num 21 		*/
 { 	'r',	'R',	ASCII	},	/* 0x2D, Key num 20 		*/
 { 	'5',	'%',	ASCII	},	/* 0x2E, Key num 6 		*/
 { 	F6,	F6,	FUNCTION},	/* 0x2F, Key num 117 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x30, Key num 0 		*/
 { 	'n',	'N',	ASCII	},	/* 0x31, Key num 51 		*/
 { 	'b',	'B',	ASCII	},	/* 0x32, Key num 50 		*/
 { 	'h',	'H',	ASCII	},	/* 0x33, Key num 36 		*/
 { 	'g',	'G',	ASCII	},	/* 0x34, Key num 35 		*/
 { 	'y',	'Y',	ASCII	},	/* 0x35, Key num 22 		*/
 { 	'6',	'^',	ASCII	},	/* 0x36, Key num 7 		*/
 { 	F7,	F7,	FUNCTION},	/* 0x37, Key num 118 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x38, Key num 0 		*/
 { 	0,	0,	ALT	},	/* 0x39, Key num 62 		*/
 { 	'm',	'M',	ASCII	},	/* 0x3A, Key num 52 		*/
 { 	'j',	'J',	ASCII	},	/* 0x3B, Key num 37 		*/
 { 	'u',	'U',	ASCII	},	/* 0x3C, Key num 23 		*/
 { 	'7',	'&',	ASCII	},	/* 0x3D, Key num 8 		*/
 { 	'8',	'*',	ASCII	},	/* 0x3E, Key num 9 		*/
 { 	F8,	F8,	FUNCTION},	/* 0x3F, Key num 119 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x40, Key num 0 		*/
 { 	',',	'<',	ASCII	},	/* 0x41, Key num 53 		*/
 { 	'k',	'K',	ASCII	},	/* 0x42, Key num 38 		*/
 { 	'i',	'I',	ASCII	},	/* 0x43, Key num 24 		*/
 { 	'o',	'O',	ASCII	},	/* 0x44, Key num 25 		*/
 { 	'0',	')',	ASCII	},	/* 0x45, Key num 11 		*/
 { 	'9',	'(',	ASCII	},	/* 0x46, Key num 10 		*/
 { 	F9,	F9,	FUNCTION},	/* 0x47, Key num 120 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x48, Key num 0 		*/
 { 	'.',	'>',	ASCII	},	/* 0x49, Key num 54 		*/
 { 	'/',	'?',	ASCII	},	/* 0x4A, Key num 55 		*/
 { 	'l',	'L',	ASCII	},	/* 0x4B, Key num 39 		*/
 { 	';',	':',	ASCII	},	/* 0x4C, Key num 40 		*/
 { 	'p',	'P',	ASCII	},	/* 0x4D, Key num 26 		*/
 { 	'-',	'_',	ASCII	},	/* 0x4E, Key num 12 		*/
 { 	F10,	F10,	FUNCTION},	/* 0x4F, Key num 121 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x50, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x51, Key num 0 		*/
 { 	'\'',	'\"',	ASCII	},	/* 0x52, Key num 41 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x53, Key num 0 		*/
 { 	'[',	'{',	ASCII	},	/* 0x54, Key num 27 		*/
 { 	'=',	'+',	ASCII	},	/* 0x55, Key num 13 		*/
 { 	F11,	F11,	FUNCTION},	/* 0x56, Key num 122 		*/
 { 	PRTSC,	PRTSC,	FUNCTION},	/* 0x57, Key num 124 		*/
 { 	0,	0,	RCTRL	},	/* 0x58, Key num 64 		*/
 { 	0,	0,	RSHIFT	},	/* F0 0x59, Key num 57 		*/
 { 	'\r',	'\r',	ASCII	},	/* 0x5A, Key num 43 		*/
 { 	']',	'}',	ASCII	},	/* 0x5B, Key num 28 		*/
 { 	'\\',	'|',	ASCII	},	/* 0x5C, Key num 29 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x5d, Key num 0 		*/
 { 	F12,	F12,	FUNCTION},	/* 0x5E, Key num 123 		*/
 { 	0,	0,	SCROLL	},	/* 0x5F, Key num 125 		*/
 { 	DOWN,	DOWN,	NUMPAD	},	/* 0x60, Key num 84 		*/
 { 	LEFT,	LEFT,	NUMPAD	},	/* 0x61, Key num 79 		*/
 { 	PAUSE,	PAUSE,	FUNCTION},	/* 0x62, Key num 126 		*/
 { 	UP,	UP,	NUMPAD	},	/* 0x63, Key num 83 		*/
 { 	DEL,	DEL,	NUMPAD	},	/* 0x64, Key num 76 		*/
 { 	END,	END,	NUMPAD	},	/* 0x65, Key num 81 		*/
 { 	MCTRL('H'), MCTRL('H'),	ASCII }, /* 0x66, Key num 15 		*/
 { 	INS,	INS,	NUMPAD	},	/* 0x67, Key num 75 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x68, Key num 0 		*/
 { 	END,	'1',	NUMPAD	},	/* 0x69, Key num 93 		*/
 { 	RIGHT,	RIGHT,	NUMPAD	},	/* 0x6A, Key num 89 		*/
 { 	LEFT,	'4',	NUMPAD	},	/* 0x6B, Key num 92 		*/
 { 	HOME,	'7',	NUMPAD	},	/* 0x6C, Key num 91 		*/
 { 	PGDWN,	PGDWN,	NUMPAD	},	/* 0x6D, Key num 86 		*/
 { 	HOME,	HOME,	NUMPAD	},	/* 0x6E, Key num 80 		*/
 { 	PGUP,	PGUP,	NUMPAD	},	/* 0x6F, Key num 85 		*/
 { 	INS,	'0',	NUMPAD	},	/* 0x70, Key num 99 		*/
 { 	DEL,	'.',	NUMPAD	},	/* 0x71, Key num 104 		*/
 { 	DOWN,	'2',	NUMPAD	},	/* 0x72, Key num 98 		*/
 { 	MIDDLE,	'5',	NUMPAD	},	/* 0x73, Key num 97 		*/
 { 	RIGHT,	'6',	NUMPAD	},	/* 0x74, Key num 102 		*/
 { 	UP,	'8',	NUMPAD	},	/* 0x75, Key num 96 		*/
 { 	0,	0,	NUMLOCK	},	/* 0x76, Key num 90 		*/
 { 	'/',	'/',	ASCII	},	/* 0x77, Key num 95 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x78, Key num 0 		*/
 { 	'\r',	'\r',	ASCII  },	/* F0 0x79, Key num 108 	*/
 { 	PGDWN,	'3',	NUMPAD	},	/* 0x7A, Key num 103 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x7b, Key num 0 		*/
 { 	'+',	'+',	ASCII	},	/* 0x7C, Key num 106 		*/
 { 	PGUP,	'9',	NUMPAD	},	/* 0x7D, Key num 101 		*/
 { 	'*',	'*',	ASCII	},	/* 0x7E, Key num 100 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x7f, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x80, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x81, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x82, Key num 0 		*/
 { 	0,	0,	UNKNOWN	},	/* 0x83, Key num 0 		*/
 { 	'-',	'-',	ASCII	}	/* 0x84, Key num 105 		*/
} ;
