/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:04:57 by OpenVMS SDL EV1-31     */
/* Source:   8-APR-1991 13:31:03 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]ALPHA_DEF.SD */
/********************************************************************************************************************************/
/* file:	alpha_def.sdl                                               */
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
/* Abstract:	Alpha specific data types and limits.                       */
/*                                                                          */
/* Author:	JDS - Jim Sawin                                             */
/*                                                                          */
/* Revision History:                                                        */
/*                                                                          */
/*  4	by JDS, Mon Apr  8 13:30:46 1991                                    */
/* 	Added RAZ_GPR, RAZ_FPR.                                             */
/*                                                                          */
/*  3	by JDS, Thu Mar 28 13:39:19 1991                                    */
/*	Added MAX_PT.                                                       */
/*                                                                          */
/*  2	by JDS, Thu Mar 14 13:47:40 1991                                    */
/*	Removed MAX_IPR (now in aprdef.sdl) and PS_REG (not needed).        */
/*                                                                          */
/*  1	by JDS, Wed Mar  6 16:11:12 1991                                    */
/*	Initial entry.                                                      */
/*                                                                          */
/*** MODULE alpha_def ***/
#define SP_REG 30                       /* SP is R30                        */
#define RAZ_GPR 31                      /* This GPR is Read-As-Zero         */
#define RAZ_FPR 31                      /* This FPR is Read-As-Zero         */
#define MAX_GPR 31                      /* Max GPR number                   */
#define MAX_FPR 31                      /* Max FPR number                   */
#define MAX_PT 31                       /* Max PALtemp number               */
/* item ADDRESS quadword unsigned typedef;		/* Data type for an address */
typedef unsigned int ADDRESS;           /* Data type for an address         */
typedef unsigned char REGNUM;           /* Data type for a register number  */
typedef unsigned int REGDATA [2];       /* Data type for a register         */
 
