/* body.h */

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

#ifndef BODY_H
#define BODY_H 1

#define MAXFLOW 5000
#define MAXRUNCYCLES 10000

/* 
** this macro removes trailing white space from a character string
*/
#ifndef chop_space
#define chop_space(str) {char *p=str;while(*p!='\0')p++;while(isspace(*--p))*p='\0';}
#endif

#ifndef s_upper
#define s_upper(str) {char *p;for(p=str;(*p=toupper(*p))!='\0';p++);}
#endif


int palcode_violation_checker (List *, char *, char *);	 /* V3.25 */
void loop_on_entry_list (List *, int, char *);
/* int get_exe_file (char *, char *); renamed get_exe_map_files V3.25 */
int get_exe_map_files (char *, char *);			/* V3.25 */
void print_cycle (int, int);
int open_map_file (char *);
int read_map_file (FILE *);
void process_goto_information (List *);
void found_pvc_label (char *ptr, List *computed_goto);
void free_exe (int len);

#endif
