/* file:	ansi.c
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
 * 		This module implements an ANSI escape sequence parser
 *		for the console.  It reads in characters from STDIN, 
 *		and returns when a token is found.  A token
 *		may simply be a character, or an entire control sequence.
 *		In the later case, a data structure is filled in to indicate
 *		what sequence was found, and what its parameters were.
 *
 *		This code is not intended to be a general purpose ansi parser
 *		that will work in all environments.  Its original intent
 *		is to provide a mechanism by which the console can accept
 *		and interpret device ID messages from the various terminals,
 *		and to interpret LK201 keyboard codes that are sent out by
 *		the various terminals.
 *
 *        
 *	The parser recognizes as tokens
 *
 *	regular characters
 *	2/0 - 7/15
 *	10/1 - 15/14
 *
 *	escape sequences:
 *	ESC     I..I          	F
 *	1/11	2/0 - 2/15	3/0 - 7/14
 *
 *	control sequences
 *	CSI	P..P		I..I 		F
 *	9/11	3/0 - 3/15	2/0 - 2/15	4/0 -  7/14
 *
 *	SS2	GL
 *	8/14	2/1 - 7/14
 *
 *	SS3	GL
 *	8/15	2/1 - 7/14
 *
 *	C1
 *	8/0 - 9/15		8 bit environment
 *	1/11  4/0 - 5/15	7 bit fallback
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      02-Aug-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	rhd	31-May-1995	Mod case statement to reduce code size.
 *
 *	cbf	27-Jan-1993	add arg to satisfy new read_with_prompt functn.
 *
 *	pel	06-Mar-1992	add arg to satisfy new read_with_prompt functn.
 *
 *	ajb	07-Jan-1991	Add EOF recognition
 *
 *	pel	30-Nov-1990	use err_printf instead of fprintf.
 *
 *	pel	28-Nov-1990	access stderr differently.
 *
 *	pel	20-Nov-1990	Use new fprintf instead of stubb.
 *
 *	ajb	02-Aug-1990	Adapted from KA670
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"

extern int	getchar ();
extern int	fgetc ();

#define	LOCAL_DEBUG 0


/*
 *	The following is the state transition table for the simple ansi parser
 * that the console uses.  It determines the next state to transition to
 * based on the current state and the character just read in.
 * 
 *	Each entry in the table has three fields, the current state, the
 * next state, and a range to which the input character is checked against.
 * If the current state matches the current state in the table, then the
 * next state becomes active if the input character falls within the range.
 *
 *	The order of the entries in the table is important.  A low and
 * high range of 00 and ff respectively at the end of each current state
 * provides an easy mechanism for an "all else" condition if none of the
 * previous entries match up.
 *
 */
enum {
	AS_GLGR,
	AS_C0,
	AS_C1_F,
	AS_CSI,
	AS_CSI_P,
	AS_CSI_I,
	AS_CSI_F,
	AS_ESC,
	AS_ESC_I,
	AS_ESC_F,
	AS_SSX,
	AS_SSX_F,
	AS_ERR,
	AS_DONE
} ANSI_STATES;

typedef struct {
	unsigned char	current;
	unsigned char	next;
	unsigned char	low;
	unsigned char	high;
} TRANSITION;

TRANSITION atab [] = {

/*									*/
/*Current	Next							*/
/*State		State		Low	High				*/
/*----------	----------	------	----				*/
{AS_GLGR,	AS_CSI,		0x9b,	0x9b},	/* csi sequence		*/
{AS_GLGR,	AS_SSX,		0x8e,	0x8f},	/* SS2 or SS3           */
{AS_GLGR,	AS_ESC,		0x1b,	0x1b},	/* esc sequence		*/
{AS_GLGR,	AS_GLGR,	0x00,	0xff},	/* fence		*/

{AS_ESC,	AS_ESC,		0x1b,	0x1b},	/* esc sequence		*/
{AS_ESC,	AS_GLGR,	0x18,	0x18},	/* CAN			*/
{AS_ESC,	AS_GLGR,	0x1A,	0x1A},	/* SUB			*/
{AS_ESC,	AS_C0,		0x00,	0x1f},	/* all other C0		*/
{AS_ESC,	AS_ESC_I,	0x20,	0x2f},	/* 2/0 - 2/15		*/
{AS_ESC,	AS_CSI,		0x5b,	0x5b},	/* CSI fallback		*/
{AS_ESC,	AS_C1_F,	0x40,	0x5f},	/* C1 fallback		*/
{AS_ESC,	AS_ESC_F,	0x30,	0x7E},	/* 3/0 - 7/14		*/
{AS_ESC,	AS_ERR,		0x00,	0xff},	/* all else is error	*/

{AS_ESC_I,	AS_ESC,		0x1b,	0x1B},	/* ESC			*/
{AS_ESC_I,	AS_GLGR,	0x18,	0x18},	/* CAN			*/
{AS_ESC_I,	AS_GLGR,	0x1A,	0x1A},	/* SUB			*/
{AS_ESC_I,	AS_CSI,		0x9B,	0x9B},	/* CSI			*/
{AS_ESC_I,	AS_ESC_I,	0x20,	0x2f},	/* 2/0 - 2/15  		*/
{AS_ESC_I,	AS_ESC_F,	0x30,	0x7E},	/* 3/0 - 7/14		*/
{AS_ESC_I,	AS_C0,		0x00,	0x1f},	/* all other C0		*/
{AS_ESC_I,	AS_ERR,		0x00,	0xff},	/* all else is error	*/

{AS_CSI,	AS_CSI,		0x9b,	0x9b},	/* CSI			*/
{AS_CSI,	AS_ESC,		0x1b,	0x1B},	/* ESC			*/
{AS_CSI,	AS_GLGR,	0x18,	0x18},	/* CAN			*/
{AS_CSI,	AS_GLGR,	0x1A,	0x1A},	/* SUB			*/
{AS_CSI,	AS_C0,		0x00,	0x1f},	/* all other C0		*/
{AS_CSI,	AS_CSI_P,	0x30,	0x3f},	/* 3/0 - 3/15		*/
{AS_CSI,	AS_CSI_I,	0x20,	0x2f},	/* 2/0 - 2/15		*/
{AS_CSI,	AS_CSI_F,	0x40,	0x7e},	/* final		*/
{AS_CSI,	AS_ERR,		0x00,	0xff},	/* all else is error	*/

{AS_CSI_P,	AS_CSI,		0x9b,	0x9b},	/* CSI			*/
{AS_CSI_P,	AS_ESC,		0x1b,	0x1B},	/* ESC			*/
{AS_CSI_P,	AS_GLGR,	0x18,	0x18},	/* CAN			*/
{AS_CSI_P,	AS_GLGR,	0x1A,	0x1A},	/* SUB			*/
{AS_CSI_P,	AS_C0,		0x00,	0x1f},	/* all other C0		*/
{AS_CSI_P,	AS_CSI_P,	0x30,	0x3f},	/* 3/0 - 3/15		*/
{AS_CSI_P,	AS_CSI_I,	0x20,	0x2f},	/* 2/0 - 2/15		*/
{AS_CSI_P,	AS_CSI_F,	0x40,	0x7e},	/* final		*/
{AS_CSI_P,	AS_ERR,		0x00,	0xff},	/* all else is error	*/

{AS_CSI_I,	AS_CSI,		0x9b,	0x9b},	/* CSI			*/
{AS_CSI_I,	AS_ESC,		0x1b,	0x1B},	/* ESC			*/
{AS_CSI_I,	AS_GLGR,	0x18,	0x18},	/* CAN			*/
{AS_CSI_I,	AS_GLGR,	0x1A,	0x1A},	/* SUB			*/
{AS_CSI_I,	AS_C0,		0x00,	0x1f},	/* all other C0		*/
{AS_CSI_I,	AS_CSI_I,	0x20,	0x2f},	/* 2/0 - 2/15		*/
{AS_CSI_I,	AS_CSI_F,	0x40,	0x7e},	/* final		*/
{AS_CSI_I,	AS_ERR,		0x00,	0xff},	/* all else is error	*/

{AS_SSX,	AS_SSX_F,	0x21,	0x7e},	/* only GL maps		*/
{AS_SSX,	AS_GLGR,	0x00,	0xff},	/* everything else	*/


{AS_GLGR,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_C0,		AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_C1_F,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_CSI,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_CSI_P,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_CSI_I,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_CSI_F,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_ESC,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_ESC_I,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_ESC_F,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_SSX,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_SSX_F,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_ERR,	AS_DONE,	0x00,	0xff},	/* error fence		*/
{AS_DONE,	AS_DONE,	0x00,	0xff},	/* error fence		*/

};

/*+
 * ============================================================================
 * = ansi_parser - read and parse an ANSI token from a character stream       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine interprets a byte stream as one conforming to ANSI escape
 *	and control sequences.  It tokenizes that stream, and returns on every
 *	token.  Since this routine is not intended to be a general purpose
 *	ANSI parser, certain escape and control sequences are dropped.
 *
 *	In order for the parser to work corectly, state must be maintained
 *	between calls.  This information is kept in the state structure, which
 *	is passed by reference on every call.  The state structure is
 *	allocated and initialized by the caller.  Initialization is
 *	accomplished by flooding the structure with 0.
 *  
 *	The token found is extracted from the state structure.
 *
 *	Errors in the stream (i.e. poorly formed sequences) cause the machine
 *	to reset itself and wait for more input.  An ANSI_ACCEPT status
 *	won't happen until a valid sequence is seen.
 *	
 * FORM OF CALL:
 *  
 *	ansi_parser (as, character)
 *  
 * RETURNS:
 *
 *	0 - parse complete, a complete token is in the structure.
 *	1 - need more input before reaching an accepting state
 *
 * ARGUMENTS:
 *
 *	struct ANSI *as	- address of structure for maintaining ANSI state
 * 	int character 	- next character in input stream
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int ansi_parser (struct ANSI *as, int ch) {

int status = 0;

	/* If the previous return code was an accepting state, then we
	 * clear the internal buffer of characters read in.
	 */
	if (as->rvalue == ANSI_ACCEPT) {
	    as->buf_overfl = 0;
	    as->bufix = 0;
	}

	/* remember the character in the state and check for overflow */
	as->c = ch;
	if (as->bufix < sizeof (as->buf)) {
	    as->buf [as->bufix++] = as->c;
	} else {
	    as->buf_overfl = 1;
	}

	/*
	 * Figure out what the next state is based on the current
	 * state and character just read in.
	 */
	next_ansi_state (as);

	/*
	 * Regular vanilla character.
	 */
	if (as->states [0] == (int)AS_GLGR) {
	    status = ANSI_ACCEPT;
	}

	/*
	 * Character is a C0 character that was received during
	 * a control or escape sequence.  Return the C0 character,
	 * and pick up the parsing of the sequence the next time
	 * we're called.  We do this by popping back into the state
	 * that we were in when the C0 character was received.
	 */
	else if (as->states [0] == (int)AS_C0) {
	    memcpy (
		&as->states [0],
		&as->states [1],
		sizeof (as->states) - sizeof (as->states [0])
	    );
	    status = ANSI_ACCEPT;
	}

	/*
	 * C1 character implemented in a 7 bit fallback mode.
	 * Convert the character into a real C1 character, and
	 * then bypass the input routine.  The reason for this is
	 * that we are in effect generating a character.  Revert
	 * to the state two states ago, and let that state then
	 * handle the dispatching of the C1 character.
	 */
	else if (as->states [0] == (int)AS_C1_F) {
	    as->c += 0x40;
	    as->states [0] = (int) AS_GLGR;
	    status = ANSI_NEEDMORE;
	}


	/*
	 * The character, or characters in the case of fallback mode,
	 * are a CSI introducer.  Initialize the fields that pertain
	 * to CSI sequences.
	 */
	else if (as->states [0] == (int)AS_CSI) {
	    sequence_init (as);
	    status = ANSI_NEEDMORE;
	}

	/*
	 * Character just received is a parameter.
	 */
	else if (as->states [0] == (int)AS_CSI_P) {
	    process_parameter (as);
	    status = ANSI_NEEDMORE;
	}

	/*
	 * Character just received is an intermediate
	 */
	else if (as->states [0] == (int)AS_CSI_I) {
	    process_intermediate (as);
	    status = ANSI_NEEDMORE;
	}


	/*
	 * Received the final character for a CSI.  Return a CSI
	 * indicating that a valid CSI is now in the ansi state.
	 * If the previous state was processing a parameter, then
	 * bump up the parameter count.
	 */
	else if (as->states [0] == (int)AS_CSI_F) {
	    if (as->states [1] == (int) AS_CSI_P) {
		as->pvc++;
	    }
	    as->states [0] = (int) AS_GLGR;
	    as->c = CSI;
	    status = ANSI_ACCEPT;
	}

	/*
	 * Escape character.  It is the start of an escape sequence,
	 * or some C1 sequence in the case OF fallback mode.
	 */
	else if (as->states [0] == (int)AS_ESC) {
	    sequence_init (as);
	    status = ANSI_NEEDMORE;
	}

	/*
	 * Character just received is an intermediate
	 */
	else if (as->states [0] == (int)AS_ESC_I) {
	    process_intermediate (as);
	    status = ANSI_NEEDMORE;
	}

	/*
	 * Received the final character for an ESC sequence.  Return an
	 * ESC indicating a valid escape sequence is now in the ansi state.
	 */
	else if (as->states [0] == (int)AS_ESC_F) {
	    as->states [0] = (int) AS_GLGR;
	    as->c = ESC;
	    status = ANSI_ACCEPT;
	}
		

	/*
	 * SS2 or SS3
	 */
	else if (as->states [0] == (int)AS_SSX) {
	    as->pval [0] = as->c;
	    status = ANSI_NEEDMORE;
	}

	/*
	 * Wait for the next character for a SS2 or SS3
	 */
	else if (as->states [0] == (int)AS_SSX_F) {
	    as->states [0] = (int) AS_GLGR;
	    as->c = as->pval [0];
	    status = ANSI_ACCEPT;
	}

	/*
	 * An illegal character has been encountered in the parsing of
	 * an escape or control sequence.  Set the error flag and
	 * continue parsing until the final character is encountered.
	 * Drop the offending character on the floor.
	 */
	else if (as->states [0] == (int)AS_ERR) {
	    as->error = 1;
	    memcpy (
		&as->states [0],
		&as->states [1],
		sizeof (as->states) - sizeof (as->states [0])
	    );
	    status = ANSI_NEEDMORE;
	}

	/* how do we get here ? */
	else if (as->states [0] == (int)AS_DONE) {
	    as->states [0] = (int) AS_GLGR;
	    status = ANSI_ACCEPT;
	}

	/* We should have captured the character in one of the cases above,
	 * so arrival here constitutes and error. So now what?
	 */
	/* ajbfix */

	return (as->rvalue = status);

}

/*
 * sequence_init (as)
 *
 *	An ESC or CSI has been received.  Initialize the ansi state accordingly.
 */
void sequence_init (struct ANSI *as) {

	as->il_overfl = 0;
	as->pv_overfl = 0;
	as->error = 0;
	as->pvc = 0;
	as->ilc = 0;
	as->pchar = 0;

	memset (&as->pval [0], 0, sizeof (as->pval));
	memset (&as->ilist [0], 0, sizeof (as->ilist));
}

/*
 * process_intermediate ()
 *
 * Function:	Store the current character in the intermediate list.
 *		Set the overflow flag if the list overflows.
 *
 */
void process_intermediate (struct ANSI *as) {

	/*
	 * check for overflow
	 */
	if (as->ilc >= (sizeof (as->ilist) / sizeof (as->ilist [0]))) {
	    as->il_overfl = 1;
	    return;
	}

	as->ilist [as->ilc++] = as->c;
}

/*
 * process_parameter ()
 *
 * Function:	Process a parameter character.  This version ignores everything
 *		except digits and parameter separators.
 *
 */
void process_parameter (struct ANSI *as) {

	/*
	 * Ignore the the character if there is an error or overflow
	 */
	if (as->error | as->pv_overfl) return;

	/*
	 * Dispatch on the character
	 */
	switch (as->c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    as->pval [as->pvc] *= 10;
	    as->pval [as->pvc] += as->c - '0';
	    break;

	/*
	 * Parameter separator.  Check for parameter overflow
	 */
	case ';':
	    as->pvc++;
	    if (as->pvc > (sizeof (as->pval) / sizeof (as->pval [0]))) {
		as->pv_overfl = 1;
	    }
	    return;

	case ':':
	case '<':
	case '=':
	    return;

	/*
	 * DEC private parameter
	 */
	case '>':
	case '?':
	    as->pchar = as->c;
	    return;

	default:
	    as->error = 1;
	    return; /* not an intermediate */
	}
}

/*+
 * ============================================================================
 * = next_ansi_state - calculate the next ansi state.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Generate the next state for the ansi parser given the current state and
 *	the current character.
 *  
 * FORM OF CALL:
 *  
 *	next_ansi_state (as)
 *  
 * RETURNS:
 *
 *	x - next state
 *       
 * ARGUMENTS:
 *
 * 	struct ANSI *as - ansi state
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
void next_ansi_state (struct ANSI *as) {
	int		i;

	for (i=0; i< sizeof (atab) / sizeof (atab [0]); i++) {
	    if (atab [i].current != as->states [0]) continue;
	    if ((atab [i].low <= as->c) && (as->c <= atab [i].high)) {
		memcpy (
		    &as->states [1], 
		    &as->states [0],
		    sizeof (as->states) - sizeof (as->states [0])
		); 
		as->states [0] = atab [i].next;
		return;
	    }
	}

	/* Assume a well formed table so we don't have to catch falling 
	 * off it.
	 */
}

/*
 * A table that associates LK201 escape and control sequences with token
 * numbers.
 */
struct {
	u_short	equiv;		/* what it is translated into */
	char	seq [6];	/* an escape sequence */
} keytrans [] = {

  {KEY_CUP,	"\233A"},
  {KEY_CUD,	"\233B"},
  {KEY_CUR,	"\233C"},
  {KEY_CUL,	"\233D"},

  {KEY_FIND,	"\2331~"},
  {KEY_INSERT,	"\2332~"},
  {KEY_REMOVE,	"\2333~"},
  {KEY_SELECT,	"\2334~"},
  {KEY_PREV,	"\2335~"},
  {KEY_NEXT,	"\2336~"},

  {KEY_F6,	"\23317~"},
  {KEY_F7,	"\23318~"},
  {KEY_F8,	"\23319~"},
  {KEY_F9,	"\23320~"},
  {KEY_F10,	"\23321~"},
  {KEY_F11,	"\23323~"},
  {KEY_F12,	"\23324~"},
  {KEY_F13,	"\23325~"},
  {KEY_F14,	"\23326~"},
  {KEY_HELP,	"\23328~"},
  {KEY_DO,	"\23329~"},
  {KEY_F17,	"\23331~"},
  {KEY_F18,	"\23332~"},
  {KEY_F19,	"\23333~"},
  {KEY_F20,	"\23334~"},

  {KEY_CUP,	"\033[A"},
  {KEY_CUD,	"\033[B"},
  {KEY_CUR,	"\033[C"},
  {KEY_CUL,	"\033[D"},

  {KEY_CUP,	"\033OA"},
  {KEY_CUD,	"\033OB"},
  {KEY_CUR,	"\033OC"},
  {KEY_CUL,	"\033OD"},

  {KEY_0,	"\033Op"},
  {KEY_1,	"\033Oq"},
  {KEY_2,	"\033Or"},
  {KEY_3,	"\033Os"},
  {KEY_4,	"\033Ot"},
  {KEY_5,	"\033Ou"},
  {KEY_6,	"\033Ov"},
  {KEY_7,	"\033Ow"},
  {KEY_8,	"\033Ox"},
  {KEY_9,	"\033Oy"},
  {KEY_PF1,	"\033OP"},
  {KEY_PF2,	"\033OQ"},
  {KEY_PF3,	"\033OR"},
  {KEY_PF4,	"\033OS"},
  {KEY_COMMA,	"\033Ol"},
  {KEY_DASH,	"\033Om"},
  {KEY_PERIOD,	"\033On"},
  {KEY_ENTER,	"\033OM"},

  {KEY_FIND,	"\033[1~"},
  {KEY_INSERT,	"\033[2~"},
  {KEY_REMOVE,	"\033[3~"},
  {KEY_SELECT,	"\033[4~"},
  {KEY_PREV,	"\033[5~"},
  {KEY_NEXT,	"\033[6~"},

  {KEY_F6,	"\033[17~"},
  {KEY_F7,	"\033[18~"},
  {KEY_F8,	"\033[19~"},
  {KEY_F9,	"\033[20~"},
  {KEY_F10,	"\033[21~"},
  {KEY_F11,	"\033[23~"},
  {KEY_F12,	"\033[24~"},
  {KEY_F13,	"\033[25~"},
  {KEY_F14,	"\033[26~"},
  {KEY_HELP,	"\033[28~"},
  {KEY_DO,	"\033[29~"},
  {KEY_F17,	"\033[31~"},
  {KEY_F18,	"\033[32~"},
  {KEY_F19,	"\033[33~"},
  {KEY_F20,	"\033[34~"}
};

/*+
 * ============================================================================
 * = get_keycode - return the next keycode from an LK201 or IBM keyboard.     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine returns the next ANSI token from an input stream.  If the
 *	the token is an escape sequence that is generated an LK201 or an IBM
 *	keyboard, then that sequence is accepted, and an appropriate keyboard
 *	token returned.  Otherwise, the state returned by the ansi parser is
 *	returned.
 *  
 * FORM OF CALL:
 *  
 *	get_keycode (as, input, input_param)
 *  
 * RETURNS:
 *
 *	x - keyboard token or results from ansi parser
 *       
 * ARGUMENTS:
 *
 *	struct ANSI *as	- address of structure for maintaining ANSI state
 * 	int *input ()	- address of routine to fetch next character
 *	int input_param - callback parameter
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int get_keycode (struct ANSI *as, int (*input) (), unsigned int input_param) {
	int	i;
	int	c;

	/* Keep feeding the parser with characters until we reach a
	 * final state
	 */
	do {
	    c = (*input) (input_param);
	    if (c == EOF) return EOF;
	} while (ansi_parser (as, c) == ANSI_NEEDMORE);

	/*
	 * See if the token returned matches an LK201 keystroke
	 */
	for (i=0; i<sizeof (keytrans) / sizeof (keytrans [0]); i++) {
	    if (strncmp (keytrans [i].seq, & as->buf [0], as->bufix) == 0) {
		return keytrans [i].equiv;
	    }
	}

	/* otherwise return the sequence found */
	return as->c;
}

#if LOCAL_DEBUG

char *state_strings [] = {
	"AS_GLGR",
	"AS_C0",
	"AS_C1_F",
	"AS_CSI",
	"AS_CSI_P",
	"AS_CSI_I",
	"AS_CSI_F",
	"AS_ESC",
	"AS_ESC_I",
	"AS_ESC_F",
	"AS_SSX",
	"AS_SSX_F",
	"AS_ERR",
	"AS_DONE",
	""
};


/*---------*/
void dump_state (struct ANSI *as) {
	int	i;

	printf ("il=%d pv=%d err=%d\n", as->il_overfl, as->pv_overfl, as->error);

	for (i=0; i<sizeof (as->states) / sizeof (as->states [0]); i++) {
	    printf ("%s ", state_strings [as->states [i]]);
	}
	printf ("\n");

	printf ("c=%c pchar=%c\n", as->c, as->pchar);

	printf ("pvc (%d) ", as->pvc);
	for (i=0; i<sizeof (as->pval) / sizeof (as->pval [0]); i++) {
	    printf ("%d ", as->pval [i]);
	}
	printf ("\n");

	printf ("ilc (%d) ", as->ilc);
	for (i=0; i<sizeof (as->ilist) / sizeof (as->ilist [0]); i++) {
	    printf ("%02X ", as->ilist [i]);
	}
	printf ("\n");
	
}



char *input [] = {
	"abcdef\033[cghi\033(A",
	"\033[A",
	"\033[B",
	"\033[C",
	"\033[D",
	"\033OA",
	"\033OB",
	"\033OC",
	"\033OD",
	"\033Op",
	"\033Oq",
	"\033Or",
	"\033Os",
	"\033Ot",
	"\033Ou",
	"\033Ov",
	"\033Ow",
	"\033Ox",
	"\033Oy",
	"\033OP",
	"\033OQ",
	"\033OR",
	"\033OS",
	"\033Ol",
	"\033Om",
	"\033On",
	"\033OM",
	"\033[1~",
	"\033[2~",
	"\033[3~",
	"\033[4~",
	"\033[5~",
	"\033[6~",
	"\033[17~",
	"\033[18~",
	"\033[19~",
	"\033[20~",
	"\033[21~",
	"\033[23~",
	"\033[24~",
	"\033[25~",
	"\033[26~",
	"\033[28~",
	"\033[29~",
	"\033[31~",
	"\033[32~",
	"\033[33~",
	"\033[34~",
	"Z",
	""
};

char	*ip = 0;
int	ix = 0;

int local_next () {
	return kbin ();

	if (ip == 0) {
	    ix = 0;
	    ip = input [ix];
	}
	if (*ip == 0) {
	    if (ip == input [ix]) return kbin ();
	    ix++;
	    ip = input [ix];
	}
	printf ("%02X ", (unsigned int) *ip & 0x0000000ff);
	return *ip++;
}

/*----------*/
void main (int argc, char *argv []) {
	struct ANSI	ap, *as;
	int		i;
	int		c;
	extern int	kbin ();
	char		buf [32];

	while (1) {
	    read_with_prompt ("pr>", sizeof (buf), buf, 0, 0, 1);
	    printf ("%s", buf);
	}

	as = &ap;
	memset (as, 0, sizeof (ap));

	while (1) {
	    get_keycode (as, kbin);
	}
	exit (1);

	for (i=0; i<sizeof (input); i++) {
	    switch (c = ansi_parser (as, local_next)) {
	    case ESC:
		printf ("ESC: %02X\n", (unsigned int) as->c & 0xff);
		break;
	    case CSI:
		printf ("CSI: %02X\n", (unsigned int) as->c & 0xff);
		break;
	    case SS2:
		printf ("SS2: %02X\n", (unsigned int) as->c & 0xff);
		break;
	    case SS3:
		printf ("SS3: %02X\n", (unsigned int) as->c & 0xff);
		break;
	    default:
		printf ("DEF: %02X\n", (unsigned int) as->c & 0xff);
	    }
	}
}



#include	stdio	/* exclude */
#include	ssdef	/* exclude */
#include	descrip	/* exclude */
#include	iodef	/* exclude */
#include	ctype	/* exclude */

#define	FALSE	0
#define	TRUE	1
#define	EOS	0

#define	BUFFLEN	32			/* Size of typeahead buffer	*/

/*
 * Local static database:
 */

$DESCRIPTOR(inpdev, "TT");	/* Terminal to use for input	*/
static long termset[2] = { 0, 0 };	/* No terminator		*/

/*
 * Local variables
 */

static long	ichan;			/* Gets channel number for TT:	*/
static char	opened = 0;		/* TRUE when opened		*/
static char	ibuff [BUFFLEN];	/* Input buffer -- one byte	*/
static char	*buffptr = ibuff;	/* For typeahead processing	*/
static char	*buffend = ibuff;	/* For typeahead processing	*/

/**********
 * Get one byte without echoing
 */
int kbin () {

	register int	errorcode;
	struct IOSTAB {
		short int	status;
		short int	offset_to_terminator;
		short int	terminator;
		short int	terminator_size;
	} iostab;

	if (buffptr < buffend) {
	    return (*buffptr++ & 0377);	/* Empty our typeahead buffer	*/
	}

	if (!opened) {
	    if ((errorcode = sys$assign(&inpdev, &ichan, 0, 0)) != SS$_NORMAL) {
		err_printf( "KBIN assign failed.  code = %X\n", errorcode);
		exit (errorcode);
	    }
	    opened = TRUE;
	}

	/*
	 * See if there's something in the system typeahead buffer
	 * Read up to BUFLEN bytes with "zero" timeout.  This will return
	 * whatever's in the timeout buffer.  The iostab.offset_to_terminator
	 * and iostab.terminator_size will yield the number of bytes read.
	 */
	errorcode = sys$qiow(1,		/* Event flag 			*/
		ichan,			/* Input channel		*/
		IO$_READLBLK | IO$M_NOECHO | IO$M_NOFILTR | IO$M_TIMED,
					/* Timed read with zero wait	*/
		&iostab,		/* I/O status block		*/
		NULL,			/* AST block (none)		*/
		0,			/* AST parameter		*/
		&ibuff,			/* P1 - input buffer		*/
		BUFFLEN,		/* P2 - buffer length		*/
		0,			/* P3 - ignored (timeout)	*/
		&termset,		/* P4 - terminator set		*/
		NULL,			/* P5 - ignored (prompt buffer)	*/
		0);			/* P6 - ignored (prompt size)	*/

	buffend = &ibuff[iostab.offset_to_terminator + iostab.terminator_size];
	if (buffend > ibuff) {
	    buffptr = &ibuff [1];	/* Setup typeahead pointer and	*/
	    return (ibuff [0] & 0377);	/* Return the first character	*/
	}

	/*
	 * Nothing in typeahead buffer, nothing read.  Read one character.
	 */
	errorcode = sys$qiow(1,		/* Event flag 			*/
		ichan,			/* Input channel		*/
		IO$_READLBLK | IO$M_NOECHO | IO$M_NOFILTR,
					/* Read, no echo, no translate	*/
		&iostab,		/* I/O status block		*/
		NULL,			/* AST block (none)		*/
		0,			/* AST parameter		*/
		&ibuff,			/* P1 - input buffer		*/
		1,			/* P2 - buffer length		*/
		0,			/* P3 - ignored (timeout)	*/
		&termset,		/* P4 - terminator set		*/
		NULL,			/* P5 - ignored (prompt buffer)	*/
		0);			/* P6 - ignored (prompt size)	*/

	if (errorcode == SS$_NORMAL) {
	    return (ibuff [0] & 0377);
	} else {
	    return (EOF);
	}
}
#endif
