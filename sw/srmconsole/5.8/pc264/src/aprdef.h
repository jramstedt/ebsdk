/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:04:57 by OpenVMS SDL EV1-31     */
/* Source:  28-JAN-2000 16:24:09 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]APRDEF.SDL;1 */
/********************************************************************************************************************************/
/* file:	aprdef.sdl                                                  */
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
/* Abstract:	Alpha Internal Processor Register IDs.                      */
/*                                                                          */
/* Author:	JDS - Jim Sawin                                             */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/* 3	by ajb 14-Jan-1992 Added missing KSP definition                     */
/*                                                                          */
/* 2	by HCB, 11-4-91 Added EV ABOX and IBOX IPRs.                        */
/*                                                                          */
/* 1	by JDS, Mon Mar 11 10:14:52 1991                                    */
/*	Initial entry.                                                      */
/*** MODULE $aprdef ***/
#define APR$K_ASN 0
#define APR$K_ASTEN 1
#define APR$K_ASTSR 2
#define APR$K_FEN 3
#define APR$K_IPIR 4
#define APR$K_IPL 5
#define APR$K_MCES 6
#define APR$K_PCBB 7
#define APR$K_PRBR 8
#define APR$K_PTBR 9
#define APR$K_SCBB 10
#define APR$K_SIRR 11
#define APR$K_SISR 12
#define APR$K_TBCHK 13
#define APR$K_TBIA 14
#define APR$K_TBIAP 15
#define APR$K_TBIS 16
#define APR$K_KSP 17
#define APR$K_ESP 18
#define APR$K_SSP 19
#define APR$K_USP 20
#define APR$K_VPTB 21
#define APR$K_WHAMI 22
#define APR$K_PS 23
#define APR$K_ICSR 24
#define APR$K_MCSR 25
#define APR$K_VIRBND 26
#define APR$K_SYSPTBR 27
#define IBX$K_TB_TAG 28
#define IBX$K_ITB_PTE 29
#define IBX$K_ICCSR 30
#define IBX$K_ITB_PTE_TEMP 31
#define IBX$K_EXC_ADDR 32
#define IBX$K_SL_RCV 33
#define IBX$K_ITBZAP 34
#define IBX$K_ITBASM 35
#define IBX$K_ITBIS 36
#define IBX$K_PS 37
#define IBX$K_EXC_SUM 38
#define IBX$K_PAL_BASE 39
#define IBX$K_HIRR 40
#define IBX$K_SIRR 41
#define IBX$K_ASTRR 42
#define IBX$K_HIER 43
#define IBX$K_SIER 44
#define IBX$K_ASTER 45
#define IBX$K_SL_CLR 46
#define IBX$K_SL_XMIT 47
#define ABX$K_DTB_CTL 48
#define ABX$K_DTB_PTE 49
#define ABX$K_DTB_PTE_TEMP 50
#define ABX$K_MMCSR 51
#define ABX$K_VA 52
#define ABX$K_DTBZAP 53
#define ABX$K_DTASM 54
#define ABX$K_DTBIS 55
#define ABX$K_BIU_ADDR 56
#define ABX$K_BIU_STAT 57
#define ABX$K_DC_ADDR 58
#define ABX$K_DC_STAT 59
#define ABX$K_FILL_ADDR 60
#define ABX$K_ABOX_CTL 61
#define ABX$K_ALT_MODE 62
#define ABX$K_CC 63
#define ABX$K_CC_CTL 64
#define ABX$K_BIU_CTL 65
#define ABX$K_FILL_SYNDROME 66
#define ABX$K_BC_TAG 67
#define MAX_IPR 67
 
