/* file:	qscan_def.h                                                  */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*                                                                          */

#ifndef QSCAN_DEF
#define QSCAN_DEF
 
struct QSTRUCT {
    char id [32];
    int present;
    int val_type;
    union  {                            /* union of values                  */
        char *string;
        int integer;
        } value;
    } ;
#endif

int qscan (
	int *argc,
	char *argv [],
	char *qintro,
	char *flist,
	struct QSTRUCT	*qvb) ;

