/****************************************************************************/
/* file:	presto_def.h						    */
/*                                                                          */
/* Copyright (C) 1992, 1993 by						    */
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
/* Abstract:	Cobra Prestoserve Memory Module structure definitions.      */
/*                                                                          */
/* Author:	Harold Buckingham                                           */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	hcb	02-Apr-1993	Fix Presto Battery enable def		    */
/*                                                                          */
/*	hcb	03-Feb-1993	Add Presto Battery enable def		    */
/*                                                                          */
/*	hcb	17-Dec-1992	Initial entry.                              */
/*                                                                          */
                                   
#define PRESTO_BANK_SEL 16         
#define IIC_PRESTO_DEF 16

/* NVRAM Contents */
#define PRESTO_SYS_ID_ADDR 0
#define PRESTO_SIG_ADDR 1024

/* Prestoserve CSR bits */
#define PRESTO_CSR 0x2000000
#define prestocsr$m_bchrg 1
#define prestocsr$m_bdisc 2
#define prestocsr$m_bfail 4
#define prestocsr$m_dmode2 64
#define prestocsr$m_dmode1 128


#define PRESTO_SIGNATURE 0xBD100248
#define PRESTO_BATT_ENAB 0x13
