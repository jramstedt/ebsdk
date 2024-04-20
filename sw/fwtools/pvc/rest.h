/* rest.h */

/*
 *	
 *	Copyright (c) 1992
 *	Digital Equipment Corporation, Maynard, Massachusetts 01754
 *	
 *	This software is furnished under a license for use only on a single computer
 *	system  and  may be  copied  only  with the inclusion of the above copyright
 *	notice.  This software, or  any other copies thereof, may not be provided or
 *	otherwise made  available  to any other person except for use on such system
 *	and to one who agrees to these license terms.  Title to and ownership of the
 *	software shall at all times remain in DEC.
 *	
 *	The information  in  this software  is  subject to change without notice and
 *	should not be construed as a commitment by Digital Equipment Corporation.
 *	
 *	DEC  assumes no responsibility for the use or reliability of its software on
 *	equipment which is not supplied by DEC.
 *	
 *	
 *	REVISION HISTORY:		
 *	
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *	  0     gpc     AUG-92          First Created
 *	
 */

#ifndef REST_H
#define REST_H 1

int not_ignored (int, int);
int new_warning (int, int);
int new_error (int, int);
void remove_old_caveats (List *);
void add_caveat (int, List *_list);
ERR_MSG *search_caveat (int, List *_list, INSTR *);
void add_restriction (int, int, char *, char *);
void clean_restrictions (void);

#endif
