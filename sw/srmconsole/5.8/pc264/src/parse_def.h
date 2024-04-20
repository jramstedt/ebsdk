/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:01 by OpenVMS SDL EV1-31     */
/* Source:   9-MAY-1991 15:50:12 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]PARSE_DEF.SD */
/********************************************************************************************************************************/
/* file:	parse_def.sdl                                               */
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
/* Abstract:	This file describes all the data structures and constants   */
/*		that LEX and YACC fill in after a line has been parsed.  The */
/*		parser data structures are designed to be generous in the   */
/*		number of parameters, qualifers, qualifier values etc. that */
/*	 	could be present.  While memory has not been needlessly     */
/*		wasted, conservation of memory is not a primary goal.       */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	ajb	09-May-1991	Archive structures not needed in the Bourne */
/*				shell.                                      */
/*                                                                          */
/*	ajb	06-Mar-1991	Don't copy qualifier strings into qstruct, but */
/*				copy the pointer only.                      */
/*                                                                          */
/*	cfc	04-Mar-1991	add TOK$K_TEXT                              */
/*                                                                          */
/*	cfc	27-Jan-1991	add {}                                      */
/*                                                                          */
/*	cfc	12-Jan-1991	add TOK$K_EQUALS; cpu field to PNODE.       */
/*                                                                          */
/*	cfc	07-Jan-1991	add TOK$K_CPUSPEC; cpu field to PNODE.      */
/*				Explicitly state TOK$K_ token defs.         */
/*				This is because we no longer extract        */
/*				the defs to make %tokens from them for      */
/*				Grammar.y.  Grammar.y needs %left affinities */
/*				for its tokens now:  When changing any      */
/*				TOK$K terminal tokens in grammar.y, be      */
/*				sure to define them in parse_def.sdl as well. */
/*				Added flag fg to PARSE to indicate foreground */
/*				background mode.  This replaces explicit    */
/*				passing of foreground flag to shell_execute. */
/*                                                                          */
/*	cfc	09-Dec-1990	add TOK$K_CPUSPEC; cpu field to PNODE.      */
/*	cfc	05-Dec-1990	add value field, and type to qstruct        */
/*                                                                          */
/*	pel	05-Dec-1990	add MAX_PATHNAME, for PARSE std filenames.    */
/*                                                                          */
/*	pel	29-Nov-1990	increase size of PARSE std file name arrays. */
/*                                                                          */
/*	ajb	06-Nov-1990	Add new token for shell parameters          */
/*                                                                          */
/*	cfc	27-Sep-1990	Make argv dynamic...**argv.                 */
/*                                                                          */
/*	ajb	24-Sep-1990	Make QSTRUCT and SYMBOl interchangeable     */
/*                                                                          */
/*	ajb	09-Aug-1990	Redundant definitions                       */
/*                                                                          */
/*	ajb	29-Jun-1990	Add structure for parsing qualifiers        */
/*                                                                          */
/*	ajb	18-May-1990	Add in parse tree for shell operators       */
/*                                                                          */
/*	ajb	09-Feb-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE parse ***/
#define MAX_NAME 32                     /* max length of any identifier     */
/*                                                                          */
/* The following structures are used by the routine qscan which picks off   */
/* Unix style qualifiers from a command line.                               */
/*                                                                          */
/*+                                                                         */
/* ===============================================                          */
/* = QSTRUCT - A command qualifier               =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*   Describes a qualifier in a command line.                               */
/*                                                                          */
/* STRUCTURE DESCRIPTION:                                                   */
/*                                                                          */
/*   If present on a command line, a qualifier is put in this structure.    */
/*   To get multiple qualifiers, make an array of this structure.           */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*                                                                          */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*	              present |   longword | qualifier was present.         */
/*	                      |	           |                                */
/*	              value   |   address  | The address of the value of the qualifier. */
/*	                      |	           |                                */
/*	              id      |	  byte arr | the qualifier.                 */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*--                                                                        */
struct QSTRUCT {
    char id [32];
    int present;
    int val_type;
    union  {                            /* union of values                  */
        char *string;
        int integer;
        } value;
    } ;
struct SYMBOL {
    char mne [32];
    int v0;
    int v1;
    } ;
 
