/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:49 by OpenVMS SDL EV1-31     */
/* Source:   9-OCT-1996 15:50:47 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]APC.SDL;1 */
/********************************************************************************************************************************/
/* file: APC.SDL                                                            */
/*                                                                          */
/* Copyright (C) 1993, 1996 by                                              */
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
/* Abstract:	AlphaPC platform common defintions.                         */
/*                                                                          */
/* Author: 	Eric Goehl                                                  */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	egg	01-Aug-1996	Initial port taken from EB164.SDL           */
/*** MODULE $apccommon ***/
#define APC$PRIMARY_CPU 0
/*                                                                          */
/*  Definitions for all Evaluation Board and AlphaPC motherboard            */
/*  platforms supported by Digital Semiconductor.                           */
/*                                                                          */
#define PLATFORM_EB64 0
#define PLATFORM_EB64P 1
#define PLATFORM_PC64 2
#define PLATFORM_PCI64 3
#define PLATFORM_EB66 4
#define PLATFORM_EB66P 5
#define PLATFORM_EB164 6
#define PLATFORM_PC164 7
#define PLATFORM_PC264 8
#define PLATFORM_LAST 9
/*                                                                          */
/* Common firmware revision numbers used on AlphaPC platforms.              */
/*                                                                          */
#define FW_BL_MAJOR 4                   /* major firmware revision          */
#define FW_BL_MINOR 5                   /* minor revision                   */
#define APC_PLATFORM 1
 
