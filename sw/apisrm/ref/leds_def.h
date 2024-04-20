/*****************************************************************************/
/* Created:  8-JAN-1997 03:25:04 by OpenVMS SDL EV1-31 			    */
/* Source:   5-JUN-1992 14:35:26 AFW5USER1:[CONSOLE.REF]LEDS_DEF.SDL;1      */
/****************************************************************************/
/* file:	leds_def.sdl                                                */
/*                                                                          */
/* Copyright (C) 1991, 1992 by                                              */
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
/* Abstract:	Cobra LED control and iic bus EEROM data structure definitions. */
/*                                                                          */
/* Author:	David Baird                                                 */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	dwb	3-Mar-1992	Initial entry.                              */
/*                                                                          */
/*	dwb	5-Jun-1992	Correct LED bit Postions for FBUS and IO    */
/*                                                                          */
/*** MODULE $leds_def ***/
/* Set/Clear LED routine bit masks.                                         */
#define LEDS$M_CPU1 1
#define LEDS$M_CPU2 2
#define LEDS$M_MEM0 4
#define LEDS$M_MEM1 8
#define LEDS$M_MEM2 16
#define LEDS$M_MEM3 32
#define LEDS$M_FBUS 64
#define LEDS$M_IO 128
