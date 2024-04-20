/* kbd.h
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
 *	Translation tables definitions.
 *
 *  AUTHORS:
 *
 *	Judith Gold
 *
 *  CREATION DATE:
 *  
 *	02-May-1994	
 *
 *
 *  MODIFICATION HISTORY:
 *
 *	15-Oct-1996  wcc    Folded in changes. 
 *
 *	27-SEP-1996  rhd    added definition for SCRLCK, LCMPCHR and defined 
 *			    F2-F5 to a null char
 *
 *	27-jun-1996  rhd    added definition for Japanese language support.
 *			    (AJ = ANSI KBD, JJ = JIS KBD)
 *				#define LANG_K_JAPANESE_JJ		0x50
 *				#define LANG_K_JAPANESE_AJ		0x52
 *				#define NUMBER_OF_LANGUAGES               18 
 *
 *	31-may-1996   dm    Modified alt_diffs structure to provide for shifted
 *			    and unshifted values.  Added keypad_xlate structure
 *                          Added a few more key definitions: SLASH, PLUS
 *                          STAR, MINUS, MULTIPLY, and DIVIDE.
 *
 *      27-feb-1996  al     Major changed to get rid of conditionals, and
 *                          convert tables to characters, instead of strings.
 *                          Got rid of "kbdscan.h", "kbd_def.sdl", and 
 *                          "lang_def.sdl" by putting them in this file.
 *
 *      6-sep-1995   al      Add alt_diffs structure to support ALT keys.
 *
 *	13-Mar-1995 rbb	    Conditionalize for EB164
 *
 *	27-Dec-1994 er	    Conditionalize for EB66
 *
 *	10-Nov-1994 jrk	    Fix for Turbo
 *
 *	15-Aug-1994 er	    Conditionalize for SD164
 *
 *	23-Jun-1994 er	    Conditionalize for EB64+
 *
 *	9-Ma-1994   dtr	    Added in mustang and the rest of the workstations to
 *			    the conditionals
 *
 *--
 */

#define PCXAL 0
#define LK411 1

#define LOWERCASE 0
#define UPPERCASE 1

#define	FIND		((unsigned char*)"\233\061\176")/* CSI 1 ~ */
#define	SELECT		((unsigned char*)"\233\064\176")/* CSI 4 ~ */
#define	INSERT		((unsigned char*)"\233\100")	/* CSI @ */
#define	HOME		((unsigned char*)"\233\110")	/* CSI H */
#define	DELETE		((unsigned char*)"\233\120")	/* CSI P */
#define	PRTSC		((unsigned char*)"\210\213")
#define	PAUSE		((unsigned char*)"\210")
/* Don't have proper translation for the following: */
#define	PGUP		((unsigned char*)"\0") 
#define	PGDWN		((unsigned char*)"\0")
#define	END  		((unsigned char*)"\0")
#define	SCRLCK		((unsigned char*)"\0") 
#define	LCMPCHR		((unsigned char*)"\0") 

/* Keypad Keys */
#define SLASH		((unsigned char*)"/")
#define PLUS		((unsigned char*)"+")
#define STAR		((unsigned char*)"*")
#define MINUS		((unsigned char*)"-")
#define MULTIPLY	((unsigned char*)"\327")	/* "×" */
#define DIVIDE		((unsigned char*)"\367")	/* "÷" */

/* ARROW KEYS */
#define	UP		((unsigned char*)"\233\101")	/* CSI A */
#define	DOWN		((unsigned char*)"\233\102")	/* CSI B */
#define	RIGHT		((unsigned char*)"\233\103")	/* CSI C */
#define	LEFT		((unsigned char*)"\233\104")	/* CSI D */

#define PF1_SCANCODE 0x76
#define PF2_SCANCODE 0x77
#define PF3_SCANCODE 0x7e
#define PF4_SCANCODE 0x84
#define PF1   ((unsigned char*)"\033\117\120")  /* ESC O P */
#define PF2   ((unsigned char*)"\033\117\121")  /* ESC O Q */
#define PF3   ((unsigned char*)"\033\117\122")  /* ESC O R */
#define PF4   ((unsigned char*)"\033\117\123")  /* ESC O S */


/* Don't have proper translation for the following:	*/
#define	F2		((unsigned char*)"\0") 
#define	F3		((unsigned char*)"\0") 
#define	F4		((unsigned char*)"\0") 
#define	F5		((unsigned char*)"\0") 
/*							*/

#define	F6		((unsigned char*)"\23317~")	/* CSI 1 7 ~ */
#define	F7		((unsigned char*)"\23318~")	/* CSI 1 8 ~ */
#define	F8		((unsigned char*)"\23319~")	/* CSI 1 9 ~ */
#define	F9		((unsigned char*)"\23320~")	/* CSI 2 0 ~ */
#define	F10		((unsigned char*)"\23321~")	/* CSI 2 1 ~ */
#define	F11		((unsigned char*)"\23323~")	/* CSI 2 3 ~ */
#define	F12		((unsigned char*)"\23324~")	/* CSI 2 4 ~ */
#define	F13		((unsigned char*)"\23325~")	/* CSI 2 5 ~ */

#define	F14		((unsigned char*)"\23326~")	/* CSI 2 6 ~ */
#define	HELP		((unsigned char*)"\23328~")	/* CSI 2 8 ~ */
#define	DO		((unsigned char*)"\23329~")	/* CSI 2 9 ~ */
#define	F17		((unsigned char*)"\23331~")	/* CSI 3 1 ~ */
#define	F18		((unsigned char*)"\23332~")	/* CSI 3 2 ~ */
#define	F19		((unsigned char*)"\23333~")	/* CSI 3 3 ~ */
#define	F20		((unsigned char*)"\23334~")	/* CSI 3 4 ~ */


/* Define MAX_LENGTH_OF_MULTI_BYTE_SEQUENCES to handle the largest of the
 * above sequences, which is 5 characters plus null terminator.
 */
#define MAX_LENGTH_OF_MULTI_BYTE_SEQUENCES  6

#define F1		0x07  /*Used as scroll*/
#define CAPS		0x14
#define LSHIFT		0x12
#define RSHIFT		0x59
#define LCTRL		0x11
#define RCTRL		0x58
#define	LALT		0x19
#define	RALT		0x39
#define	NUMLOCK		0x76
#define XON             0X11
#define XOFF            0X13
#define S_SCANCODE      0X1b  
#define Q_SCANCODE      0X15  
#define MAX_CODE	0x85

/*
 * SRM defined language codes
 */
#define LANG_K_NONE                        0    
#define LANG_K_DANSK                    0x30 
#define LANG_K_DEUTSCH                  0x32 
#define LANG_K_DEUTSCH_SCHWEIZ          0x34 
#define LANG_K_ENGLISH_AMERICAN         0x36 
#define LANG_K_ENGLISH_BRITISH          0x38 
#define LANG_K_ESPANOL                  0x3a 
#define LANG_K_FRANCAIS	                0x3c 
#define LANG_K_FRANCAIS_CANADA	        0x3e 
#define LANG_K_FRANCAIS_SUISSE_ROMANDE  0x40 
#define LANG_K_ITALIANO	                0x42 
#define LANG_K_NEDERLANDS               0x44 
#define LANG_K_NORSK                    0x46 
#define LANG_K_PORTUGUES                0x48 
#define LANG_K_SUOMI                    0x4a 
#define LANG_K_SVENSKA                  0x4c 
#define LANG_K_VLAAMS                   0x4e 
/* Added japanese (AJ = ANSI KBD, JJ = JIS KBD) */
#define LANG_K_JAPANESE_JJ		0x50
#define LANG_K_JAPANESE_AJ		0x52
#define NUMBER_OF_LANGUAGES               18 

struct xlate_t
{
   unsigned char scancode;
   unsigned char unshifted;
   unsigned char shifted;
} ;

struct alt_diffs /*Exists to service ALT selections*/
{
   unsigned char scancode ;
   unsigned char unshifted ;
   unsigned char shifted ;
} ;

/*Exists to service all multi byte translations, such as escape sequences.*/
struct multi_byte_xlate 
{
   unsigned int scancode ;
   unsigned char *value ;
} ;

/* Exists to service the keypad translations */
struct keypad_xlate 
{
   unsigned int scancode ;
   unsigned char *value ;
   unsigned int numlock ;
} ;


/* PHOENIX KEYBOARD CONTROLLER DEFINITIONS*/

#define	KBD_DATA 0x60
#define	KBD_STAT 0x64
#define	KBD_CMD  0x64

#define	STAT$M_OBF      1  /* output buffer full */
#define	STAT$M_IBF      2  /* input buffer full */
#define	STAT$M_SYS      4  /* system flag */
#define	STAT$M_CD       8  /* command/data */
#define	STAT$M_KBEN    16  /* keyboard enable */
#define	STAT$M_ODS     32  /* output buffer source */
#define	STAT$M_GTO     64  /* general time-out */
#define	STAT$M_PERR   128  /* receive parity error */

#define	CTL_RDMODE  0x20
#define	CTL_WRMODE  0x60
#define	CTL_TEST1   0xAA
#define	CTL_TEST2   0xAB
#define	CTL_ENABLE  0xAE
#define	CTL_RDOUT   0xD0
#define	CTL_WROUT   0xD1
#define	CTL_WRMOUSE 0xD4
#define	CTL_RESET   0xff

#define KBD_INT_ENABLE		0x02	/* 0 = enable */

/* status register bits */
#define	KBD_OBF		0x01
#define	KBD_IBF		0x02
#define	KBD_KBEN	0x10
#define	KBD_ODS		0x20
#define	KBD_GTO		0x40
#define	KBD_PERR	0x80

/* mode register bits */
#define KBD_EKI		0x01
#define KBD_SYS		0x04
#define KBD_EMI		0x02
#define KBD_DKB		0x10
#define KBD_DMS		0x20
#define KBD_KCC		0x40

/* kbd */
#define	KBD_RESET	0xFF
#define	KBD_MAKEBREAK	0xFC
#define	KBD_MAKE	0xFD
#define	KBD_SETLEDS	0xED
#define	KBD_SELECTCODE	0xF0
#define	KBD_ENABLE	0xF4
#define	KBD_ACK		0xFA
#define	KBD_RESEND	0xFE
#define	KBD_DEFAULTS	0xF6

/* rtc */
#if APC_PLATFORM
#define	RTCA		0x070
#define	RTCD		0x071
#else
#define	RTCA		0x0170
#define	RTCD		0x0171
#endif
#define	RTCCR1		0x6A
#define	ATKBD		0x02

/* make/break - mode 3 */
#define LSHIFT_MAKE  0x2A
#define LSHIFT_BREAK 0xAA
#define RSHIFT_MAKE  0x36
#define RSHIFT_BREAK 0xB6
#define LCTRL_MAKE   0x38
#define LCTRL_BREAK  0xB8
#define RCTRL_MAKE   0x3A
#define RCTRL_BREAK  0xBA
#define LALT_MAKE    0x71
#define LALT_BREAK   0xF1
#define RALT_MAKE    0x72
#define RALT_BREAK   0xF2
