/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:33:34 by OpenVMS SDL EV1-33     */
/* Source:  25-JAN-1991 16:59:53 AFW_USER$:[CONSOLE.V58.COMMON.SRC]PD_DEF.SDL;1 */
/********************************************************************************************************************************/
/* file:	pd_def.sdl                                                  */
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
/* Abstract:	Definitions for Procedure Descrioptors                      */
/*                                                                          */
/* Author:	AJ Beaverson, Stephen Shirron                               */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*** MODULE pd_def ***/
struct PDLIST {
    int pd;
    char *name;
    } ;
 
