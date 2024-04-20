/********************************************************************************************************************************/
/* file:	display.h                                                       */
/*                                                                          */
/* Copyright (C) 1993 by                                                    */
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
/* Facility:	EVAX/Cobra firmware                                         */
/*                                                                          */
/* Abstract:	This file describes constants used by display functions     */
/*                                                                          */
/* Author:	J. A. DeNisco                                                   */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	jad	24-Aug-1993	Initial entry.                                          */
/*                                                                          */
/*	rps	12-oct-1995	update display size for BURNS		    */
/*									    */
/*	dpm	13-oct-1995	Changed #ifdef BURNS to #if BURNS	   */
/*                                                                          */
/********************************************************************************************************************************/

#define ON 1
#define OFF 0
#define BACKGROUND_COLOR 0x04           /* Magenta.                         */
#define FOREGROUND_COLOR 0x0f		/* White.                           */
#define BLINK_ATTRIBUTE 0x80
#define NORMAL_VIDEO  (((BACKGROUND_COLOR<<4)|FOREGROUND_COLOR)&0xff)
#define REVERSE_VIDEO (((FOREGROUND_COLOR<<4)|BACKGROUND_COLOR)&0xff)
#define MAX_COL 80
#define MAX_ROW 25

