/* file:	readline.c
 *
 * Copyright (C) 1990, 1992 by
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
 *      Readline functions for the Alpha firmware.  Still missing:
 *~
 *		1) terminal type mapping (MCS etc)
 *		2) multiline continuation
 *~
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      dd-mmm-yyyy
 *
 *  MODIFICATION HISTORY:
 *
 *      jrk     24-Apr-1995     Add hardcopy/video
 *                              
 *      jrk     14-Sep-1993     Add help, ^j, do
 *                              
 *      cbf     12-jan-1992     read_with_prompt gets new argument - echo
 *                              
 *	ajb	19-Aug-1992	Fix case where long lines aren't handled
 *				correctly, causing corruptions.
 *
 *	pel	06-Mar-1992	read_with_prompt; add support w/in scripts by
 *				accepting 2 optional args, for stdin/out so
 *				that the two channels may be different than
 *				the current process' stdin/out.
 *
 *	ajb	15-jan-1992	Add a killpending check in the inner loop.
 *				This forces a shell that has been ^x'd 
 *				to recognize sooner that it had better
 *				get out of the way.
 *
 *	ajb	13-Jan-1992	add an optional file pointer to
 *				read_with_prompt so that this routine may
 *				work within scripts (within scripts stdin
 *				is not the tt)
 *
 *      pel     05-Sep-1991	read_with_prompt; do printf (prompt) instead
 *				of printf("%s", prompt) to support msg facilty
 *
 *	ajb	18-Jan-1991	Fix recall nits.
 *
 *	pel	28-Nov-1990	access stdin differently.
 *
 *	ajb	11-Oct-1990	Don't allow ^u to store blank lines.
 *
 *	ajb	16-Aug-1990	Recall buffer initialization is now via a
 *				separate routine so code can be restarted.
 *
 *	ajb	06-Aug-1990	Command line editing and recall
 *
 *	ajb	14-May-1990	Return EOF on a ^Z
 *
 *	ajb	01-May-1990	Remove read$, write$ (now handled by
 *				the shell and the drivers).
 *
 *	ajb	19-Apr-1990	add read$, write$ for synchronization
 *
 *	ajb	10-Apr-90	Translated from KA670.
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:common.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:tt_def.h"
#include "cp$inc:prototypes.h"

#define HELP_KEY	0x8		/* ^h */

enum {
	rdl_def,
	rdl_ret,
	rdl_bol,
	rdl_del,
	rdl_ctrlc,
	rdl_ctrld,
	rdl_ctrlj,
	rdl_ctrlr,
	rdl_ctrlt,
	rdl_ctrlu,
	rdl_ctrlx,
	rdl_up,
	rdl_right,
	rdl_left,
	rdl_down,
	rdl_insert,
	rdl_eol,
	rdl_help,
	rdl_do,
	rdl_ignore,
	rdl_eof,
	rdl_tab
} rdl_types;

/*
 * Create a table that allows us to map character codes into tokens.  A token
 * is returned if the input character is within the low and high values.
 */
struct {
	unsigned int	low;
	unsigned int	high;
	int		token;
} rdl_tokens [] = {
	{0x0020,	0x007e,		rdl_def},	/* default			*/
	{0x000d,	0x000d,		rdl_ret},	/* carriage return		*/
	{0x0009,	0x0009,		rdl_tab},	/* tab				*/
	{0x001a,	0x001a,		rdl_eof},	/* control Z			*/
	{BS,		BS,		rdl_bol},	/* go to begining of line	*/
	{DEL,		DEL,		rdl_del},	/* delete			*/
	{CTRL_B,	CTRL_B,		rdl_up},	/* recall previous line		*/
	{CTRL_C,	CTRL_C,		rdl_ctrlc},	/* control C			*/
	{CTRL_D,	CTRL_D,		rdl_ctrld},	/* control D			*/
	{CTRL_F,	CTRL_F,		rdl_right},	/* cursor right			*/
	{CTRL_J,	CTRL_J,		rdl_ctrlj},	/* delete word			*/
	{CTRL_R,	CTRL_R,		rdl_ctrlr},	/* redisplay line		*/
	{CTRL_T,	CTRL_T,		rdl_ctrlt},	/* control T			*/
	{CTRL_U,	CTRL_U,		rdl_ctrlu},	/* delete line			*/
	{CTRL_X,	CTRL_X,		rdl_ctrlx},	/* control X			*/
	{KEY_HELP,	KEY_HELP,	rdl_help},	/* help                         */
	{KEY_F9,	KEY_F9,		rdl_help},	/* help                         */
	{KEY_DO,	KEY_DO,		rdl_do},	/* do                           */
	{KEY_F10,	KEY_F10,	rdl_do},	/* do                           */
	{KEY_CUP,	KEY_CUP,	rdl_up},	/* escape sequences from the 	*/
	{KEY_CUD,	KEY_CUD,	rdl_down},	/* cursor keypad		*/
	{KEY_CUR,	KEY_CUR,	rdl_right},
	{KEY_CUL,	KEY_CUL,	rdl_left},
	{CTRL_A,	CTRL_A,		rdl_insert},	/* toggle insert/overstrike mode*/
	{CTRL_E,	CTRL_E,		rdl_eol},	/* go to end of line		*/
	{CTRL_H,	CTRL_H,		rdl_bol},	/* go to begining of line	*/
	{0x00A1,	0x00fe,		rdl_def},	/* iso-latin1 printables	*/
	{0x0000,	-1,		rdl_ignore},	/* ignore everything else	*/
};

/*
 * The recall buffers hangs off of the tty port block.  This allows recall
 * buffers to be terminal specific.  Default MAX_RECALL if it isn't defined
 * elsewhere.
 */
#ifndef MAX_RECALL
#define	MAX_RECALL	16
#endif

typedef struct {
	int	overstrike;
	int	terminator;
	char	*ptrs [MAX_RECALL];
	struct SEMAPHORE	serial;
} RCB;

#if MODULAR
#define in_del_clear(a) _in_del_clear(a)
void _in_del_clear();
#else
#define in_del_clear(a)
#endif

/*+
 * ============================================================================
 * = rcb_init - initialize the recall buffer                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Initialize a recall buffer.  This routine will not deallocate any
 *	previously initialized buffers.
 *  
 * FORM OF CALL:
 *  
 *	rcb_init (ttpb)
 *  
 * RETURNS:
 *
 *      msg_success - success
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void rcb_init (struct TTPB *ttpb) {
	RCB *rcb;

	ttpb->rcb = rcb = (RCB *) dyn$_malloc (
	    sizeof (RCB),
	    DYN$K_SYNC | DYN$K_NOOWN | DYN$K_FLOOD
	);

	rcb->overstrike = 0;
	rcb->terminator = 0;
	krn$_seminit (&rcb->serial, 1, "rwp_sync");
}

/*+
 * ============================================================================
 * = read_with_prompt - read line from standard input and echo prompt         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Read in a line from the standard input, prompting the user in the
 *	process.  Support simple editing commands such as insert/overstrike,
 *	goto begining/end of line, repaint the line, horizontal cursor
 *	positioning, command recall, and abort.  Control characters are
 *	ignored. Tabs are translated to spaces.
 *
 *	Up to n-1 characters are read in (space is left for the terminating
 *	newline).  The resulting buffer is always null terminated.
 *
 *	Specifically, the folowing editing sequences are supported:
 *
 *	^U	abort the current call.  Return 0.
 *	^R	Redisplay the line line current line and leave the cursor at
 *		the end of the line.
 *	CUP	(cursor up).  Recall the next line in the recall buffer.  If
 *		at the end of the recall buffer, then generate a blank line.
 *	CUD	(cursor down) Recall the previous line in the recall buffer. If
 *		at the begining of the buffer, then generate a blank line.
 *	CUR	(cursor right).  Move the cursor right one position.  If at the
 *		end of the line, then no action is taken.
 *	CUL	(cursor left).  Move the cursor left one position.  If at the
 *		begining of the line, then no action is taken.
 *	^H	Move to the begining of the line.
 *	^E	Move to the end of the line.
 *	^A	Toggle between insert and overstrike mode.
 *	^J	Erase previous word.
 *	^Z	Treat as an EOF if it is the first character typed.
 *
 *	The call is terminated on a
 *		- newline
 *		- a ^Z if the first character typed
 *		- a ^U
 *
 *	If terminated with a ^Z, then EOF is returned.  In all other cases,
 *	strlen (buf) is returned.
 * 
 * FORM OF CALL:
 *  
 *	read_with_prompt (*prompt, maxbuf, *buf, fp_in, fp_out, echo)
 *  
 * RETURNS:
 *
 *      x - Number of characters read in or EOF.  A blank line has at least 1
 *	    character, the newline.
 *       
 * ARGUMENTS:
 *
 *      char *prompt -	address of primary prompt string
 *	int maxbuf -	size of input buffer
 *	char *buf -	address of input buffer, sized to allow inclusion of
 *			terminating null.
 *	struct FILE *fp_in  - optional file pointer.  If 0, then use stdin
 *			of the current process.
 *	struct FILE *fp_out - optional file pointer.  If 0, then use stdout
 *			of the current process.
 *      int echo -      surpress echo of input to prompt if echo = 0.
 *                      also keeps input out of recall buffer.  All other
 *                      functions are the same.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int read_with_prompt (char *prompt, int maxbuf, char *buf, 
			struct FILE *fp_in, struct FILE *fp_out,
                        int echo ) {
	int	c;
	int	i;
	int	done;
	int	length;
	int	index;
	int	in_del;
	int	fromtty;
	int	console_type;
	int	help_up = 0;
	int	subdata[2];
	struct PCB *pcb;
	struct ANSI *as, ap;
	extern int fgetc ();
	int	rcb_index;
	struct TTPB *ttpb;
	struct FILE *infp, *outfp;
	RCB *rcb;

	/*
	 * If the user has supplied an FP, use that.  We assume that
	 * this FP is opened for read and write.  Otherwise we
	 * default to stdin and stdout.
	 */
	pcb = getpcb();
	if (fp_in) {
	    infp = fp_in;
	} else {
	    infp = pcb->pcb$a_stdin;
	}

	if (fp_out) {
	    outfp = fp_out; 
        } else {
	    outfp = pcb->pcb$a_stdout;
	}
        
	subdata[0] = outfp;
	subdata[1] = &in_del;

	/*
	 * The recall buffer is hanging off of the tty port block.
	 * If we're connected to a tty, get the address of the recall
	 * buffer.  If we're not connected to a tty, then there is no recall
	 * buffer, and the keycodes that control access to the recall buffer
	 * are ignored.
	 */
	if (isatty (infp)) {
	    ttpb = (struct TTPB *) infp->ip->misc;
	    console_type = ttpb->type;
	    ttpb->page = ttpb->spage;		/* restore if zeroed */
	    ttpb->linecnt = 0;
	    ttpb->stop = 0;
	    rcb = ttpb->rcb;
	} else {
	    rcb = 0;
	    ttpb = 0;
	}

	/*
	 * The recall buffer index is set to -1 on underflow (i.e all
	 * recallable buffers are reached by cursor up) and to MAX_RECALL on
	 * overflow (all recallable buffers are reached by cursor down).
	 */
	rcb_index = -1;

	/*
	 * Serialize access
	 */
	if (rcb) krn$_wait (&rcb->serial);

	fprintf (outfp, prompt);
	done = 0;
	length = 0;
	index = 0;
	in_del = 0;
	as = &ap;
	memset (as, 0, sizeof (struct ANSI));
                
	if (rcb)
	if (rcb->terminator == HELP_KEY) {
	    rcb->terminator = 0;
	    c = KEY_CUP;
	    fromtty = 0;
	    help_up = 1;
	    goto read_with_prompt_loop;
	}

	while (!done) {

	    /* keep terminated */
	    buf [length] = 0;

	    /* terminate the process if killpending */
	    if (killpending ()) {
		if (rcb) krn$_post (&rcb->serial);
		return 0;
	    }

	    /* Read in the next keycode.  Keycodes may be regular ansi
	     * characters, or in the case of keypad keys, a 16 bit value.
	     */
	    fromtty = 1;
	    c = get_keycode (as, fgetc, infp);
	    if (c == EOF) c = 0x1a;
	    
read_with_prompt_loop:
	    /*
	     * Figure out which token we have
	     */
	    for (i=0; i<sizeof (rdl_tokens) / sizeof (rdl_tokens [0]); i++) {
      	        if ((rdl_tokens [i].low <= c)  &&  (c <= rdl_tokens [i].high))
                {
		    break;
		}
	    }

#if MODULAR
	    if (console_type) {
		switch (rdl_tokens [i].token) {
		    case rdl_insert:
		    case rdl_bol:
		    case rdl_eol:
		    case rdl_left:
		    case rdl_right:
		    case rdl_ctrlj:
			fputc (7, outfp);
			continue;
		}
	    }
#endif

	    switch (rdl_tokens [i].token) {

	    /*
	     * On an EOF, treat as an EOF if it is the first character
	     * on the command line.  Otherwise, ignore it.
	     */
	    case rdl_eof:
		in_del_clear(&subdata);
		if ((length == 0) && (index == 0)) {
		    fprintf (outfp, "^Z\n");
		    done = 1;
		    if (rcb) rcb->terminator = 0x1a;
		    if (rcb) krn$_post (&rcb->serial);
		    return 0;
		}
		break;

	    case rdl_eol:
          	fprintf (outfp, "%s", buf + index);
		index = strlen (buf);
		break;                             
		
	    case rdl_bol:
		index = 0;
		fprintf (outfp, "\r%s", prompt);
		break;

	    case rdl_help:
		if (length < (maxbuf - 4)) {
		    if (length != 0)
			buf [length++] = ' ';
		    buf [length++] = HELP_KEY;	/* ^h */
		    buf [length++] = '\n';
		    buf [length] = 0;
		    printf ("\n");
		    if (rcb) rcb->terminator = HELP_KEY;
		    done = 1;
		}
		break;

	    case rdl_do:
		if (length != 0) break;
		if (index != 0) break;
		strcpy(&buf[0], "do_key\n");
		length = index = strlen (buf);
		printf ("do\n");
		if (rcb) rcb->terminator = '\n';
		done = 1;
		break;

	    /*
	     * Fetch the previous line in the recall buffer.
	     */
	    case rdl_up:
		if (rcb) {
		    rcb_index++;
		    if (rcb_index >= MAX_RECALL) rcb_index = MAX_RECALL;

		    /* index points to a valid buffer */
		    if ((rcb_index < MAX_RECALL) && rcb->ptrs [rcb_index]) {
			length = clone_rcb (rcb->ptrs [rcb_index], buf, maxbuf-1);

		    /* index is outside of range of current buffers */
		    } else {
			buf [0] = 0;
			length = 0;

			/* Insure that the index points to just one
			 * beyond the last valid buffer
			 */
			if (rcb_index && (rcb->ptrs [rcb_index-1] == 0)) {
			    rcb_index--;
			}
		    }

		    index = length;
#if MODULAR
		    if (console_type)
			if (fromtty)
			    fprintf (outfp, " ^\n");
#endif
		    if (help_up) {
			help_up = strlen(buf);
			if (buf[help_up-1] == ' ') {
			    buf[help_up-1] = 0;
			    index--;
			    length--;
			}
			help_up = 0;
		    }
		    render_line (prompt, buf, index, outfp, console_type);
		}
		break;
		
	    case rdl_down:
		if (rcb) {
		    rcb_index--;

		    /* we've underflowed the recall buffer, so generate a
		     * blank line.
		     */
		    if (rcb_index < 0) {
			buf [0] = 0;
			length = 0;
			rcb_index = -1;

		    } else {
			length = clone_rcb (rcb->ptrs [rcb_index], buf, maxbuf-1);
		    }
		    index = length;
#if MODULAR
		    if (console_type)
			fprintf (outfp, " v\n");
#endif
		    render_line (prompt, buf, index, outfp, console_type);
		}
		break;

	    case rdl_left:
		if (index == 0) break;
		index--;
		fputc ('\b', outfp);
		break;

	    case rdl_right:
		if (index >= length) break;
		fputc (buf [index++], outfp);
		break;

	    case rdl_insert:
		if (rcb) rcb->overstrike ^= 1;
		break;

	    /* A return is always accepted, even if it truncates
	     * existing data
	     */
	    case rdl_ret:
		in_del_clear(&subdata);
		buf [min (length, maxbuf-2)] = '\n';
		length++;
		fprintf (outfp, "\n");
		if (rcb) rcb->terminator = '\n';
		done = 1;
		break;

	    case rdl_del:
#if MODULAR
		if (!console_type) {	/* video */
#endif
		if (length == 0) break;
		if (index == 0) break;
		index--;
		for (i=index; i<length; i++) buf [i] = buf [i+1];
		length--;
                if ( echo )
		   fprintf (outfp, "\b%s \b", & buf [index]);
		for (i=index; buf [i]; i++) fputc ('\b', outfp);

#if MODULAR
		} else {			/* hardcopy */
		if (length != 0) {
		    length--;
		    index--;
		    if (!in_del) {
			fputc ('\\', outfp);
			in_del = 1;
		    }
		    fputc(buf[length], outfp);
		}
		if (length == 0)
		    in_del_clear(&subdata);
		}
#endif
		break;

	    case rdl_ctrlc:
		in_del_clear(&subdata);
		printf ("^C\n");
		length = 0;
		buf [length++] = 3;		/* ^C */
		buf [length] = 0;
		if (rcb) rcb->terminator = 3;
		done = 1;
		break;

	    case rdl_ctrld:
		in_del_clear(&subdata);
		printf ("^D\n");
		length = 0;
		buf [length++] = 3;		/* Treat as ^C */
		buf [length] = 0;
		if (rcb) rcb->terminator = 3;
		done = 1;
		break;

	    case rdl_ctrlx:
		in_del_clear(&subdata);
		printf ("^X\n");
		length = 0;
		buf [length++] = 3;		/* Treat as ^X */
		buf [length] = 0;
		if (rcb) rcb->terminator = 3;
		done = 1;
		break;

	    case rdl_ctrlt:
		in_del_clear(&subdata);
		printf ("^T\n");
		length = 0;
		buf [length++] = 3;		/* Treat as ^C */
		buf [length] = 0;
		if (rcb) rcb->terminator = 3;
		done = 1;
		break;

	    case rdl_ctrlr:
		in_del_clear(&subdata);
		fprintf (outfp, "%s^R\n", & buf [index]);
                if ( echo )     
		   render_line (prompt, buf, index, outfp, console_type);
		break;
		
	    case rdl_ctrlu:
		in_del_clear(&subdata);
		fprintf (outfp, "%s^U\n", & buf [index]);
		length = 0;
		index = 0;
		buf [index] = 0;
		   render_line (prompt, buf, index, outfp, console_type);
		break;

	    case rdl_ctrlj:
		if (length == 0) break;
		if (index == 0) break;
		if (buf[index-1] == ' ')
		    goto rdl_ctrlj_loop;
		while (buf[index-1] != ' ') {
rdl_ctrlj_loop:
			index--;
			for (i=index; i<length; i++) buf [i] = buf [i+1];
			length--;
			buf [length] = 0;
			printf ("\b%s \b", & buf [index]);
			for (i=index; buf [i]; i++) putchar ('\b');
			if (length == 0) break;
			if (index == 0) break;
		}
		break;

	    /* ajbfix: more general solution needed for control characters */
	    /* tabs get converted into spaces (quick fix to recall problem) */
	    case rdl_tab:
		c = ' ';
		/* fallthrough */

	    /* insert a character into the buffer.  Leave room for the
	     * terminating newline and null
	     */
	    case rdl_def:
		if (length >= maxbuf-2) break;
		in_del_clear(&subdata);
		if (rcb && rcb->overstrike) {
		    buf [index] = c;
                    if ( echo )
		       fputc (c, outfp);
		    if (index >= length) length++;
		    index++;
		} else {
		    length++;
		    for (i=length; i>index; i--) buf [i] = buf [i-1];
		    buf [index] = c;
		    index++;
                    if ( echo )
		       fprintf (outfp, "%s", & buf [index - 1]);
		    for (i=index-1; i<length-1; i++) fputc ('\b', outfp);
		}
		break;

	    case rdl_ignore:
		break;
	    }
	}

	/* keep terminated */
	buf [length] = 0;
	if (ttpb)
	    ttpb->linecnt = 0;

	/*
	 * Install the line into the recall buffer subject to several
	 * conditions.
	 */
	if (rcb && 		/* gotta have a recall buffer             */
             echo &&            /* don't want to recall an un-echoed line  */
	    (length > 1) && 	/* simple newlines are ignored            */
	    (rcb->ptrs [0] ? strcmp (buf, rcb->ptrs [0]) : 1)) {

	    /* last line in the recall buffer gets dropped if it's there */
	    if (rcb->ptrs [MAX_RECALL-1]) {
		dyn$_free (rcb->ptrs [MAX_RECALL-1], DYN$K_SYNC);
		rcb->ptrs [MAX_RECALL-1] = 0;
	    }

	    /* shift all the lines up by one */
	    for (i=MAX_RECALL-1; i>0; i--) {
		rcb->ptrs [i] = rcb->ptrs [i-1];
	    }

	    /* Allocate a new one */
	    rcb->ptrs [0] = strcpy (dyn$_malloc (length + 1, DYN$K_SYNC | DYN$K_NOOWN), buf);
	}

	if (rcb) krn$_post (&rcb->serial);
	return length;
}

/*----------*/
void render_line (char *prompt, char *buf, int index, struct FILE *fp, int console_type)
{
#if MODULAR
    if (!console_type)
#endif
	fprintf (fp, "\r%s%s\033[0K\r\033[%dC",
	  prompt,
	  buf,
	  strlen (prompt) + index);

#if MODULAR
    else
	fprintf (fp, "\r%s%s", prompt, buf);
#endif
}

#if MODULAR
void _in_del_clear(int *subdata[])
{
    if (*subdata[1]) {
	fputc ('\\', subdata[0]);
	*subdata[1] = 0;
    }
}
#endif

/*
 * Copy from a recall buffer into a working string.  Strip off
 * the recall buffer's newline if it doesn't get truncated in the
 * copy.
 *
 * Return the length of the resulting null terminated buffer.
 */
int clone_rcb (char *recall, char *buf, int n) {
	char *dst;

	dst = buf;
	while ((*recall != '\n') && (n > 0)) {
	    if (*recall == HELP_KEY)
		break;
	    *dst++ = *recall++;
	    n--;
	}
	*dst = 0;
	return (dst - buf);
}

#if 0

/*+
 * ============================================================================
 * = recall - Show the recall buffer.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The recall command prints out the entire recall buffer.  The most
 *	recent commands are listed first.  The recall command itself is not
 *	listed.
 *  
 *   COMMAND FORM:
 *
 *      recall ( )
 *
 *   TAG: recall 0 RXBZ recall
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	None
 *
 *   COMMAND OPTION(S):
 *
 * 	None
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>echo foo
 *	foo
 *	>>>echo bar
 *	bar
 *	>>>echo this
 *	this
 *	>>>echo that
 *	that
 *	>>>recall
 *	 0: `echo that'
 *	 1: `echo this'
 *	 2: `echo bar'
 *	 3: `echo foo'
 *      >>>
 *~
 * FORM OF CALL:
 *  
 *	recall ( int argc, char *argv [] )
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion.
 *	msg_failure - routine failed
 *       
 * ARGUMENTS:
 *
 * 	argc		- number of words present on command line
 *	char *argv[]	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	A file may be modified.
 *
-*/
int recall( int argc, char *argv[] ) {
    RCB *rcb;
    int i;
    struct PCB *pcb;
    struct FILE *fp;
    struct TTPB *ttpb;
	struct ANSI ap;
	extern int fgetc ();
	int	c;

    pcb = getpcb();
    if (isatty (pcb->pcb$a_stdin)) {
	fp = (struct FILE *)pcb->pcb$a_stdin;
	ttpb = (struct TTPB *)fp->ip->misc;
	rcb = ttpb->rcb;
	if (rcb == 0) return msg_success;

	/* wipe out this command's footprint in the recall buffer */
	dyn$_free (rcb->ptrs [0], DYN$K_SYNC);
	for (i = 0; i<MAX_RECALL-1; i++) {
	    rcb->ptrs[i] = rcb->ptrs[i+1];
	}
	rcb->ptrs [MAX_RECALL-1] = 0;

	show_rcb (rcb);
    }
}

void show_rcb (RCB *rcb) {
	int i;

	/* print out the buffer */
	for( i = 0; i < MAX_RECALL; i++ ) {
	    if (rcb->ptrs [i]) {
		printf ("%2d: %s", i, rcb->ptrs[i]);
	    }
	}
}
#endif
