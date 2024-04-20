/* KBD_TRANSLATIONS.C
 *
 * Copyright (C) 1992, 1993, 1994 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Translation tables for the different language keyboards supported by
 *	the Alpha architecture.
 *
 *  AUTHORS:
 *
 *	Judith Gold
 *
 *  CREATION DATE:
 *
 *	02-May-1994
 *
 *  MODIFICATION HISTORY:
 *
 *	wcc	15-Oct-1996	Folded in changes for Japanese keyboard.
 *
 *	rhd	24-jun-1996	Defined followin scan codes
 *				PCXAL:
 *				(0x57=PRTSC, 0x5F=SCRLCK, 0x62=PAUSE).
 *				LK411:
 *				(0x28=LCMPCHR, 0x85=Muhenkan ,0x86=Henken),
 *				 0x87=Hiragen)
 *				Made corrections to japanese ansi kbd.
 * 
 *	wcc	27-Aug-1996	Converted Quoted '' stringes to there hex value.
 *
 *	rhd	27-jun-1996	Added support for japanese for ANSI and JIS KBD
 *					struct xlate_t pcxaj_aj_japanese_diffs[]
 *					struct xlate_t lk411jj_japanese_diffs[]
 *					struct xlate_t lk411aj_japanese_diffs[]
 *
 *	dm	31-May-1996	Added support for foreign languages on
 *				lk411 keyboards and corrected missing
 *				values for pcxal keyboards.  Added support
 * 				for alt key functions for both sets of
 *				keyboards.
 *
 *	al	4-Mar-1996	Major changes again.  The old translation
 *                              table kept string pointers for all codes,
 *                              even though most codes translated to just
 *                              a single character.  This was to allow for
 *                              multi-byte sequences, such as what the
 *                              function keys generate.  I made the main
 *                              translation table just a character table, and
 *                              created a multi-byte table for just those
 *                              codes that needed it.  This is much more
 *                              efficient and easily understood.  This saves
 *                              4.6 Kbytes of space.  This work is in
 *                              preparation for adding language support for
 *                              all remaining languages, which is about a
 *                              dozen.
 *
 *	al	23-Aug-1995	Major changes to support PCXAL and LK411
 *				keyboards.  A set of language tables can
 *	      			now be kept for each keyboard.  An
 *		 		environment variable will be used to set
 *				the keyboard type (PCXAL or LK411).  Since
 *				complaints from France were the impetus for
 *				this change, just the french tables have
 *				been created for each keyboard, the rest of
 *				the tables are shared, and may be changed
 *				in the future on an as-needed basis.
 *
 *      al      18-Apr-1995     Fix french for ptt evt101104
 *
 *	rbb	13-Mar-1995	Conditionalize for EB164
 *
 *	noh	10-Feb-1995	Corrected various compile errors when compiling
 *				on ANSI-C. Included common_protos.h
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	jrk	10-Nov-1994	fix for Turbo, add function keys
 *
 *	er	15-aug-1994	Conditionalize for SD164
 *
 *	er	23-jun-1994	Conditionalize for EB64+
 *
 *	dtr	13-may-1994	ADded in the rest of the keyboard codes.  This
 *				needs to be revisited when dead diacritical are
 *				decided upon.
 *		
 *	dtr	11-may-1994	turned on the other platform conditionals.
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:kbd.h"

/*
This table shows all scan codes for North American keyboards, and how they
translate for both the pcxal and the lk411.  The "result" column shows whether
the scan code is interpreted the same, or differently, on both keyboards.

                 PCXAL                      LK411
                 =====                      =====
Scan
Code  	unshifted     shifted       unshifted     shifted        result
====	============  ============  ============  ============   ======
0x00    unused        unused        unused        unused         same
0x01    unused        unused        unused        unused         same
0x02    unused        unused        unused        unused         same
0x03    unused        unused        unused        unused         same
0x04    unused        unused        unused        unused         same
0x05    unused        unused        unused        unused         same
0x06    unused        unused        unused        unused         same
0x07    F1            F1            F1            F1             same*0
0x08    0x1b, ESC     0x1b, ESC     0x60, `       0x7e, ~        diff*1
0x09    unused        unused        F13           F13            diff*2
0x0a    unused        unused        F14           F14            diff
0x0b    unused        unused        Help          Help           diff
0x0c    unused        unused        Do            Do             diff
0x0d    0x09, tab     0x09, tab     0x09, tab     0x09, tab      same
0x0e    0x60, `       0x7e, ~       0x3c, <       0x3e, >        diff*3
0x0f    F2            F2            F2            F2             same
0x10    unused        unused        F17           F17            diff
0x11    LCtrl         LCtrl         LCtrl         LCtrl          same
0x12    LShift        LShift        LShift        LShift         same
0x13    unused        unused        0x2d, kp -    0x2d, kp -     diff*4
0x14    caps          caps          caps          caps           same
0x15    0x71, q       0x51, Q       0x71, q       0x51, Q        same
0x16    0x31, 1       0x21, !       0x31, 1       0x21, !        same
0x17    F3            F3            F3            F3             same
0x18    unused        unused        unused        unused         same*5
0x19    LAlt          LAlt          LAlt          LAlt           same
0x1a    0x7a, z       0x5a, Z       0x7a, z       0x5a, Z        same
0x1b    0x73, s       0x53, S       0x73, s       0x53, S        same
0x1c    0x61, a       0x41, A       0x61, a       0x41, A        same
0x1d    0x77, w       0x57, W       0x77, w       0x57, W        same
0x1e    0x32, 2       0x40, @       0x32, 2       0x40, @        same
0x1f    F4            F4            F4            F4             same
0x20    unused        unused        unused        unused         same
0x21    0x63, c       0x43, C       0x63, c       0x43, C        same
0x22    0x78, X       0x58, X       0x78, x       0x58, X        same
0x23    0x64, d       0x44, D       0x64, d       0x44, D        same
0x24    0x65, e       0x45, E       0x65, e       0x45, E        same
0x25    0x34, 4       0x24, $       0x34, 4       0x24, $        same
0x26    0x33, 3       0x23, #       0x33, 3       0x23, #        same
0x27    F5            F5            F5            F5             same
0x28    unused        unused        unused        unused         same
0x29    0x20, Space   0x20, Space   0x20, Space   0x20, Space    same
0x2a    0x76, v       0x76, V       0x76, v       0x76, V        same
0x2b    0x66, f       0x46, F       0x66, f       0x46, F        same
0x2c    0x74, t       0x54, T       0x74, t       0x54, T        same
0x2d    0x72, r       0x52, R       0x72, r       0x52, R        same
0x2e    0x35, 5       0x25, %       0x35, 5       0x25, %        same
0x2f    F6            F6            F6            F6             same
0x30    unused        unused        unused        unused         same
0x31    0x6e, N       0x4e, N       0x6e, n       0x4e, N        same
0x32    0x62, b       0x42, B       0x62, b       0x42, B        same
0x33    0x68, h       0x48, H       0x68, h       0x48, H        same
0x34    0x67, g       0x47, G       0x67, g       0x47, G        same
0x35    0x79, y       0x59, Y       0x79, y       0x59, Y        same
0x36    0x36, 6       0x5e, ^       0x36, 6       0x5e, ^        same
0x37    F7            F7            F7            F7             same
0x38    unused        unused        unused        unused         same
0x39    RAlt          RAlt          RAlt          RAlt           same
0x3a    0x6d, m       0x4d, M       0x6d, m       0x4d, M        same
0x3b    0x6a, j       0x4a, J       0x6a, j       0x4a, J        same
0x3c    0x75, u       0x55, U       0x75, u       0x55, U        same
0x3d    0x37, 7       0x26, &       0x37, 7       0x26, &        same
0x3e    0x38, 8       0x2a, *       0x38, 8       0x2a, *        same
0x3f    F8            F8            F8            F8             same
0x40    unused        unused        unused        unused         same
0x41    0x1c, ,       0x3c, <       0x1c, ,       0x3c, <        same
0x42    0x6b, k       0x4b, K       0x6b, k       0x4b, K        same
0x43    0x69, i       0x49, I       0x69, i       0x49, I        same
0x44    0x6f, o       0x4f, O       0x6f, o       0x4f, O        same
0x45    0x30, 0       0x29, )       0x30, 0       0x29, )        same
0x46    0x39, 9       0x28, (       0x39, 9       0x28, (        same
0x47    F9            F9            F9            F9             same
0x48    unused        unused        unused        unused         same
0x49    0x1e, .       0x3e, >       0x1e, .       0x3e, >        same
0x4a    0x2f, /       0x3f, ?       0x2f, /       0x3f, ?        same
0x4b    0x6c, l       0x4c, L       0x6c, l       0x4c, L        same
0x4c    0x3b, ;       0x3a, :       0x3b, ;       0x3a, :        same
0x4d    0x70, p       0x50, P       0x70, p       0x50, P        same
0x4e    0x2d, -       0x5f, _       0x2d, -       0x5f, _        same
0x4f    F10           F10           F10           F10            same
0x50    unused        unused        unused        unused         same
0x51    unused        unused        unused        unused         same*6
0x52    0x27, '       0x22, "       0x27, '       0x22, "        same
0x53    unused        unused        unused        unused         same*7
0x54    0x5b, [       0x7b, {       0x5b, [       0x7b, {        same
0x55    0x3d, =       0x2b, +       0x3d, =       0x2b, +        same
0x56    F11           F11           F11           F11            same
0x57    Print         Print         F18           F18            diff
0x58    RCtrl         RCtrl         RCtrl         RCtrl          same
0x59    RShift        RShift        RShift        RShift         same
0x5a    0x0d, Enter   0x0d, Enter   0x0d, Return  0x0d, Return   same
0x5b    0x5d, ]       0x7d, }       0x5d, ]       0x7d, }        same
0x5c    0x5c, \       0x7c, |       0x5c, \       0x7c, |        same*8
0x5d    unused        unused        unused        unused         same*9
0x5e    F12           F12           F12           F12            same
0x5f    Scroll        Scroll        F19           F19            diff
0x60    DownArrow     DownArrow     DownArrow     DownArrow      same
0x61    LeftArrow     LeftArrow     LeftArrow     LeftArrow      same
0x62    Pause         Pause         F20           F20            diff
0x63    UpArrow       UpArrow       UpArrow       UpArrow        same
0x64    Delete        Delete        Remove        Remove         same
0x65    End           End           Select        Select         diff
0x66    Backspace     Backspace     Backspace     Backspace      same
0x67    Insert        Insert        Insert        Insert         same
0x68    unused        unused        unused        unused         same
0x69    unused        0x31, kp 1    0x31, kp 1    0x31, kp 1     diff
0x6a    RightArrow    RightArrow    RightArrow    RightArrow     same
0x6b    LeftArrow,kp4 0x34, kp 4    0x34, kp 4    0x34, kp 4     diff
0x6c    unused        0x37, kp 7    0x37, kp 7    0x37, kp 7     diff
0x6d    Page Down     Page Down     Next          Next           same
0x6e    Home          Home          Find          Find           diff
0x6f    Page Up       Page Up       Previous      Previous       same
0x70    unused        0x30, kp 0    0x30, kp 0    0x30, kp 0     diff
0x71    unused        0x2e, kp .    0x2e, kp .    0x2e, kp .     diff
0x72    DownArrow,kp2 0x32, kp 2    0x32, kp 2    0x32, kp 2     diff
0x73    unused        0x35, kp 5    0x35, kp 5    0x35, kp 5     diff
0x74    RightArrow,kp6 0x36, kp 6   0x36, kp 6    0x36, kp 6     diff
0x75    UpArrow, kp8  0x38, kp 8    0x38, kp 8    0x38, kp 8     diff
0x76    Num Lock      Num Lock      PF1           PF1            diff
0x77    0x2f, KP /    0x2f, KP /    PF2           PF2            diff
0x78    unused        unused        unused        unused         same
0x79    0x0d, kp Ent  0x0d, kp Ent  0x0d,kp Ent   0x0d, kp Enter same
0x7a    unused        0x33, kp 3    0x33, kp 3    0x33, kp 3     diff
0x7b    unused        unused        unused        unused         same
0x7c    unused        0x2b, kp +    0x2c, kp ,    0x2c, kp ,     diff
0x7d    unused        0x39, kp 9    0x39, kp 9    0x39, kp 9     diff
0x7e    0x2a, kp *    0x2a, kp *    PF3           PF3            diff
0x7f    unused        unused        unused        unused         same
0x80    unused        unused        unused        unused         same
0x81    unused        unused        unused        unused         same
0x82    unused        unused        unused        unused         same
0x83    unused        unused        unused        unused         same
0x84    0x2d, kp -    0x2d, kp -    PF4           PF4            same

IMPORTANT NOTES:
	*0 - scan code 07 is not used on any Japanese keyboards

	*1 - scan code 08 occurs from different key locations between the
	     lk411 and pcxal keyboards

	*2 - scan code 09 is F1 key on all Japanese keyboards

     	*3 - scan code 0e occurs from different key locations between the
	     lk411 and pcxal keyboards

	*4 - scan code 13 is unused on US PCXAL keyboards but IS used on
	     PCXAL keyboards for all other countries.

	*5 - scan code 18 is F13 on Japanese LK411 keyboard only.

	*6 - scan code 51 is ONLY used on LK411 and PCXAJ Japanese keyboards.

  	*7 - scan code 53 is unused on US PCXAL and LK411 keyboards but IS used
	     on PCXAL keyboards for all other countries and LK411 keyboards for
             Japanese keyboards only.

	*8 - scan code 5c is ONLY used on PCXAL keyboards for the US.  It is
	     not used on PCXAL keyboards for any other country.  In addition,
	     the key producing this keyboard is at different locations between
	     the US PCXAL keyboard and all LK411 keyboards.

	*9 - scan code 5d is ONLY used on LK411 and PCXAJ Japanese keyboards.

*/

/*
 * Main translation table for PCXAL and LK411/LK450 keyboards
 *
 * THIS TABLE IS FOR SINGLE CHARACTER TRANSLATIONS.  MULTI-BYTE TRANSLATIONS
 * ARE CONTAINED IN THE MULTI-BYTE TABLES.
 *
 * NOTE: the scancodes used here are those generated by the keyboard
 *       when it is in scanmode 3.
 *
 */

struct xlate_t xlate_table[] = {

 /* scan, unshifted,  shifted */

 { 0x07,     F1,      F1 },
 { 0x08,   0x1b,    0x1b },  /* PCXAL only, all LK411 will redefine. Esc */
 { 0x0D,   0x09,    0x09 },		/* Tab		*/
 { 0x0E,   0x60,    0x7e },  /* PCXAL only, all LK411 will redefine. '`', '~'*/
 { 0x15,   0x71,    0x51 },		/* 'q', 'Q'	*/
 { 0x16,   0x31,    0x21 },		/* '1', '!'	*/
 { 0x1A,   0x7a,    0x5a },		/* 'z', 'Z'	*/
 { 0x1B,   0x73,    0x53 },		/* 's', 'S'	*/
 { 0x1C,   0x61,    0x41 },		/* 'a', 'A'	*/
 { 0x1D,   0x77,    0x57 },		/* 'w', 'W'	*/
 { 0x1E,   0x32,    0x40 },		/* '2', '@'	*/
 { 0x21,   0x63,    0x43 },		/* 'c', 'C'	*/
 { 0x22,   0x78,    0x58 },		/* 'x', 'X'	*/
 { 0x23,   0x64,    0x44 },		/* 'd', 'D'	*/
 { 0x24,   0x65,    0x45 },		/* 'e', 'E'	*/
 { 0x25,   0x34,    0x24 },		/* '4', '$'	*/
 { 0x26,   0x33,    0x23 },		/* '3', '#'	*/
 { 0x29,   0x20,    0x20 },		/* Space	*/
 { 0x2A,   0x76,    0x56 },		/* 'v', 'V'	*/
 { 0x2B,   0x66,    0x46 },		/* 'f', 'F'	*/
 { 0x2C,   0x74,    0x54 },		/* 't', 'T'	*/
 { 0x2D,   0x72,    0x52 },		/* 'r', 'R'	*/
 { 0x2E,   0x35,    0x25 },		/* '5', '%'	*/
 { 0x31,   0x6e,    0x4e },		/* 'n', 'N'	*/
 { 0x32,   0x62,    0x42 },		/* 'b', 'B'	*/
 { 0x33,   0x68,    0x48 },		/* 'h',	'H'	*/
 { 0x34,   0x67,    0x47 },		/* 'g', 'G'	*/
 { 0x35,   0x79,    0x59 },		/* 'y', 'Y'	*/
 { 0x36,   0x36,    0x5e },		/* '6', '^'	*/
 { 0x3A,   0x6d,    0x4d },		/* 'm', 'M'	*/
 { 0x3B,   0x6a,    0x4a },		/* 'j', 'J'	*/
 { 0x3C,   0x75,    0x55 },		/* 'u', 'U'	*/
 { 0x3D,   0x37,    0x26 },		/* '7', '&'	*/
 { 0x3E,   0x38,    0x2a },		/* '8', '*'	*/
 { 0x41,   0x2c,    0x3c },		/* ',', '<'	*/
 { 0x42,   0x6b,    0x4b },		/* 'k', 'K'	*/
 { 0x43,   0x69,    0x49 },		/* 'i', 'I'	*/
 { 0x44,   0x6f,    0x4f },		/* 'o', 'O'	*/
 { 0x45,   0x30,    0x29 },		/* '0', ')'	*/
 { 0x46,   0x39,    0x28 },		/* '9', '('	*/
 { 0x49,   0x2e,    0x3e },		/* '.', '>'	*/
 { 0x4A,   0x2f,    0x3f },		/* Slash, '?'	*/
 { 0x4B,   0x6c,    0x4c },		/* 'l', 'L'	*/
 { 0x4C,   0x3b,    0x3a },		/* ';', ':'	*/
 { 0x4D,   0x70,    0x50 },		/* 'p'', 'P'	*/
 { 0x4E,   0x2d,    0x5f },		/* '-', '_'	*/
 { 0x52,   0x27,    0x22 },		/* '\'', '\"'	*/
/* 0x53, ALL NON US PC keyboards ONLY, will need to define this key */
 { 0x54,   0x5b,    0x7b },		/* '[', '{'	*/
 { 0x55,   0x3d,    0x2b },		/* '=','+'	*/
 { 0x5A,   0x0d,    0x0d },		/* Carriage return */
 { 0x5B,   0x5d,    0x7d },		/* ']',  '}'	*/
 { 0x5C,   0x5c,    0x7c },		/* Backslash, '|' */
 { 0x66,   0x7f,    0x7f },		/* Delete	*/
 { 0x79,   0x0d,    0x0d },		/* Carriage return */
 { 0xfa,    0,	    0 },  		/* For mfg script that blinks LEDs */
 { 0xff,    0,	    0 }			/* Terminator */
} ;


struct multi_byte_xlate common_multi_byte_xlate_table [] =
{
   { 0x0f,   F2  },
   { 0x17,   F3  },
   { 0x1f,   F4  },
   { 0x27,   F5  },
   { 0x2f,   F6  },
   { 0x37,   F7  },
   { 0x3f,   F8  },
   { 0x47,   F9  },
   { 0x4f,   F10 },
   { 0x56,   F11 },
   { 0x5e,   F12 },
   { 0x60,   DOWN },
   { 0x61,   LEFT },
   { 0x63,   UP   },
   { 0x64,   DELETE },
   { 0x67,   INSERT },
   { 0x6A,   RIGHT },
   { 0x6D,   PGDWN },
   { 0x6f,   PGUP },
   { 0xff,   0 } /* Terminator */
} ;

struct multi_byte_xlate pcxal_multi_byte_xlate_table [] =
{
   { 0x57, PRTSC   },
   { 0x5F, SCRLCK  },
   { 0x62, PAUSE   },
   { 0x6b, LEFT},
   { 0x72, DOWN},
   { 0x74, RIGHT},
   { 0x75, UP},
   { 0x65, END },
   { 0x6e, HOME },
   { 0xff, 0 }
};

struct multi_byte_xlate lk411_multi_byte_xlate_table [] =
{
 { 0x09, F13    },
 { 0x0a, F14    },
 { 0x0b, HELP   },
 { 0x0c, DO     },
 { 0x10, F17    },
 { 0x57, F18    },
 { 0x5F, F19    },
 { 0x62, F20    },
 { 0x65, SELECT },
 { 0x6e, FIND   },
 { 0x28, LCMPCHR },
 { 0x76, PF1 },
 { 0x77, PF2 },
 { 0x7e, PF3 },
 { 0x84, PF4 },
 { 0x18, F13 },  /* used by Japanese keyboards only! */
 { 0xff, 0 }
};

struct multi_byte_xlate *(specific_multi_byte_xlate_table[]) =
{
   pcxal_multi_byte_xlate_table,
   lk411_multi_byte_xlate_table,
   0
} ;

struct keypad_xlate diff_pc_keypad_xlate_table[] =
{
   { 0x77,	DIVIDE,		0xf7},  /* keypad division symbol'÷' */
   { 0x7e,	MULTIPLY,	0xd7},  /* keypad multiplication symbol'×' */
   { 0xff,	0x00,  		0x00}   /* terminator */
} ;

struct keypad_xlate pcxal_keypad_xlate_table[] =
{
   { 0x69,  END,    0x31 },  /* keypad 1 */
   { 0x6b,  LEFT,   0x34 },  /* keypad 4 */
   { 0x6c,  HOME,   0x37 },  /* keypad 7 */
   { 0x70,  INSERT, 0x30 },  /* keypad 0 */
   { 0x71,  DELETE, 0x2e },  /* keypad . */
   { 0x72,  DOWN,   0x32 },  /* keypad 2 */
   { 0x73,  UP,     0x35 },  /* keypad 5 */
   { 0x74,  RIGHT,  0x36 },  /* keypad 6 */
   { 0x75,  UP,     0x38 },  /* keypad 8 */
   { 0x77,  SLASH,  0x2f },  /* keypad / */
   { 0x7a,  PGDWN,  0x33 },  /* keypad 3 */
   { 0x7c,  PLUS,   0x2b },  /* keypad + */
   { 0x7d,  PGUP,   0x39 },  /* keypad 9 */
   { 0x7e,  STAR,   0x2a },  /* keypad * */
   { 0x84,  MINUS,  0x2d },  /* keypad - */
   { 0xff,  0x00,   0x00 }
} ;

struct xlate_t lk411_number_pad_xlate_table[] =
{
   { 0x13,  0x2d, 0x2d },  /*keypad - */
   { 0x69,  0x31, 0x31 },  /*keypad 1 */
   { 0x6b,  0x34, 0x34 },  /*keypad 4 */
   { 0x6c,  0x37, 0x37 },  /*keypad 7 */
   { 0x70,  0x30, 0x30 },  /*keypad 0 */
   { 0x71,  0x2e, 0x2e },  /*keypad . */
   { 0x72,  0x32, 0x32 },  /*keypad 2 */
   { 0x73,  0x35, 0x35 },  /*keypad 5 */
   { 0x74,  0x36, 0x36 },  /*keypad 6 */
   { 0x75,  0x38, 0x38 },  /*keypad 8 */
   { 0x7a,  0x33, 0x33 },  /*keypad 3 */
   { 0x7c,  0x2c, 0x2c },  /*keypad , */
   { 0x7d,  0x39, 0x39 },  /*keypad 9 */
   { 0xff,     0,    0 }
} ;

struct xlate_t common_number_pad_xlate_table[] =
{
   { 0x79,   0x0d,  0x0d },  /*keypad Enter */
   { 0xff,     0,    0 }
} ;

/***************************************************************************
 * Differences between keyboards.  Besides language differences, there are *
 * differences between, for example, an LK411 and a PCXAL keyboard.  The   *
 * main translation table keeps the defaults, then the language changes    *
 * for that type of keyboard are copied in.                                *
 ***************************************************************************/

/**********************************************************************
 *  PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL PCXAL *
 **********************************************************************/

struct xlate_t pcxal_danish_diffs[] = { /* Dansk */
/* keycode  unshifted  		shifted  */	/*language 0x30 */
 { 0x0e,	0xbd,		0xa7},		/* '½', section'§' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34, 		0xa4},		/* '4', currency'¨' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xe6,		0xc6},		/* 'æ', 'Æ'	*/
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x52,	0xf8,		0xd8},		/* oslash'ø', Ooblique'Ø' */
 { 0x53,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x54,       	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x55,	0xb4, 		0x60},		/* acute, '`'	*/
 { 0x5b,	0xa8,		0x5e}, 		/* diaeresis'¨', '^' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_austrian_german_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x32 */
 { 0x0e,	0x5e,		0xb0},		/* '^', degree'°' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x26,       	0x33,		0xa7},		/* '3', section '§'*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö'	*/
 { 0x4e,	0xdf,		0x3f},		/* ssharp'ß', '?' */
 { 0x52,	0xe4, 		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä' */
 { 0x53,	0x23,		0x27},		/* '#', '\''	*/
 { 0x54,	0xfc,		0xdc},		/* udiaeresis'ü', Udiaeresis'Ü' */
 { 0x55,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x5b,	0x2b,		0x2a},		/* '+', '*'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_swiss_german_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x34 */
 { 0x0e,	0xa7,		0xb0},		/* section'§', degree'°' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0x31,		0x2b},		/* '1', '+'	*/
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34,		0xe7},		/* '4', ccedilla'ç' */
 { 0x26,	0x33,		0x2a},		/* '3', '*'	*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf6,		0xe9},		/* odiaeresis'ö', eacute'é' */
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x52,	0xe4, 		0xe0},		/* adiaeresis'ä', agrave'à' */
 { 0x53,	0x24,		0xa3},		/* '$', sterling'£' */
 { 0x54,	0xfc,		0xe8},		/* udiaeresis'ü', egrave'è' */
 { 0x55,	0x5e,		0x60},		/* '^', '`'	*/
 { 0x5b,	0xa8,		0x21},		/* diaeresis'¨', '!' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_us_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x36 */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_uk_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x38 */
 { 0x0e,	0x60,		0xac},		/* '`', notsign	*/
 { 0x13,	0x5c,		0x7c},		/* Backslash, '|' */
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x26,	0x33,		0xa3},		/* '3', sterling'£' */
 { 0x52,       	0x27,		0x40},		/* '\'', '@'	*/
 { 0x53,	0x23,		0x7e},		/* '#', '~'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_spanish_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x3a */
 { 0x0e,	0xba,		0xaa},		/* masculine'º', ordfeminine'ª' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x26,	0x33,		0xb7},		/* 3, '·'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf1,		0xd1},		/* ntilde'ñ', Ntilde'Ñ' */
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x52,	0xb4,		0xa8},		/* acute, diaeresis'¨' */
 { 0x53,       	0xe7,		0xc7},		/* ccedilla'ç', Ccedilla'Ç' */
 { 0x54,	0x60, 		0x5e},		/* '`', '^'	*/
 { 0x55,	0xa1,		0xbf},		/* exclamdown'¡', qstndown'¿' */
 { 0x5b,	0x2b,		0x2a},		/* '+', '*'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_french_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x3c */
 { 0x0e,	0xb2,		0xb2},		/* '²', '²'	*/
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x15,	0x61,		0x41},		/* 'a', 'A'	*/
 { 0x16,	0x26,		0x31},		/* '&', '1'	*/
 { 0x1a,	0x77, 		0x57},		/* 'w', 'W'	*/
 { 0x1c,	0x71,		0x51},		/* 'q', 'Q'	*/
 { 0x1d,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x1e,	0xe9,		0x32},		/* eacute'é', '2' */
 { 0x25,	0xb4,		0x34},		/* acute, '4'	*/
 { 0x26,	0x22,		0x33},		/* '\"', '3'	*/
 { 0x2e,	0x28,		0x35},		/* '(', '5'	*/
 { 0x36,	0x2d,		0x36},		/* '-', '6'	*/
 { 0x3a,	0x2c,		0x3f},		/* ',', '?'	*/
 { 0x3d,	0xe8,		0x37},		/* egrave'è', '7' */
 { 0x3e,	0x5f,		0x38},		/* '_', '8'	*/
 { 0x41,	0x3b,		0x2e},		/* ';', '.'	*/
 { 0x45,	0xe0,		0x30},		/* agrave'à', '0' */
 { 0x46,	0xe7,		0x39},		/* ccedilla'ç', '9' */
 { 0x49,	0x3a,		0x2f},		/* ':', Slash	*/
 { 0x4a,	0x21,		0xa7},		/* '!', section sign'§' */
 { 0x4c,	0x6d,		0x4d},		/* 'm', 'M'	*/
 { 0x4e,	0x29,		0xb0},		/* ')', degree'°' */
 { 0x52,	0xf9,		0x25},		/* ugrave'ù', '%' */
 { 0x53,	0x2a,		0xb5},		/* '*', 'µ'	*/
 { 0x54,	0x5e,		0xa8},		/* '^', twoupperdots */
 { 0x5b,	0x24,		0xa3},		/* '$', sterling'£' */
 { 0xff, 	0, 		0   }		/* terminator */
};


struct xlate_t pcxal_french_canadian_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x3e */
 { 0x0e,       	0x23,		0x7c},		/* '#', '|'	*/
 { 0x13,	0xab,		0xbb},		/* guilemotlft'«', guilemotrgt'»' */
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x26,	0x33,		0x2f},		/* '3', Slash	*/
 { 0x36,	0x36,		0x3f},		/* '6', '?'	*/
 { 0x41,	0x2c,		0x2c},		/* ',', ','	*/
 { 0x49,	0x2e,		0x2e},		/* '.', '.'	*/
 { 0x4a,	0xe9,		0xc9},		/* eacute'é', Eacute'É' */
 { 0x4c,	0x3b,		0x3a},		/* ';', ':'	*/
 { 0x52,	0x60,		0x60},		/* '`', '`'	*/
 { 0x53,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x54,	0x5e,		0x5e},		/* '^', '^'	*/
 { 0x5b,	0xb8,		0xa8},		/* cedilla, diaeresis'¨' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_swiss_french_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x40 */
 { 0x0e,	0xa7,		0xb0},		/* section'§', degree'°' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0x31,		0x2b},		/* '1', '+'	*/
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34,		0xe7},		/* '4', ccedilla'ç' */
 { 0x26,	0x33,		0x2a},		/* '3', '*'	*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xe9,		0xf6},		/* eacute'é', odiaeresis'ö' */
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x52,	0xe0,		0xe4},		/* agrave'à', adiaeresis'ä' */
 { 0x53,	0x24,		0xa3},		/* '$', sterling'£' */
 { 0x54,	0xe8,		0xfc},		/* egrave'è', udiaeresis'ü' */
 { 0x55,	0x5e,		0x60},		/* '^', '`'	*/
 { 0x5b,	0xa8,		0x21},		/* diaeresis'¨', '!' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_italian_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x42 */
 { 0x0e,	0x5c,		0x7c},		/* Backslash, '|' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x26,       	0x33,		0xa3},		/* '3', sterling'£' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf2,		0xe7},		/* ograve'ò', ccedilla'ç' */
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x52,	0xe0,		0xb0},		/* agrave'à', degree'°' */
 { 0x53,	0xf9,		0xa7},		/* ugrave'ù', section'§' */
 { 0x54,	0xe8,		0xe9},		/* egrave'è', eacute'é' */
 { 0x55,       	0xec,		0x5e},		/* igrave'ì', '^' */
 { 0x5b,       	0x2b,		0x2a},		/* '+', '*'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_dutch_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x44 */
 { 0x0e,	0x40,		0xa7},		/* '@', section'§' */
 { 0x13,	0x5d,		0x5b},		/* ']', '['	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,		0x5f},		/* '7', '_'	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x27},		/* '0', '\''	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x3d},		/* '-',  '='	*/
 { 0x4c,	0x2b,		0xb1},		/* '+', plusminus'±' */
 { 0x4e,	0x2f,		0x3f},		/* Slash, '?'	*/
 { 0x52,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x53,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x54,	0xa8,		0x5e}, 		/* diaeresis'¨', '^' */
 { 0x55,	0xb0,		0x7e},		/* degree'°', '~' */
 { 0x5b,	0x2a,		0x7c},		/* '*', brokenbar */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_norwegian_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x46 */
 { 0x0e,	0x7c,		0xa7},		/* '|', section'§' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34, 		0xa4},		/* '4', currency'¨' */
 { 0x36,	0X36,		0x26},		/* '6',  '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf8,		0xd8},		/* oslash'ø', Ooblique'Ø' */
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x52,	0xe6,		0xc6},		/* 'æ', 'Æ'	*/
 { 0x53,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x54,       	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x55,	0x5c,		0x60},		/* Backslash, '`' */
 { 0x5b,	0xa8,		0x5e}, 		/* diaeresis'¨', '^' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_portuguese_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x48 */
 { 0x0e,	0x5c,		0x7c},		/* Backslash, '|' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34, 		0x24},		/* '4', '$'	*/
 { 0x36,	0X36,		0x26},		/* '6',  '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xe7,		0xc7},		/* ccedilla'ç', Ccedilla'Ç' */
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x52,	0xba,		0xaa},		/* masculine'º', ordfeminine'ª' */
 { 0x53,	0x7e,		0x5e},		/* '~', '^'	*/
 { 0x54,       	0x2b,		0x2a},		/* '+', '*'	*/
 { 0x55,	0xab,		0xbb},		/* guilemotlft'«', guilemotrgt'»' */
 { 0x5b,	0xb4,		0x60},		/* acute, '`'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_finnish_diffs[] = {
/* keycode     	unshifted	shifted */	/*language 0x4a */
 { 0x0e,	0xa7,		0xbd},		/* section'§', '½' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34, 		0xa4},		/* '4', currency'¨' */
 { 0x36,	0X36,		0x26},		/* '6',  '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö' */
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x52,	0xe4,		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä' */
 { 0x53,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x54,       	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x55,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x5b,	0xa8,		0x5e}, 		/* diaeresis'¨', '^' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_swedish_diffs[] = {
/* keycode	unshifted	shifted */	/*language 0x4c */
 { 0x0e,	0xa7,		0xbd},		/* section'§', '½' */
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x25,	0x34, 		0xa4},		/* '4', currency'¨' */
 { 0x36,	0X36,		0x26},		/* '6',  '&'	*/
 { 0x3d,	0x37,		0x2f},		/* '7', Slash	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö' */
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x52,	0xe4,		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä' */
 { 0x53,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x54,       	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x55,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x5b,	0xa8,		0x5e}, 		/* diaeresis'¨', '^' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t pcxal_belgian_diffs[] = {
/* keycode     	unshifted	shifted */	/*language 0x4e */
 { 0x0e,	0xb2,		0xb3},		/* '²', '³'	*/
 { 0x13,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x15,	0x61,		0x41},		/* 'a', 'A'	*/
 { 0x16,	0x26,		0x31},		/* '&', '1'	*/
 { 0x1a,	0x77, 		0x57},		/* 'w', 'W'	*/
 { 0x1c,	0x71,		0x51},		/* 'q', 'Q'	*/
 { 0x1d,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x1e,	0xe9,		0x32},		/* eacute'é', '2' */
 { 0x25,	0x27,		0x34},		/* '\'', '4'	*/
 { 0x26,	0x22,		0x33},		/* '\"', '3'	*/
 { 0x2e,	0x28,		0x35},		/* '(', '5'	*/
 { 0x36,	0xa7,		0x36},		/* section'§', '6' */
 { 0x3a,	0x2c,		0x3f},		/* ',', '?'	*/
 { 0x3d,	0xe8,		0x37},		/* egrave'è', '7' */
 { 0x3e,	0x21,		0x38},		/* '!', '8'	*/
 { 0x41,	0x3b,		0x2e},		/* ';', '.'	*/
 { 0x45,	0xe0,		0x30},		/* agrave'à', '0' */
 { 0x46,	0xe7,		0x39},		/* ccedilla'ç', '9' */
 { 0x49,	0x3a,		0x2f},		/* ':', Slash	*/
 { 0x4a,	0x3d,		0x2b},   	/* '=', '+'	*/
 { 0x4c,	0x6d,		0x4d},		/* 'm', 'M'	*/
 { 0x4e,	0x29,		0xb0},		/* ')', degree'°' */
 { 0x52,	0xf9,		0x25},		/* ugrave'ù', '%' */
 { 0x53,	0xb5,		0xa3},		/* mu'µ', sterling '£' */
 { 0x54,	0x5e,		0xa8},		/* '^', twoupperdots */
 { 0x55,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x5b,	0x24,		0x2a},		/* '$', '*'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

/* Japanese PCXAJ-Aj model keyboard is to be supported, based upon the specs
 * for the PCXAJ-AA and LK411-JJ Japanese keyboards.  So you should double
 * check the keycode values and corresponding symbols.
 */

struct xlate_t pcxaj_aj_japanese_diffs[] = {
/* keycode	unshifted	shifted		language 0x50,0x52 */
 { 0x0e,	0,		0x7e},		/* NoSymbol, '~' */
 { 0x1e,	0X32,		0x22},		/* '2',  '\"'	*/
 { 0x36,	0X36,		0x26},		/* '6',  '&'	*/
 { 0x3d,	0x37,		0x27},		/* '7',  '\''	*/
 { 0x3e,	0X38,		0x28},		/* '8',  '('	*/
 { 0x46,	0X39,		0x29},		/* '9',  ')'	*/
 { 0x45,	0X30,		0x7e},		/* '0',  '~'	*/
 { 0x4e,	0x2d,		0x3d},		/* '-',  '='	*/
 { 0x55,	0x5e,		0x7e},		/* '^',  '~'	*/
 { 0x5d,        0x5c,		0x7c},		/* Backslash ('¥'), '|' x key */
 { 0x54,	0x40,		0x60},		/* '@',  '`'	*/
 { 0x5b,	0x5b,		0x7b},		/* '[',  '{'	*/
 { 0x4c,	0x3b,		0x2b},		/* ';',  '+'	*/
 { 0x52,	0x3a,		0x2a},		/* ':',  '*'	*/
 { 0x53,	0x5d,		0x7d},		/* ']',  '}'	*/
 { 0x51,	0x5c,		0x5f},		/* Backslash, '_' */
 { 0x85, 	0,	   	0   },		/* NoSymbol (Muhenkan) */
 { 0x86, 	0,   		0   },		/* NoSymbol (Henken)   */
 { 0x87, 	0,   		0   },		/* NoSymbol (Hiragana) */
 { 0xff, 	0,		0   }		/* terminator */
};

/**********************************************************************
 *  LK411 LK411 LK411 LK411 LK411 LK411 LK411 LK411 LK411 LK411 LK411 *
 **********************************************************************/
/*
 * IMPORTANT NOTE:  0x08 and 0x0e values *MUST* be redefined for all
 *                  LK411 keyboards, no matter what the language is.
 */
struct xlate_t lk411_danish_diffs[]=
{
/* keycode	unshifted	shifted */	/*language 0x30*/
 { 0x08, 	0x7e, 		0xa7},		/* '~', section'§' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0xa3},		/* '3', sterling'£' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x55,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x54,	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x5b,	0xa8,		0x5e},		/* twoupperdots, '^' */
 { 0x4c,	0xe6,		0xc6},		/* 'æ',  'Æ'	*/
 { 0x52,	0xf8,		0xd8},		/* oslash'ø', Ooblique'Ø' */
 { 0x5c,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_austrian_german_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x32*/
 { 0x08,	0x5e,		0xb0},		/* '^', degree'°' First row */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0xa7},		/* '3', section'§' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0xdf,		0x3f},		/* ssharp'ß', '?' */
 { 0x55,	0x27,		0x60},		/* '\'', '`'	*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x54,	0xfc,		0xdc},		/* udiaeresis'ü', Udiaeresis'Ü' */
 { 0x5b,	0x2b,		0x2a},		/* '+', '*'	*/
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö'*/
 { 0x52,	0xe4,		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä' */
 { 0x5c,	0x23,		0x27},		/* '#', '\''	*/
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_swiss_german_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x34*/
 { 0x08, 	0x21, 		0xb0},		/* '!', degree'°' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0x31,		0x2b},		/* '1', '+'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0x2a},		/* '3', '*'	*/
 { 0x25,	0x34,		0xe7},		/* '4', ccedilla'ç' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x27,	      	0x3f},		/* '\'', '?'	 */
 { 0x55,	0x5e,	   	0x60},		/* '^', '`'	*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x54,	0xfc,		0xe8},		/* udiaeresis'ü', egrave'è' */
 { 0x5b,	0xa8,		0x7e},		/* twoupperdots, '~' */
 { 0x4c,	0xf6,		0xe9},		/* odiaeresis'ö', eacute'é' */
 { 0x52,	0xe4,		0xe0},		/* adiaeresis'ä', agrave'à' */
 { 0x5c,	0x24,		0xa3},		/* '$', sterling'£' */
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_us_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x36*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x13, 	0x2d,   	0x2d},		/* '-', '-'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_uk_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x38*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x26,	0x33,		0xa3},		/* '3', 'sterling'£' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_spanish_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x3a*/
 { 0x08,	0xbf,		0xa1},		/* qstndown'¿', exclamdown'¡' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x27,		0x3f},		/* '\'', '?'	*/
 { 0x55,        0xba,		0xaa},		/* 'º', 'ª'	*/
 { 0x54,	0x60,		0x5e},		/* '`', '^'	*/
 { 0x5b,	0x2b,		0x2a},		/* '+', '*'	*/
 { 0x4c,	0xf1,		0xd1},		/* ntilde'ñ', Ntilde'Ñ' */
 { 0x5c,	0xe7,		0x7e},		/* ccedilla'ç', '~' */
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_french_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x3c*/
 { 0x08,	0x60,		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x15,	0x61,		0x41},		/* 'a', 'A'	*/
 { 0x16,	0x26,		0x31},		/* '&', '1'	*/
 { 0x1a,	0x77, 		0x57},		/* 'w', 'W'	*/
 { 0x1c,	0x71,		0x51},		/* 'q', 'Q'	*/
 { 0x1d,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x1e,	0xe9,		0x32},		/* eacute'é', '2' */
 { 0x25,	0x27,		0x34},		/* '\'', '4'	*/
 { 0x26,	0x22,		0x33},		/* '\"', '3'	*/
 { 0x2e,	0x28,		0x35},		/* '(', '5'	*/
 { 0x36,	0xa7,		0x36},		/* section'§', '6' */
 { 0x3a,	0x2c,		0x3f},		/* ',', '?'	*/
 { 0x3d,	0xe8,		0x37},		/* egrave'è', '7' */
 { 0x3e,	0x21,		0x38},		/* '!', '8'	*/
 { 0x41,	0x3b,		0x2e},		/* ';', '.'	*/
 { 0x45,	0xe0,		0x30},		/* agrave'à', '0' */
 { 0x46,	0xe7,		0x39},		/* ccedilla'ç', '9' */
 { 0x49,	0x3a,		0x2f},		/* ':', Slash	*/
 { 0x4a,	0x3d,		0x2b},   	/* '=', '+'	*/
 { 0x4c,	0x6d,		0x4d},		/* 'm', 'M'	*/
 { 0x4e,	0x29,		0xb0},		/* ')', degree'°' */
 { 0x52,	0xf9,		0x25},		/* ugrave'ù', '%' */
 { 0x54,	0x5e,		0xa8},		/* '^', twoupperdots */
 { 0x55,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x5b,	0x24,		0x2a},		/* '$', '*'	*/
 { 0x5c,	0x23,		0x40},		/* '#', '@'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_french_canadian_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x3e*/
 { 0x08, 	0x7e, 		0xb0},		/* '~', degree'°' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0x2f},		/* '3', Slash	*/
 { 0x36,	0x36,		0x3f},		/* '6', '?'	*/
 { 0x54,	0xe7,		0xc7},		/* ccedilla'ç', Ccedilla'Ç' */
 { 0x5b,	0x23,		0x40},		/* '#', '@'	*/
 { 0x52,	0x60,		0x5e},		/* '`', '^'	*/
 { 0x41,	0x2c,		0x27},		/* ',', '\''	*/
 { 0x49,	0x2e,		0x2e},		/* '.', '.'	*/
 { 0x4a,	0xe9,		0xc9},		/* eacute'é', Eacute'É' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_swiss_french_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x40*/
 { 0x08, 	0x21, 		0xb0},		/* '!', degree'°' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0x31,		0x2b},		/* '1', '+'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0x2a},		/* '3', '*'	*/
 { 0x25,	0x34,		0xe7},		/* '4', ccedilla'ç' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x27,		0x3f},		/* '\'',  '?'	*/
 { 0x55,	0x5e, 		0x60},		/* '^', '`'	*/
 { 0x35,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x54,	0xe8,		0xfc},		/* egrave'è' udiaeresis'ü' */
 { 0x5b,	0xa8,		0x7e},		/* twoupperdots, '~' */
 { 0x4c,	0xe9,		0xf6},		/* eacute'é', odiaeresis'ö' */
 { 0x52,	0xe0,		0xe4},		/* agrave'à', adiaeresis'ä' */
 { 0x5c,	0x24,		0xa3},		/* '$', sterling'£' */
 { 0x1a,	0x79,		0x59},		/* 'y', 'Y'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_italian_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x42*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0xa3,		0x31},		/* sterling'£', '1' */
 { 0x1e,	0xe9,		0x32},		/* eacute'é', '2' */
 { 0x26,	0x22,		0x33},		/* '\"', '3'	*/
 { 0x25,	0x27,		0x34},		/* '\'', '4'	*/
 { 0x2e,	0x28,		0x35},		/* '(', '5'	*/
 { 0x36,	0x5f,		0x36},		/* '_', '6'	*/
 { 0x3d,	0xe8,		0x37},		/* egrave'è', '7' */
 { 0x3e,	0x5e,		0x38},		/* '^',  '8'	*/
 { 0x46,	0xe7,		0x39},		/* ccedilla'ç', '9' */
 { 0x45,	0xe0,		0x30},		/* agrave'à', '0' */
 { 0x4e,	0x29,		0xb0},		/* ')', degree'°' */
 { 0x55,	0x2d,		0x2b},		/* '-', '+'	*/
 { 0x1d,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x54,	0xec,		0x3d},		/* igrave'ì', '=' */
 { 0x5b,	0x24,		0x26},		/* '$', '&'	*/
 { 0x4c,	0x6d,		0x4d},		/* 'm', 'M'	*/
 { 0x52,	0xf9,		0x25},		/* ugrave'ù', '%' */
 { 0x5c,	0x2a,		0xa7},		/* '*', section'§' */
 { 0x1a,	0x77,		0x57},		/* 'w', 'W'	*/
 { 0x3a,	0x2c,		0x3f},		/* ',', '?'	*/
 { 0x41,	0x3b,		0x2e},		/* ';', '.'	*/
 { 0x49,	0x3a,		0x2f},		/* ':', Slash	*/
 { 0x4a,	0xf2,		0x21},		/* ograve'ò', '!' */
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_dutch_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x44*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_norwegian_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x46*/
 { 0x08, 	0x7e, 		0x40},		/* '~', '@'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x55,	0xb4,		0x60},		/* acute, '`'	*/
 { 0x54,	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x5b,	0xa8,		0x5e},		/* twoupperdots, '^' */
 { 0x4c,	0xf8,		0xd8},		/* oslash'ø', Ooblique'Ø' */
 { 0x52,	0xe6,		0xc6},		/* 'æ', 'Æ'	*/
 { 0x5c,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_portuguese_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x48*/
 { 0x08, 	0x5c, 		0x7c},		/* Backslash, '|' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x36,	0x36,		0x22},		/* '6', '\"'	*/
 { 0x54,	0x27,		0x60},		/* '\'', '`'	*/
 { 0x4c,	0xe7, 		0xc7},		/* ccedilla'ç', Ccedilla'Ç' */
 { 0x52,	0x7e,		0x5e},		/* '~', '^'	*/
 { 0x5c,	0x5b,		0x7b},		/* '[', '{'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_finnish_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x4a*/
 { 0x08,	0x7e,		0x40},		/* '~', '@' First row */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', '\"'	*/
 { 0x26,	0x33,		0xa7},		/* '3', section'§' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x55,	0x5e,		0x60},		/* '^', '`'	*/
 { 0x54,	0xe5,		0xc5},		/* aring'å', Aring'Å' 2nd Row */
 { 0x5b,	0xfc,		0xdc},		/* udiaeresis'ü', Udiaeresis'Ü' */
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö'*/
 { 0x52,	0xe4,		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä'*/
 { 0x5c,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';' 4th Row */
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_swedish_diffs[] =
{
/* keycode	unshifted	shifted */	/*language 0x4c*/
 { 0x08, 	0x7e, 		0xb0},		/* '~', degree'°' */
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x1e,	0x32,		0x22},		/* '2', quotedbl'\"' */
 { 0x26,	0x33,		0xa7},		/* '3', section'§' */
 { 0x36,	0x36,		0x26},		/* '6', '&'	*/
 { 0x3d,	0x37,	        0x2f},		/* '7', Slash	*/
 { 0x3e,	0x38,		0x28},		/* '8', '('	*/
 { 0x46,	0x39,		0x29},		/* '9', ')'	*/
 { 0x45,	0x30,		0x3d},		/* '0', '='	*/
 { 0x4e,	0x2b,		0x3f},		/* '+', '?'	*/
 { 0x55,	0x5e,		0x60},		/* '^', '`'	*/
 { 0x54,	0xe5,		0xc5},		/* aring'å', Aring'Å' */
 { 0x5b,	0xfc,		0xdc},		/* udiaeresis'ü', Udiaeresis'Ü' */
 { 0x4c,	0xf6,		0xd6},		/* odiaeresis'ö', Odiaeresis'Ö' */
 { 0x52,	0xe4,		0xc4},		/* adiaeresis'ä', Adiaeresis'Ä' */
 { 0x5c,	0x27,		0x2a},		/* '\'', '*'	*/
 { 0x41,	0x2c, 		0x3b},		/* ',', ';'	*/
 { 0x49,	0x2e,		0x3a},		/* '.', ':'	*/
 { 0x4a,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};

struct xlate_t lk411_belgian_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4e*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x16,	0x26,		0x31},		/* '&', '1'	*/
 { 0x1e,	0xe9,		0x32},		/* eacute'é', '2' */
 { 0x26,	0x22,		0x33},		/* '\"', '3'	*/
 { 0x25,	0x27,		0x34},		/* '\'', '4'	*/
 { 0x2e,	0x28,		0x35},		/* '(', '5'	*/
 { 0x36,	0xa7,		0x36},		/* section'§', '6' */
 { 0x3d,	0xe8,		0x37},		/* egrave'è', '7' */
 { 0x3e,	0x21,		0x38},		/* '!', '8'	*/
 { 0x46,	0xe7,		0x39},		/* ccedilla'ç', '9' */
 { 0x45,	0xe0, 		0x30},		/* agrave'à', '0' */
 { 0x4e,	0x29,		0xb0},		/* ')', degree'°' */
 { 0x55,	0x2d,		0x5f},		/* '-', '_'	*/
 { 0x15,       	0x61,		0x41},		/* 'a', 'A'	*/
 { 0x1d,	0x7a,		0x5a},		/* 'z', 'Z'	*/
 { 0x54,	0x5e,		0xa8},		/* '^', twoupperdots */
 { 0x5b,	0x24,		0x2a},		/* '$', '*'	*/
 { 0x1c,	0x71, 		0x51},		/* 'q', 'Q'	*/
 { 0x4c,	0x6d,		0x4d},		/* 'm', 'M'	*/
 { 0x52,	0xf9,		0x25},		/* ugrave'ù', '%' */
 { 0x5c,	0x23,		0x40},		/* '#', '@'	*/
 { 0x1a,	0x77,		0x57},		/* 'w', 'W'	*/
 { 0x3a,	0x2c,		0x3f},		/* ',', '?'	*/
 { 0x41,	0x3b,		0x2e},		/* ';', '.'	*/
 { 0x49,	0x3a, 		0x2f},		/* ':', Slash	*/
 { 0x4a,	0x3d,		0x2b},		/* '=', '+'	*/
 { 0xff, 	0, 		0   }		/* terminator */
};


/* janpanese jis */
struct xlate_t lk411jj_japanese_diffs[] =
{
/* keycode	unshifted	shifted		language 0x50	*/
 { 0x08,	0x1b,		0x1b},		/* Escape 	*/
 { 0x1e,	0x32,		0x22},		/* '2',	 '\"'  	*/
 { 0x36,	0x36,		0x26},		/* '6',	 '&'	*/
 { 0x3d,	0x37,	 	0x27},		/* '7',  '\''	*/
 { 0x3e,	0x38,		0x28},		/* '8',	 '('	*/
 { 0x45,	0x30,		0x30},		/* '0',	 '0'	*/
 { 0x46,	0x39,	 	0x29},		/* '9',  ')'	*/
 { 0x4c,	0x3b,		0x2b},		/* ';',  '+'	*/
 { 0x4e,	0x2d,		0x3d},		/* '-',  '='	*/
 { 0x51,	0, 		0x5f},		/* NoSymbol, '_' */
 { 0x52,	0x3a,		0x2a},		/* ':',  '*'	*/
 { 0x53,	0x5d,		0x7d},		/* ']',  '}'	*/
 { 0x54,	0x40,		0x60},		/* '@',  '`'	*/
 { 0x55,	0x5e,		0x7e},		/* '^',  '\~'	*/
 { 0x5b,	0x5b,		0x7b},		/* '[',  '{'	*/
 { 0x5d,	0x5c,		0x7c},		/* '\',  '|'	*/
 { 0x85, 	0,	   	0   },		/* NoSymbol (Muhenkan) */
 { 0x86, 	0,   		0   },		/* NoSymbol (Henken)   */
 { 0x87, 	0,   		0   },		/* NoSymbol (Hiragana) */
 { 0xff,	0,		0,  }		/* terminator	*/
};

/* janpanese ansi */
struct xlate_t lk411aj_japanese_diffs[] =
{
/* keycode	unshifted	shifted		language 0x52	*/
 { 0x08, 	0x60, 		0x7e},		/* '`', '~'	*/
 { 0x0e,	0x3c,		0x3e},		/* '<', '>'	*/
 { 0x13, 	0x2d,   	0x2d},		/* '-', '-'	*/
 { 0x85, 	0,	   	0   },		/* NoSymbol (Muhenkan) */
 { 0x86, 	0,   		0   },		/* NoSymbol (Henken)   */
 { 0x87, 	0,   		0   },		/* NoSymbol (Hiragana) */
 { 0xff, 	0, 		0   }		/* terminator */
};


/*
 *Presently, only the french diff table has been modified for the
 *LK411.  All others are set for the PCXAL.  As tables get added to
 *map correctly, make sure you change the preface to LK411 instead of
 *PCXAL.  For example, after you create an lk411_danish_diffs table,
 *change pcxal_danish_diffs in the LK411 section of the following table
 *to lk411_danish_diffs so that it points to the new table.
 *
 *It's conceivable that many of these tables will never
 *get updated for the particular keyboard, due to lack of:  sales for a
 *particular language, customer complaints, a desire for pain.
 */

struct xlate_t *(xlate_diffs[][NUMBER_OF_LANGUAGES]) =
{
   { /*This is for PCXAL*/
	pcxal_danish_diffs,			/* 0x30 */
	pcxal_austrian_german_diffs,		/* 0x32 */
	pcxal_swiss_german_diffs,		/* 0x34 */
	pcxal_us_diffs,			        /* 0x36 *//* US is default */
	pcxal_uk_diffs,		         	/* 0x38 */
	pcxal_spanish_diffs,			/* 0x3a */
	pcxal_french_diffs,		        /* 0x3c */
	pcxal_french_canadian_diffs,		/* 0x3e */
	pcxal_swiss_french_diffs,		/* 0x40 */
	pcxal_italian_diffs,			/* 0x42 */
	pcxal_dutch_diffs,			/* 0x44 */
	pcxal_norwegian_diffs,	         	/* 0x46 */
	pcxal_portuguese_diffs, 		/* 0x48 */
	pcxal_finnish_diffs,			/* 0x4a */
	pcxal_swedish_diffs,	  		/* 0x4c */
	pcxal_belgian_diffs,			/* 0x4e */
	pcxaj_aj_japanese_diffs,		/* 0x50 */
	pcxaj_aj_japanese_diffs			/* 0x52 used line up with lk*/
   },

   { /*This if for LK411*/
	lk411_danish_diffs,			/* 0x30 */
	lk411_austrian_german_diffs,		/* 0x32 */
   	lk411_swiss_german_diffs,		/* 0x34 */
	lk411_us_diffs,  		        /* 0x36 */
	lk411_uk_diffs,		         	/* 0x38 */
	lk411_spanish_diffs,			/* 0x3a */
	lk411_french_diffs,		        /* 0x3c */
	lk411_french_canadian_diffs,		/* 0x3e */
	lk411_swiss_french_diffs,		/* 0x40 */
	lk411_italian_diffs,			/* 0x42 */
	lk411_dutch_diffs,			/* 0x44 */
	lk411_norwegian_diffs,	         	/* 0x46 */
	lk411_portuguese_diffs, 		/* 0x48 */
	lk411_finnish_diffs,			/* 0x4a */
	lk411_swedish_diffs,			/* 0x4c */
	lk411_belgian_diffs,			/* 0x4e */
	lk411jj_japanese_diffs,			/* 0x50 */
	lk411aj_japanese_diffs			/* 0x52 */
   }
};

/*
 * The following supports ALT selections.  This is being implemented to
 * satisfy french problems, but can be extended to all of the languages.
 * Only the french tables are filled.  As support for other languages is
 * completed, those particular tables will be filled in too.  Right now
 * the US table, which is null, is being used for everything but the french.
 */

/*
 * pcxal   pcxal   pcxal   pcxal   pcxal   pcxal   pcxal   pcxal   pcxal
 */
struct alt_diffs pcxal_danish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x30*/
   { 0x1e,	0x40,	       	0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0x24,		0 },		/* '$', NoSymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x55,	0x7c,		0 },		/* '|', NoSymbol */
   { 0x5b,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x13,	0x5c,		0 },		/* Backslash, NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_austrian_german_alt_diffs [] =
{
/* keycode     	unshifted	shifted */	/*language 0x32*/
   { 0x1e, 	0xb2,		0 },		/* '²', NoSymbol */
   { 0x26, 	0xb3,		0 },		/* '³', NoSymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x4e, 	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x15, 	0x40,		0 },		/* '@', NoSymbol */
   { 0x5b, 	0x7e,		0 },		/* '~', NoSymbol */
   { 0x13, 	0x7c,		0 },		/* '|', NoSymbol */
   { 0x3a, 	0xb5,		0 },		/* 'µ', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
} ;

struct alt_diffs pcxal_swiss_german_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x34*/
   { 0x16,	0x7c,		0 },		/* '|', NoSymbol */
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0x23,		0 },		/* '#', NoSymbol */
   { 0x36,	0xac,		0 },		/*    , NoSymbol */
   { 0x3d,	0xa6,		0 },		/*    , NoSymbol */
   { 0x3e,	0xa2,		0 },		/* '¢', NoSymbol */
   { 0x4e,	0xb4,		0 },		/*    , NoSymbol */
   { 0x55,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x52,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x53,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x13,	0x5c,		0 },		/* Backslash, NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};


struct alt_diffs pcxal_us_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x36*/
   { 0,  	0,		0 } 		/* Terminate */
} ;

struct alt_diffs pcxal_uk_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x38*/
   { 0x0e,	0xa6,		0 },		/*    , NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_spanish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3a*/
   { 0x0e,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x16,	0x7c,		0 },		/* '|', NoSymbol */
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0x23,		0 },		/* '#', NoSymbol */
   { 0x36,	0xac,		0 },		/* '#', NoSymbol */
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x52,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x53,	0x7d,		0 },		/* '}', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_french_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3c */
   { 0x1e, 	0x7e,		0 },		/* '~', NoSymbol */
   { 0x26, 	0x23,		0 },		/* '#', NoSymbol */
   { 0x25, 	0x7b,		0 },		/* '{', NoSymbol */
   { 0x2e, 	0x5b,		0 },		/* '[', NoSymbol */
   { 0x36, 	0x7c,		0 },		/* '|', NoSymbol */
   { 0x3d, 	0x60,		0 },		/* '`', NoSymbol */
   { 0x3e, 	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x46, 	0x5e,		0 },		/* '^', NoSymbol */
   { 0x45, 	0x40,		0 },		/* '@', NoSymbol */
   { 0x4e, 	0x5d,		0 },		/* ']', NoSymbol */
   { 0x55,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x5b, 	0xa4,		0 },		/*    , NoSymbol */
   { 0,  	0,		0 } 		/* Terminate */
} ;

struct alt_diffs pcxal_french_canadian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3e*/
   { 0x0e,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x16,	0xb1,		0 },		/* plusminus'±', NoSymbol */
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0xa2,		0 },		/* '¢', NoSymbol */
   { 0x2e,	0xa4,		0 },		/*    , NoSymbol */
   { 0x36,	0xac,		0 },		/*    , NoSymbol */
   { 0x3d,	0xa6,		0 },		/*    , NoSymbol */
   { 0x3e,	0xb2,		0 },		/* '²', NoSymbol */
   { 0x46,	0xb3,		0 },		/* '³', NoSymbol */
   { 0x45,	0xbc,		0 },		/* '¼', NoSymbol */
   { 0x4e,	0xbd,		0 },		/* '½', NoSymbol */
   { 0x55,	0xbe,		0 },		/* 3/4, NoSymbol */
   { 0x44,	0xa7,		0 },		/* section'§', NoSymbol */
   { 0x4d,	0xb6,		0 },		/* '¶', NoSymbol */
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x4c,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x52,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x53,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x13,	0xb0,		0 },		/* degree'°', NoSymbol */
   { 0x3a,	0xb5,		0 },		/* 'µ',  NoSymbol */
   { 0x41,	0xaf,		0 },		/*    , NoSymbol */
   { 0x4a,	0xb4,		0 },		/*    , NoSymbol */
   {    0,	0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_swiss_french_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x40*/
   { 0x16,	0x7c,		0 },		/* '|', NoSymbol */
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0x23,		0 },		/* '#', NoSymbol */
   { 0x36,	0xac,		0 },		/*    , NoSymbol */
   { 0x3d,	0xa6,		0 },		/*    , NoSymbol */
   { 0x3e,	0xa2,		0 },		/* '¢', NoSymbol  */
   { 0x4e,	0xb4,		0 },		/*    , NoSymbol */
   { 0x55,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x52,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x53,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x13,	0x5c,		0 },		/* Backslash, NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_italian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x42*/
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x4c,	0x40,		0 },		/* '@', NoSymbol */
   { 0x52,	0x23,		0 },		/* '#', NoSymbol */
   {    0,	0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_dutch_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x44*/
   { 0x0e,	0xac,		0 },		/*    , NoSymbol */
   { 0x16,	0xb9,		0 },		/* '¹', NoSymbol */
   { 0x1e,	0xb2,		0 },		/* '²', NoSymbol */
   { 0x26,	0xb3,		0 },		/* '³', NoSymbol */
   { 0x25,	0xbc,		0 },		/* '¼', NoSymbol */
   { 0x2e,	0xbd,		0 },		/* '½', NoSymbol */
   { 0x36,	0xbe,		0 },		/* 3/4, NoSymbol */
   { 0x3d,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x3e,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x46,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x4e,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x55,	0xb8,		0 },		/* cedilla, NoSymbol */
   { 0x2d,	0xb6,		0 },		/* '¶', NoSymbol */
   { 0x1b,	0xdf,		0 },		/* ssharp'ß', NoSymbol  */
   { 0x13,	0xa6,		0 },		/*    , NoSymbol */
   { 0x1a,	0xab,		0 },		/* guillemotleft'«', NoSymbol */
   { 0x22,	0xbb,		0 },		/* guillemotright'»', NoSymbol*/
   { 0x21,	0xa2,		0 },		/* '¢', NoSymbol  */
   { 0x3a,	0xb5,		0 },		/* 'µ',  NoSymbol  */
   { 0x49,	0xb7,		0 },		/* '·', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_norwegian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x46*/
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0x24,		0 },		/* '$', NoSymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x55,	0xb4,		0 },		/*    , NoSymbol */
   { 0x5b,	0x7e,		0 },		/* '~', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_portuguese_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x48*/
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0xa7,		0 },		/* section'§', Nosymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x54,	0xa8,		0 },		/* twoupperdots, NoSymbol */
   {    0,      0,		0 }		/* Terminate */
};

struct alt_diffs pcxal_finnish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4a*/
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0x24,		0 },		/* '$', NoSymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x4e,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x5b,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x13,	0x7c,		0 },		/* '|', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_swedish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4c*/
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0xa3,		0 },		/* sterling'£', NoSymbol */
   { 0x25,	0x24,		0 },		/* '$', NoSymbol */
   { 0x3d,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x3e,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x46,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x4e,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x5b,	0x7e,		0 },		/* '~', NoSymbol */
   { 0x13,	0x7c,		0 },		/* '|', NoSymbol */
   {    0,      0,		0 } 		/* Terminate */
};

struct alt_diffs pcxal_belgian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4e*/
   { 0x16,	0x7c,		0 },		/* '|', NoSymbol */
   { 0x1e,	0x40,		0 },		/* '@', NoSymbol */
   { 0x26,	0x23,		0 },		/* '#', NoSymbol */
   { 0x36,	0x5e,		0 },		/* '^', NoSymbol */
   { 0x46,	0x7b,		0 },		/* '{', NoSymbol */
   { 0x45,	0x7d,		0 },		/* '}', NoSymbol */
   { 0x54,	0x5b,		0 },		/* '[', NoSymbol */
   { 0x5b,	0x5d,		0 },		/* ']', NoSymbol */
   { 0x52,	0xb4,		0 },		/*    , NoSymbol */
   { 0x53,	0x60,		0 },		/* '`', NoSymbol */
   { 0x13,	0x5c,		0 },		/* Backslash, NoSymbol */
   { 0x4a,	0x7e,		0 },		/* '~', NoSymbol */
   {    0,      0,		0 }		/* Terminate */
};

struct alt_diffs pcxaj_aj_japanese_alt_diffs[] =
{
/*  keycode	 unshifted	shifted		language 0x50 */
    {    0,      0,		0 }		/* Terminate */
};


/*
 * LK411  LK411  LK411  LK411  LK411  LK411  LK411  LK411  LK411  LK411  LK411
 */

struct alt_diffs lk411_danish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x30*/
   { 0x08,	0x40,		0   },		/* '@', NoSymbol */
   { 0x26,	0x23,		0   },		/* '#', NoSymbol */
   { 0x54,	0x7d,		0x5d},		/* '}', ']'	*/
   { 0x4c,	0x7b,		0x5b},		/* '{', '['	*/
   { 0x52,	0x7c,		0x5c},		/* '|', Backslash */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_austrian_german_alt_diffs [] =
{
/* keycode     	unshifted	shifted */	/*language 0x32*/
   { 0x08,	0xac,		0   },		/*    , NoSymbol */
   { 0x16, 	0xb9,		0xa1},		/* '¹', '¡' First row */
   { 0x1e, 	0xb2,		0   },		/* '²', NoSymbol */
   { 0x26, 	0xb3,		0xa3},		/* '³', sterling'£' */
   { 0x25, 	0xbc,		0xa4},		/* '¼',  '¨'	*/
   { 0x2e,	0xbd,		0   },		/* '½', NoSymbol */
   { 0x36,	0xbe,		0   },		/* 3/4, NoSymbol */
   { 0x3d, 	0x7b,		0   },		/* '{', NoSymbol */
   { 0x3e, 	0x5b,		0   },		/* '[', NoSymbol */
   { 0x46, 	0x5d,		0xb1},		/* ']', plusminus'±' */
   { 0x45, 	0x7d,		0xb0},		/* '}', degree'°' */
   { 0x4e, 	0x5c,		0xbf},		/* Backslash, questiondown'¿' */
   { 0x55, 	0xb8,		0   },		/*    , NoSymbol */
   { 0x15, 	0x40,		0   },		/* '@', NoSymbol */
   { 0x2d, 	0xb6,		0xae},		/* '¶', circleR */
   { 0x35, 	0xa5,		0   },		/* '¥', NoSymbol */
   { 0x44, 	0xf8,		0   },		/* oslash'ø', NoSymbol */
   { 0x4d, 	0xfe,		0   },		/*    , NoSymbol */
   { 0x54, 	0xa8,		0xb0},		/* twoupperdots, degree'°' */
   { 0x5b, 	0x7e,		0xaf},		/* '~',  	*/
   { 0x1c, 	0xc6,		0   },		/* 'Æ', NoSymbol */
   { 0x1b, 	0xdf,		0xa7},		/* ssharp'ß', section'§' */
   { 0x23, 	0xf0,		0xd0},		/* o with x over, MinusD */
   { 0x2b, 	0xaa,		0   },		/* 'ª', NoSymbol */
   { 0x42, 	0x26,		0   },		/* '&', NoSymbol */
   { 0x4c, 	0xb4,		0   },		/*    , NoSymbol */
   { 0x52, 	0x5e,		0   },		/* '^', NoSymbol */
   { 0x5c, 	0x60,		0   },		/* '`', NoSymbol */
   { 0x0e, 	0x7c,		0x7c},		/* '|', '|'	*/
   { 0x1a, 	0xab,		0x3c},		/* guillemotleft'«', '<' */
   { 0x22, 	0xbb,		0x3e},		/* guillemotright'»', '>' */
   { 0x21, 	0xa2,		0xa9},		/* '¢', '©'	*/
   { 0x3a, 	0xb5,		0xba},		/* 'µ', 'º'	*/
   { 0x41, 	0xd7,		0   },		/* '×', NoSymbol */
   { 0x49, 	0xb7,		0xf7},		/* '·', '÷'	*/
   {    0,      0,		0   }		 /* Terminate */
} ;

struct alt_diffs lk411_swiss_german_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x34*/
   { 0x25,	0x40,		0   },		/* '@', NoSymbol */
   { 0x54,	0x7c,		0x5c},		/* '|', Backslash */
   { 0x4c,	0x7b,		0x5b},		/* '{', '['	*/
   { 0x52,	0x7d,		0x5d},		/* '}', ']'	*/
   { 0x5c,	0x23,		0   },		/* '#', NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};


struct alt_diffs lk411_us_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x36*/
   { 0,  	0,		0   }		/* Terminate */
} ;

struct alt_diffs lk411_uk_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x38*/
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_spanish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3a*/
   { 0x08,	0x40,		0x5c},		/* '@', Backslash */
   { 0x55,	0x5d,		0x5b},		/* ']', '['	*/
   { 0x4c,	0x7b,		0x7d},		/* '{', '}'	*/
   { 0x5c,	0x7c,		0   },		/* '|', NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_french_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3c */
   { 0x1e, 	0x7c,		0   },		/* '|', NoSymbol */
   { 0x36, 	0x5b,		0   },		/* '[', NoSymbol */
   { 0x3d, 	0x5d,		0   },		/* ']', NoSymbol */
   { 0x46, 	0x7b,		0   },		/* '{', NoSymbol */
   { 0x45,	0x7d,		0   },		/* '}', NoSymbol */
   { 0x52, 	0x5c,		0   },		/* Backslash, NoSymbol */
   {    0,    	0,		0   }		/* Terminate */
} ;

struct alt_diffs lk411_french_canadian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x3e*/
   { 0x54,	0x5d,		0x5b},		/* ']', '['	*/
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_swiss_french_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x40*/
   { 0x25,	0x40,		0   },		/* '@', NoSymbol */
   { 0x54,	0x7c,		0x5c},		/* '|', Backslash */
   { 0x4c,	0x7b,		0x5b},		/* '{', '['	*/
   { 0x52,	0x7d,		0x5d},		/* '}', ']'	*/
   { 0x5c,	0x23,		0   },		/* '#', NoSymbol */
   {    0,      0,		0   } 		/* Terminate */
};

struct alt_diffs lk411_italian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x42*/
   { 0x16,	0x40,		0   },		/* '@', NoSymbol */
   { 0x1e,	0x23,		0   },		/* '#', NoSymbol */
   { 0x46,	0x5b,		0   },		/* '[', NoSymbol */
   { 0x45,	0x5d,		0   },		/* ']', NoSymbol */
   { 0x54,	0x7d,		0   },		/* '}', NoSymbol */
   { 0x52,	0x7b,		0   },		/* '{', NoSymbol */
   { 0x5c,	0x5c,		0   },		/* Backslash, NoSymbol */
   { 0x4a,	0x7c,		0   },		/* '|', NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_dutch_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x44*/
   { 0x26,	0xa3,		0   },		/* sterling'£', NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_norwegian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x46*/
   { 0x54,	0x7d,		0x5d}, 		/* '}', ']' 	*/
   { 0x4c,	0x7c,		0x5c},		/* '|', Backslash */
   { 0x52,	0x7b,		0x5b},		/* '{', '['	*/
   {    0,      0,		0   } 		/* Terminate */
};

struct alt_diffs lk411_portuguese_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x48*/
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_finnish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4a*/
   { 0x26,	0x23,		0   },		/* '#', NoSymbol */
   { 0x54,	0x7d,		0x5d},		/* '}', ']'	*/
   { 0x5b,	0x7e,		0x5e},		/* '~', '^'	*/
   { 0x4c,	0x7c,		0x5c},		/* '|', Backslash */
   { 0x52,	0x7b,		0x5b},		/* '{', '['	*/
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_swedish_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4c*/
   { 0x26,	0x40,		0   },		/* '@', NoSymbol */
   { 0x54,	0x5d,		0x5b},		/* ']', '['	*/
   { 0x5b,	0x23,		0x5c},		/* '#', Backslash */
   { 0x4c,	0x7d,		0x7b},		/* '}', '{'	*/
   { 0x52,	0x7c,		0   },		/* '|', NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411_belgian_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x4e*/
   { 0x1e,	0x7c,		0   },		/* '|', NoSymbol */
   { 0x36,	0x5b,		0   },		/* '[', NoSymbol */
   { 0x3d,	0x5d,		0   },		/* ']', NoSymbol */
   { 0x46,	0x7b,		0   },		/* '{', NoSymbol */
   { 0x45,	0x7d,		0   },		/* '}', NoSymbol */
   { 0x52,	0x5c,		0   },		/* Backslash, NoSymbol */
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411jj_japanese_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x50*/
   {    0,      0,		0   }		/* Terminate */
};

struct alt_diffs lk411aj_japanese_alt_diffs[] =
{
/* keycode     	unshifted	shifted */	/*language 0x52*/
   {    0,      0,		0   }		/* Terminate */
};


struct alt_diffs *alt_table[][NUMBER_OF_LANGUAGES] =
{
   {
	pcxal_danish_alt_diffs,			/*0x30 Dansk*/
	pcxal_austrian_german_alt_diffs,  	/*0x32 Deutsch (Deutshland/Osterreich) */
	pcxal_swiss_german_alt_diffs,		/*0x34 Deutsh (Schweiz) */
	pcxal_us_alt_diffs,			/*0x36 English (American) */
	pcxal_uk_alt_diffs,			/*0x38 English (British/Irish) */
	pcxal_spanish_alt_diffs,		/*0x3a Espanol */
	pcxal_french_alt_diffs,			/*0x3c Francais */
	pcxal_french_canadian_alt_diffs,	/*0x3e Francais (Canadian) */
	pcxal_swiss_french_alt_diffs,		/*0x40 Francais (Suisse Romande) */
	pcxal_italian_alt_diffs,		/*0x42 Italiano */
	pcxal_dutch_alt_diffs,			/*0x44 Nederlands */
	pcxal_norwegian_alt_diffs,		/*0x46 Norsk */
	pcxal_portuguese_alt_diffs,		/*0x48 Portugese */
	pcxal_finnish_alt_diffs,		/*0x4a Suomi */
	pcxal_swedish_alt_diffs,		/*0x4c Svenska */
	pcxal_belgian_alt_diffs,		/*0x4e Belgisch-Nederlands */
	pcxaj_aj_japanese_alt_diffs,		/*0x50 Japanese (PCXAJ-AA) */
	pcxaj_aj_japanese_alt_diffs		/*0x52 Japanese (PCXAl-AA) */
   },
   {
	lk411_danish_alt_diffs,			/*0x30*/
	lk411_austrian_german_alt_diffs,  	/*0x32*/
	lk411_swiss_german_alt_diffs,		/*0x34*/
	lk411_us_alt_diffs,			/*0x36*/
	lk411_uk_alt_diffs,			/*0x38*/
	lk411_spanish_alt_diffs,		/*0x3a*/
	lk411_french_alt_diffs,			/*0x3c*/
	lk411_french_canadian_alt_diffs,	/*0x3e*/
	lk411_swiss_french_alt_diffs,		/*0x40*/
	lk411_italian_alt_diffs,		/*0x42*/
	lk411_dutch_alt_diffs,			/*0x44*/
	lk411_norwegian_alt_diffs,		/*0x46*/
	lk411_portuguese_alt_diffs,		/*0x48*/
	lk411_finnish_alt_diffs,		/*0x4a*/
	lk411_swedish_alt_diffs,		/*0x4c*/
	lk411_belgian_alt_diffs,		/*0x4e*/
	lk411jj_japanese_alt_diffs,		/*0x50*/
	lk411aj_japanese_alt_diffs		/*0x52*/
   }
} ;

