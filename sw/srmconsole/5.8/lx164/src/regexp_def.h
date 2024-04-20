/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:27:59 by OpenVMS SDL EV1-33     */
/* Source:  25-OCT-1991 15:58:48 AFW_USER$:[CONSOLE.V58.COMMON.SRC]REGEXP_DEF.SD */
/********************************************************************************************************************************/
/* file:	regexp_def.sdl                                              */
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
/* Abstract:	Data structures for regular expressions.                    */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	ajb	19-Apr-1991	Remove expr field.  This wasn't being used, */
/*				and the extra malloc/free was wasting time. */
/*                                                                          */
/*	ajb	09-May-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE regexp_def ***/
#define BOL 1                           /* ascii SOH                        */
#define EOL 10                          /* ascii LF                         */
/*+                                                                         */
/* ============================================================================ */
/* = TOKEN - token                                                            = */
/* ============================================================================ */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*   Describes a token.                                                     */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*:                                                                         */
/*      ---------------------------------------------------------------------- */
/*      Structure Members  | Data Type  | Description                       */
/*      ---------------------------------------------------------------------- */
/*      chmask             | longword   | bitmask of MCS characters in token */
/*                         |            |                                   */
/*      minimum            | longword   | minimum # of characters allowed   */
/*                         |            |                                   */
/*      maximum            | longword   | maximum # of characters allowed   */
/*                         |            |                                   */
/*      cur                | longword   | current # characters in effect and */
/*                         |            | were eventually matched           */
/*:     ---------------------------------------------------------------------- */
/*                                                                          */
/*--                                                                        */
struct TOKEN {
    unsigned int chmask [8];            /* bitmask of MCS characters in token */
    int minimum;                        /* minimum # of characters allowed  */
    int maximum;                        /* maximum # of characters allowed  */
    int cur;                            /* current # characters in effect and */
/* were eventually matched                                                  */
    } ;
/*+                                                                         */
/* ============================================================================ */
/* = REGEXP - regular expression                                              = */
/* ============================================================================ */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*   The internal representation of a regular expression.                   */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*:                                                                         */
/*      ---------------------------------------------------------------------- */
/*      Structure Members  | Data Type  | Description                       */
/*      ---------------------------------------------------------------------- */
/*      tlist              | address    | pointer to tokens                 */
/*                         |            |                                   */
/*      tcount             | longword   | number of tokens in above list    */
/*:     ---------------------------------------------------------------------- */
/*                                                                          */
/*--                                                                        */
struct REGEXP {
    int tagbol;                         /* anchor to begining of line       */
    int tageol;                         /* anchor to end of line            */
    struct TOKEN *tlist;                /* pointer to tokens                */
    int tcount;                         /* number of tokens in above list   */
    } ;
 
