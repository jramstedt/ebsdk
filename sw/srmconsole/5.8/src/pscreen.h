#ifndef _pscreen_
#define _pscreen_
/* file:	pscreen.h
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
 *
 *
 * Abstract:	Definitions and constants for the powerup screen
 *
 * Author:	AJ Beaverson
 *
 * Modifications:
 */

/*
 * Constants for the status fields that pscreen looks at
 */
#define PSCREEN_NOTYET		0	/* haven't looked at the device yet */
#define PSCREEN_INPROGRESS	1
#define PSCREEN_PASS		2
#define PSCREEN_FAIL            3
#define PSCREEN_ABSENT          4

#define SCREEN_ROWS	24
#define SCREEN_COLS	(80 + 10) /* allow for escape sequences that	*/ /* ajbfix */
				  /* don't take up character cells	*/
#define	MAX_FIELDSIZE	48

/*
 * A structure that describes fields on a screen
 */
typedef struct {
	char	row;
	char	col;
	char	allow_change;	/* allow field to contribute to change counts	*/
	char	flags;		/* static, selectable etc		*/
	char	*text;		/* what should be on the screen		*/
	char	*screen;	/* what is currently on the screen	*/
} FORM;

#define FIELD_STATIC		1
#define FIELD_SELECTABLE	2
#define FIELD_ALLOWCHANGE	4	/* not implemented yet */

/*
 * The generic structure for a screen.  The order of the cells in the structure
 * is roughly analogous to the order in which they are used by show_screen.
 */
typedef struct {
	char 	*name;		/* name of the screen				*/
	char	*lds;		/* line drawing characters when in video mode	*/
	int	(*poll) ();	/* polls structures and updates forms		*/
	int	nf;		/* number of fields				*/
	FORM	*fields;	/* list of fields that were updated		*/
	int	(*done) ();	/* returns true when done			*/
	int	loopdelay;     	/* amount of time to delay between updates	*/
	int	(*cleanup) ();	
	int	nrows;		/* number of non scrolling rows in screen	*/
} SCREEN;


#define	QS_POWERUP	0	/* set if invoked from powerup */
#define QS_INTERACTIVE	1
#define	QS_CONTINUOUS	2
#define QS_HARDCOPY	3
#define QS_VIDEO	4
#define QS_SWITCH	5
#define	QS_MAX		6
#define QS_STRING "powerup interactive continuous hardcopy video switch"

#endif
