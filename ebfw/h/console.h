#ifndef __CONSOLE_H_LOADED
#define __CONSOLE_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
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

/*
 *  $Id: console.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 *
 * $Log: console.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.4  1996/05/22  21:32:05  fdh
 * Added a couple of definitions.
 *
 * Revision 1.3  1995/12/08  18:10:21  cruz
 * Increased the history buffer from 10 to 20.
 *
 * Revision 1.2  1995/10/18  12:52:03  fdh
 * Added CheckForChar() prototype.
 *
 * Revision 1.1  1995/10/03  03:37:38  fdh
 * Initial revision
 *
 */

#define MAX_ARGUMENTS   10
#define LINE_LENGTH     127
#define CMD_HISTORY     20  /* max size of history buffer */

extern char prevcmd[][LINE_LENGTH+1];
extern char cmdline[];
extern char *argv[];
extern ul hexarg[];
extern ul decarg[];
extern ui decargok[];
extern ui hexargok[];
extern ui argc;

extern int get_command(void);
extern int kbdcontinue(void );
extern BOOLEAN CheckForChar(int ch);

extern void (*FlushFunction)(void);

extern int PortSelected;	/* Use all ports until the user selects one. */

#endif /* __CONSOLE_H_LOADED */
