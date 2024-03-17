/* pvc.h */

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

#ifndef PVC_H
#define PVC_H 1

#define s_upper(str) {char *p;for(p=str;(*p=toupper(*p))!='\0';p++);}
#define chop_space(str) {char *p=str;while(*p!='\0')p++;while(isspace(*--p))*p='\0';}

void close_files();
void show_flags();
void show_palbase();
void show_files(char *, char *, char *, char *, char *); 
void clear_flag(char *);
void set_file(char *, char *);
void set_cpu (char *,int ); 
void set_flag(char *); 
int load_entries( char *, List *); 
int wr (int, char *, ...); 
void handle_int (int, ...); 

#endif
