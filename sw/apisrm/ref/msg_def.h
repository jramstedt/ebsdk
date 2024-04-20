/********************************************************************************************************************************/
/* Created 12-DEC-1991 13:11:03 by VAX SDL V3.2-12     Source: 19-NOV-1991 13:22:43 ALPHA_FW:[COBRA_FW.REF]MSG_DEF.SDL;6 */
/********************************************************************************************************************************/
/* file:	msg_def.sdl                                                 */
/*                                                                          */
/* Copyright (C) 1991 by                                                    */
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
/*                                                                          */
/*  FACILITY:                                                               */
/*                                                                          */
/*      Alpha Firmware                                                      */
/*                                                                          */
/*  MODULE DESCRIPTION:                                                     */
/*                                                                          */
/*      Message number definitions.                                         */
/*                                                                          */
/*  AUTHORS:                                                                */
/*                                                                          */
/*      ajb - AJ Beaverson                                                  */
/*	jds - Jim Sawin                                                     */
/*	kp - Kevin Peterson                                                 */
/*	tga - Tom Arnold                                                    */
/*                                                                          */
/*  CREATION DATE:                                                          */
/*                                                                          */
/*      17-Jul-1990                                                         */
/*                                                                          */
/*  MODIFICATION HISTORY:                                                   */
/*                                                                          */
/*	phk	19-Nov-1991	Added remote boot msg                       */
/*                                                                          */
/*	phk	15-Nov-1991	Added msg_fe_too_big                        */
/*                                                                          */
/*	phk	10-Oct-1991	Added msg_file_not_found                    */
/*                                                                          */
/*	jds	16-Aug-1991	Added msg_ex_* (XNA) messages.              */
/*                                                                          */
/*	tga	13-Aug-1991	Add msg_pkz_*                               */
/*                                                                          */
/* 	twp	28-Jun-1991	Add specific error callout for environment  */
/*				variable D_GROUP error                      */
/*                                                                          */
/*	cto	21-Jun-1991	Add a few exception messages                */
/*                                                                          */
/*	pel	31-May-1991	Add prompt to proceed w/ destructive test.  */
/*                                                                          */
/*	phk	29-May-1991	Add fbe csr_driver messages                 */
/*                                                                          */
/*	hcb	23-May-1991	Add memtest messages                        */
/*                                                                          */
/*	phk	22-May-1991	Add fbe dma_gen messages                    */
/*                                                                          */
/*	pel	20-May-1991	Add msg_show_iob_hdr1/2	                    */
/*                                                                          */
/*	pel	09-May-1991	Add msg_insuff_mem                          */
/*                                                                          */
/*	pel	06-May-1991	Add msg_exer_counts, msg_bad_action         */
/*                                                                          */
/*	prw	06-May-1991	Add in diagnostic messages                  */
/*                                                                          */
/*	cto	03-May-1991	Add MP messages                             */
/*                                                                          */
/*	hcb	25-Apr-1991	Remove messages for memtest - use diag messages */
/*                                                                          */
/*	cto 	25-Apr-1991	Add error messages for diags template       */
/*                                                                          */
/*	hcb	23-Apr-1991	Add error messages for memtest              */
/*                                                                          */
/*	twp	15-Apr-1991	Add error messages resulting from bad       */
/*                        	search of IOB for specific device name      */
/*                                                                          */
/*	jad	11-Apr-1991	Add some RC messages.                       */
/*                                                                          */
/*	dtm	28-Mar-1991	Add halt code messages                      */
/*                                                                          */
/*	jds	28-Mar-1991	Changed gpr out of range to generic out of range. */
/*                                                                          */
/*	jad	19-Mar-1991	Added REPLY:                                */
/*                                                                          */
/*	jds	14-Mar-1991	Added new ipr_driver message codes.         */
/*                                                                          */
/*	jds	21-Feb-1991	Added exdep -s switch error message.        */
/*                                                                          */
/* 	jds	13-Feb-1991	Added vmem message codes                    */
/*                                                                          */
/*	mrc	05-Feb-1991	modified diagnostic support messages        */
/*                                                                          */
/*	ajb	31-Jan-1991	add msg_radix                               */
/*                                                                          */
/*	mrc	14-Jan-1991	add diagnostic support messages             */
/*                                                                          */
/*	pel	08-Jan-1991	add msg_ambig_file, msg_not_removed         */
/*                                                                          */
/*	dtm	20-Dec-1990	add extra_params, insuff_params, qual_conflict */
/*                                                                          */
/*	pel	19-Nov-1990	add msg_bad_open                            */
/*                                                                          */
/*	pel	13-Nov-1990	add shell_noex,_nocmd,_nopipe               */
/*                                                                          */
/*	pel	05-Nov-1990	add file_full msg.                          */
/*                                                                          */
/*	pel	01-Nov-1990	add file_in_use msg.                        */
/*                                                                          */
/*	pel	22-Oct-1990	Add more file system messages.              */
/*                                                                          */
/*	pel	19-Oct-1990	Add rd/pi_validate failure message codes.   */
/*                                                                          */
/*	dtm	15-Oct-1990	Add msg_ev_badvalue                         */
/*                                                                          */
/*	sfs	28-Sep-1990	Add MSCP, DUP, and SCSI message codes.      */
/*                                                                          */
/*	pel	25-Sep-1990	Start error message numbers at ERR_MSG & assign */
/*                              it a value of hex 8f00 0000.                */
/*                              Keep msg_success equal to 0.                */
/*                                                                          */
/*	ajb	25-Sep-1990	Add message number for expression evaluator */
/*                                                                          */
/*	dtm	24-Sep-1990	add environment variable message codes      */
/*                                                                          */
/*	pel	Sep 19 1990	bad_filename, bad_devicename                */
/*                                                                          */
/*	cfc	Sep 14 1990	qscan_nomatch                               */
/*                                                                          */
/*	jds	Sep 12 1990	Added examine/deposit error messages.       */
/*                                                                          */
/*	kp	24-Jul-1990	Add messages for regular expressions.       */
/*                                                                          */
/*	ajb	20-Jul-1990	Add message structure and language constants. */
/*                                                                          */
/*                                                                          */
 
/*** MODULE msg_def ***/
/*+                                                                         */
/* ============================================================================ */
/* = MESSAGE - Associates message numbers with text strings.                  = */
/* ============================================================================ */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*      Each message number is associated with a language dependent string. */
/*	A number is constructed by appending the current language and message */
/*	numbers together.  This number is then looked for in the message table, */
/*	which is an array of these structures.  If found, then the string number */
/*	represents the string associated with the message in the current    */
/*	language.                                                           */
/*                                                                          */
/*	Not all messages have translations into every language, so there exists */
/*	a wildcard language which matches any language.  Typically, English */
/*	messages have as a language specifier the wildcard language, so that */
/*	when a message is printed out, English will automatically come out if */
/*	there is no valid translation.                                      */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*:                                                                         */
/*       ------------------------------------------------------------------- */
/*       Structure Members    | Data Type | Description                     */
/*       ------------------------------------------------------------------- */
/*       lang                 | bitfield  | language number, one of LANG_*  */
/*                            |           |                                 */
/*       msgnum               | bitfield  | message number, one of msg_*    */
/*                            |           |                                 */
/*       strnum               | bitfield  | String number.  This is a value */
/*                            |           | that is used to find the actual */
/*                            |           | string in a string space.       */
/*:      ------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
struct MESSAGE {
    long int msgnum;
    unsigned lang : 8;
    unsigned strnum : 24;
    } ;
/*+                                                                         */
/* ============================================================================ */
/* = MSGCTX - context structure for walking through a message string          = */
/* ============================================================================ */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*      This data structure defines a context so that routines may walk through */
/*	message strings a character at a time.  Routines that use the message */
/*	facility may not always have enough room to expand the entire message */
/*	in place, and may not even want to (their algorithms may require    */
/*	walking the string a character at a time).                          */
/*                                                                          */
/*	Since we want to hide how strings are stored, we have to provide a  */
/*	procedure interface that walks over the strings, decompressing on the */
/*	fly.  The message facility is reentrant, so context must be maintained */
/*	in the caller's context.  This structure defines that context.      */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*:                                                                         */
/*       ------------------------------------------------------------------- */
/*       Structure Members | Data Type | Description                        */
/*       ------------------------------------------------------------------- */
/*       mp                |  MESSAGE  | pointer to orginal message structure */
/*                         |           |                                    */
/*       chidx             |  integer  | character index into string        */
/*:      ------------------------------------------------------------------- */
/*                                                                          */
/*-                                                                         */
struct MSGCTX {
    struct MESSAGE *mp;                 /* pointer to original message      */
    long int chidx;                     /* character index into string      */
    } ;
/*                                                                          */
/* Language codes, used in the message data structure.                      */
/*                                                                          */
#define LANG_UNKNOWN 0                  /* Unknown language.                */
#define LANG_DANISH 1
#define LANG_GERMAN 2
#define LANG_ENGLISH 3
#define LANG_SPANISH 4
#define LANG_FRENCH 5
#define LANG_ITALIAN 6
#define LANG_DUTCH 7
#define LANG_NORWEGIAN 8
#define LANG_PORTUGUESE 9
#define LANG_FINNISH 10
#define LANG_SWEDISH 11
#define LANG_ALL 12                     /* "Wildcard" language.             */
/*                                                                          */
/* Keyboard codes.                                                          */
/*                                                                          */
#define LK201_AA 1                      /* American			    */
#define LK201_AB 2                      /* Flemish			    */
#define LK201_AC 3                      /* Canadian (French)		    */
#define LK201_AD 4                      /* Danish			    */
#define LK201_AE 5                      /* British			    */
#define LK201_AF 6                      /* Finnish			    */
#define LK201_AG 7                      /* Austrian/German		    */
#define LK201_AH 8                      /* Dutch			    */
#define LK201_AI 9                      /* Italian			    */
#define LK201_AK 10                     /* Swiss (French)		    */
#define LK201_AL 11                     /* Swiss (German)		    */
#define LK201_AM 12                     /* Swedish			    */
#define LK201_AN 13                     /* Norwegian			    */
#define LK201_AP 14                     /* Belgian/French		    */
#define LK201_AS 15                     /* Spanish			    */
#define LK201_ZZ 16                     /* Portuguese  (TEMPORARY)	    */

#include "cp$src:msgnum.h"

