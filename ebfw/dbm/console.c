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
__attribute__((unused)) static const char *rcsid = "$Id: console.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: console.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.27  1997/10/07  16:56:37  pbell
 * Updated kbdcontinue to ignore -1 characters returned by
 * UserGetChar.  The SROM and BROADCAST ports return -1 when
 * no character is availible.
 *
 * Revision 1.26  1997/04/05  13:21:31  fdh
 * Allow 0x to force numerical arguments to be interpreted
 * as hex values.  Both hexarg and decarg will be loaded with
 * the same value.
 *
 * Revision 1.25  1996/08/20  22:38:22  fdh
 * Modified ESC-backspace, ESC-delete, ESC-h, ESC-b, ESC-f, ESC-d
 * to use !isalnum, non-alphanumeric, instead of isspace, whitespace,
 * to delimit its strings.
 *
 * Revision 1.24  1996/06/18  17:34:36  fdh
 * Call UserGetChar from kbdcontinue().
 *
 * Revision 1.23  1996/06/13  03:39:14  fdh
 * Call UserPutChar instead of PutChar to support the
 * BROADCAST port type.
 *
 * Revision 1.22  1996/05/22  22:11:43  fdh
 * Added support for the SROM serial port.
 *
 * Revision 1.21  1995/12/11  23:17:17  fdh
 * I didn't implement a _tolower macro.  What was I thinking?
 * Changed back to tolower().
 *
 * Revision 1.20  1995/11/30  19:42:08  cruz
 * Fixed syntax error.
 *
 * Revision 1.19  1995/11/30  19:01:08  fdh
 * Only echo printable and space characters.
 *
 * Revision 1.18  1995/11/20  00:34:25  fdh
 * Modified to use the _tolower() macro instead
 * of the tolower() function call.
 *
 * Revision 1.17  1995/11/17  16:39:13  cruz
 * Fixed bug in donumber() which caused a conversion error
 * when the number being converted was a hexadecimal and
 * one of its digits was expressed in upper-case.
 *
 * Revision 1.16  1995/11/09  21:56:28  cruz
 * Changed type of variable.
 *
 * Revision 1.15  1995/10/26  22:14:46  cruz
 * Added casting to a several function invocations
 *
 * Revision 1.14  1995/10/22  05:04:58  fdh
 * Use PortsGetChar() instead of GetChar().
 *
 * Revision 1.13  1995/10/18  12:52:46  fdh
 * Added CheckForChar().
 *
 * Revision 1.12  1995/10/11  14:35:50  fdh
 * Added ESC-h.
 *
 * Revision 1.11  1995/10/10  18:02:26  fdh
 * Modified ^R to correctly refresh command line
 * when the current dot location is not at the end
 * of the line.
 *
 * Revision 1.10  1995/10/04  23:34:38  fdh
 * Updated local function prototypes.
 *
 * Revision 1.9  1995/10/03  03:12:41  fdh
 * Removed some obsolete functions.
 *
 * Revision 1.8  1995/09/15  20:21:26  fdh
 * Print prompt before calling get_command() and
 * call Lookup() after get_command() exits.  Also
 * return argc from get_command().
 *
 * Revision 1.7  1995/09/14  19:45:30  fdh
 * Corrected code to delete previous word whereby the
 * index could be stepped out of the cmdline array.
 *
 * Revision 1.6  1995/09/12  22:31:41  fdh
 * Inplemented edit_cmdline() to provide command line editing.
 * Editing commands are based on Emacs commands.  The cursor
 * (arrow) keys are also supported.  Insert/Overstrike mode
 * is toggled with the insert key. Additionally, the Home and
 * End keys are supported when the Debug Monitor has local
 * control of the keyboard.
 *
 * Revision 1.5  1995/09/05  21:19:51  fdh
 * Go back to echoing all characters to console.
 *
 * Revision 1.4  1995/09/05  20:45:41  fdh
 * Init command line buffer.  Disable command line recall
 * beyond first command before recall buffer is filled.
 *
 * Revision 1.3  1995/09/05  18:11:07  fdh
 * Combined command line buffers used in get_cmdline() and MonGetChar().
 * Other general optimization and cleanup.
 *
 * Revision 1.2  1995/09/03  06:53:36  fdh
 * Modified get_cmdline to use history counter.
 * Added FORWARD() and BACKWARD() macros.
 *
 * Revision 1.1  1995/09/03  05:38:43  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "mon.h"
#include "ctype.h"
#include "kbdscan.h"
#include "callback.h"
#include "console.h"

char prevcmd[CMD_HISTORY][LINE_LENGTH+1];
char cmdline[LINE_LENGTH+1];
char *argv[MAX_ARGUMENTS];
ul hexarg[MAX_ARGUMENTS];
ul decarg[MAX_ARGUMENTS];
ui decargok[MAX_ARGUMENTS];
ui hexargok[MAX_ARGUMENTS];
ui argc;
void (*FlushFunction)(void);

static int cmdEol;			/* Always points to End of Line */
static int prefix;
static int dot;

#define FORWARD(x)  ((x+1+CMD_HISTORY) % CMD_HISTORY)
#define BACKWARD(x) ((x-1+CMD_HISTORY) % CMD_HISTORY)

#define ESC 0x100
#define LBK ESC|0x200
#define LB2 LBK|0x300
 
/* Internal prototypes */
static int edit_cmdline(void );
static void donumber(int i);
static void get_cmdline(void );
static int parse_arg_strings(void );
static void cmdline_delete(int offset);

static int edit_cmdline(void)
{
  int idx;
  int c;
  int insertmode;

  insertmode = TRUE;
  prefix = 0;
  dot = cmdEol;
  while (TRUE) {
    c = (*UserGetChar)();

    /* PortsGetChar returns -1 when nothing is read */
    if (c < 0) continue;

    switch (prefix|c) {

    case '\n':			/* ^J */
      UserPutChar((char)c);
      return (c);

    case 27:			/* ESC */
      prefix |= ESC;
      break;

    case ESC|'[':		/* ESC-[ */
      prefix |= LBK;
      break;

    case LBK|'2':		/* ESC-[ */
      prefix |= LB2;
      break;

    case 14:			/* ^N */
    case 16:			/* ^P */
    case 18:			/* ^R */
    case 21:			/* ^U */
    case LBK|'A':		/* Up Arrow */
    case LBK|'B':		/* Down Arrow */
    case UP:
    case DOWN:
      while (dot < cmdEol)	/* Move to End of Line */
	UserPutChar(cmdline[dot++]);
      while (dot)		/* Erase existing line */
	{ printf ("\b \b"); --dot; }
      if (c == 18) {		/* ^R */
	dot = cmdEol;		/* Update dot pointer */
	printf (cmdline);	/* Re-display Command Line */
	break;
      }
      cmdEol = 0;		/* Update End of Line pointer */
      cmdline[0] = 0;		/* Terminate Command Line */
      return (prefix|c);


#if 0				/* This would do VMS style ^U */
    case 21:			/* ^U */
      cmdline_delete(dot);	/* Kill characters before dot */
      break;
#endif

    case ESC|'\b':		/* ESC-backspace */
    case ESC|127:		/* ESC-delete */
    case ESC|'h':		/* ESC-h */
      prefix = 0;
      idx = dot;
      while (idx) {		/* Kill trailing non-alphanumeric */
	if (isalnum((int)cmdline[--idx]))
	  break;
      }
      while (idx) {		/* Kill alphanumeric characters */
	if (!isalnum((int)cmdline[--idx]))
	  break;
      }
      cmdline_delete(dot-idx);
      break;

    case ESC|'b':		/* ESC-b */
      prefix = 0;
      while (dot) {		/* Skip trailing non-alphanumeric */
	if (!isalnum((int)cmdline[dot-1])) {
	  UserPutChar((char)'\b');
	  --dot;
	}
	else break;
      }
      while (dot) {		/* Skip argument characters */
	if (isalnum((int)cmdline[dot-1])) {
	  UserPutChar((char)'\b');
	  --dot;
	}
	else break;
      }
      break;

    case '\b':			/*  ^H    */
    case 127:			/* delete */
      if (dot > 0) cmdline_delete(1);
      break;

    case 1:			/* ^A */
    case HOME:
      while (dot > 0) {		/* To Beginning of Line */
	UserPutChar((char)'\b');	/* Move the cursor */
	--dot;
      }
      break;

    case 2:			/* ^B */
    case LBK|'D':		/* Left Arrow */
    case LEFT:
      prefix = 0;
      if (dot > 0) {		/* Back one character */
	UserPutChar((char)'\b');	/* Move the cursor */
	--dot;
      }
      break;

    case 5:			/* ^E */
    case END:
      while (dot < cmdEol) {	/* To End of Line */
	UserPutChar(cmdline[dot]);	/* Move the cursor */
	++dot;
      }
      break;

    case ESC|'f':		/* ESC-f */
      prefix = 0;
      while (dot < cmdEol) {	/* To End of Line */
	if (!isalnum((int)cmdline[dot])) {
          UserPutChar(cmdline[dot]); /* Move the cursor */
	  ++dot;
	}
	else break;
      }
      while (dot < cmdEol) {	/* To End of Line */
	if (isalnum((int)cmdline[dot])) {
          UserPutChar(cmdline[dot]); /* Move the cursor */
	  ++dot;
	}
	else break;
      }
      break;

    case ESC|'d':		/* ESC-d */
      prefix = 0;
      idx = dot;
      while (idx < cmdEol) {	/* To End of Line */
	if (!isalnum((int)cmdline[idx]))
	  ++idx;
	else break;
      }
      while (idx < cmdEol) {	/* To End of Line */
	if (isalnum((int)cmdline[idx]))
	  ++idx;
	else break;
      }
      cmdline_delete(dot-idx);	/* Note negative offset */
      break;

    case 11:			/* ^K */
      cmdline_delete(dot-cmdEol); /* Note negative offset */
      break;

    case 4:			/* ^D */
    case DEL:
      if (dot < cmdEol)
	cmdline_delete(-1);	/* Note negative offset */
      break;

    case 6:			/* ^F */
    case LBK|'C':		/* Right Arrow */
    case RIGHT:
      prefix = 0;
      if (dot < cmdEol) {	/* Forward one space */
	UserPutChar(cmdline[dot]);	/* Move the cursor */
	++dot;
      }
      break;

    case LB2|'~':		/* Insert */
    case INS:			/* Insert */
      prefix = 0;
      insertmode = insertmode ? FALSE : TRUE;
      break;

   /*
    * "." is a special one.  If it is not used
    * for repeat just pass it to the default case.
    */
    case '.':			/* Repeat */
      if (cmdEol == 0) return (c);
      /* fallthrough */
    default:
      if (prefix != 0) {	/* Ignore unrecognized sequences */
	prefix = 0;
	break;
      }

      if (!(_isprint(c) || _isspace(c))) break;

      UserPutChar((char)c);
      if (cmdEol < LINE_LENGTH) {
	if (insertmode) {	/* Insert character */
	  for (idx=cmdEol; idx>=dot; --idx) 
	    cmdline[idx] = cmdline[idx-1];

	  /* Update dot pointer and End of Line pointer */
	  cmdline[dot++] = c;	/* Insert new character */
	  cmdline[++cmdEol] = 0; /* Terminate Command Line */

	  for (idx=dot; idx<cmdEol; ++idx)
	    UserPutChar(cmdline[idx]); /* Output characters from dot */

	  for (idx=dot; idx<cmdEol; ++idx)
	    UserPutChar((char)'\b');	/* Move cursor back to dot */
	}
	else {			/* Over-strike character */
	  /* Update dot pointer and End of Line pointer */
	  cmdline[dot++] = c;	/* Place new character */

	  if (dot > cmdEol) {
	    cmdEol = dot;
	    cmdline[cmdEol] = 0; /* Terminate Command Line */
	  }
	}
      }
      else {
	cmdline[0] = 0;
	printf("\nLine exceeds %d characters. That's too long\n", LINE_LENGTH);
	return -1;
      }
    }
  }
}

static void get_cmdline(void)
{
  int ch;
  int count;
  int history = CMD_HISTORY;
  static int idx;
  int prev_idx;

  /* Initialize Argument Arrays */
  for (count = 0; count < MAX_ARGUMENTS; count += 1)
  {
    hexargok[count] = FALSE;
    decargok[count] = FALSE;
    hexarg[count] = 0;
    decarg[count] = 0;
    argv[count] = 0;
  }

  /* Initialize command line buffer */
  cmdEol = 0;
  cmdline[0] = 0;

  prev_idx = idx = idx % CMD_HISTORY;	/* Make sure it's within range */
  while (TRUE) {
    ch = edit_cmdline();
    switch(ch) {

    case 14:			/* Control-N */
    case LBK|'B':		/* Down Arrow */
    case DOWN:
      if (history != CMD_HISTORY) {
	prev_idx = FORWARD(prev_idx);
	if (++history != CMD_HISTORY)
	  strcpy(cmdline, prevcmd[prev_idx]); /* Restore previous command */
	printf(cmdline);
	cmdEol = strlen(cmdline);
      }
      break;

    case 16:			/* Control-P */
    case LBK|'A':		/* Up Arrow */
    case UP:
      if (history > 0) {
	if (prevcmd[BACKWARD(prev_idx)][0] != 0) {
	  prev_idx = BACKWARD(prev_idx);
	  --history;
	}
      }
      strcpy(cmdline, prevcmd[prev_idx]); /* Restore previous command */
      printf(cmdline);
      cmdEol = strlen(cmdline);
      break;

    case '.':			/* Repeat */
	strcpy(cmdline, prevcmd[BACKWARD(idx)]); /* Restore previous command */
	printf("%s\n", cmdline);
	return;

    case '\n':			/* Return */
      if (cmdEol) {		/* If it's not an empty command line */
	strcpy(prevcmd[idx], cmdline); /* Save as previous command */
	idx = FORWARD(idx);
      }
      return;

    case -1:
      return;
    }
    prefix = 0;
  }
}

static int parse_arg_strings(void)
{
  char *s = cmdline;
  int matched;
  int args = 0;
  while (args < MAX_ARGUMENTS)
  {
    while (isspace((int)*s)) s += 1;	/* skip leading spaces */
    if (*s == '\0') break;		/* Check for end of command string */
    if (*s == '"') {
      matched = FALSE;
      ++s;			/* Skip over beginning quote */
      argv[args++] = s;		/* store beginning of string */
      while (*s != '\0') {		/*  skip argument */
	if (*s == '"') {	/* Check for end of arg string */
	  *s++ = '\0';		/* Terminate argument string */
	  matched = TRUE;
	  break;
	}
	++s;
      }
      if (!matched) {
	printf("Unmatched \".\n");
	return -1;
      }
    }
    else {
      argv[args++] = s;		/* store beginning of string */
      while (*s != '\0') {		/*  skip argument */
	if (isspace((int)*s)) {	/* Check for end of arg string */
	  *s++ = '\0';		/* Terminate argument string */
	  break;
	}
	++s;
      }
    }
  }
  return args;
}

static void cmdline_delete(int offset)
{
  int idx;

  if (offset > 0) {
    dot -= offset;		/* Update dot pointer */
    idx = offset;
    while (idx--) UserPutChar((char)'\b'); /* Move cursor to new dot */
  }
  else offset = 0 - offset;	/* Normalize offset */

  for (idx=dot+offset; idx<cmdEol; ++idx) {
    cmdline[idx-offset] = cmdline[idx];
    UserPutChar(cmdline[idx-offset]); /* Output characters from dot */
  }

  idx = offset;
  while (idx--) UserPutChar((char)' ');	/* Erase trailing characters */

  idx = cmdEol-dot;
  while(idx--) UserPutChar((char)'\b');	/* Move cursor back to dot */

  cmdEol -= offset;		/* Update End of Line pointer */
  cmdline[cmdEol] = 0;		/* Terminate Command Line */
}

static void donumber (int i)
{
  char *s, c = '\0';
  int value;

  s = argv[i];
  hexarg[i] = 0;
  decarg[i] = 0;
  hexargok[i] = TRUE;
  decargok[i] = TRUE;

  while (*s)
  {
    if((s[0] == '0') &&
       ((s[1] == 'x') ||  (s[1] == 'X'))) {
      decargok[i] = FALSE;
      s += 2;			/* Skip over 0x */
    }

    c = tolower((int) *s++);    
    if (_isdigit(c)) value = c - '0';
    else {
      decargok[i] = FALSE;
      if (_isxdigit(c)) value = c - 'a' + 10;
      else hexargok[i] = FALSE;
    }
    if (decargok[i]) decarg[i] = (decarg[i] * 10) + value;
    if (hexargok[i]) hexarg[i] = (hexarg[i] * 16) + value;
  }

  if ((decargok[i] == FALSE) && (hexargok[i] == TRUE)) {
    decarg[i] = hexarg[i];
    decargok[i] = TRUE;
  }
}

int get_command(void)
{
  ui i;

  get_cmdline();

#ifdef PRTRACE
  if (*cmdline != 0) {
    i = 0;
    while (cmdline[i] != 0)
      printf("  %c", cmdline[i++]);
    printf("\n");

    i = 0;
    while (cmdline[i] != 0)
      printf(" %2x", cmdline[i++]);
    printf("\n");
  }
#endif

  argc = parse_arg_strings();

  /* now the command line is split into strings, with argv[]
    pointing to them */
  for (i = 0; i < argc; i += 1) donumber(i);

  return(argc);
}

int kbdcontinue(void)
{
  int in;

  printf("\n\t Hit any key to continue. Control-C to quit...");
  // Spin on UserGetChar until an actual character is hit
  // Broadcast and SROM do not wait indefinately
  while( (in = UserGetChar()) < 0 );
  printf("\n\n");
  if (in == 3) return(FALSE);
  return(TRUE);
}

BOOLEAN CheckForChar(int ch)
{
  if (inputport == SROM )	/* Disable polling of SROM Serial port */
    return FALSE;

  if (CharAv())			/* Was a key pressed? */
    return (GetChar() == ch);	/* Does it match? */
  return FALSE;
}
