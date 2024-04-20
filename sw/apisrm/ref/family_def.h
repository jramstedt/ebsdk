/* $Header$
 * file:	family_def.h   - definitions for values left by SROM
 *                                  in the memory.
 * Copyright (C) 1992, 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */


/*
 *++
 *  FACILITY:
 *
 *  MODULE DESCRIPTION:
 *  Macros to find system family and member. One set of macros based on values left
 *  by SROM in the memory and another on values found in HWRPB.
 *
 *  AUTHORS:
 *
 *	Alex Malkin
 *
 *  CREATION DATE:
 *  
 *     Aug , 1995
 *
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */


/*
 * Family and Member IDs
 */
                            /* Avanti/Mustang family */
#define AVANTI_FAMILY          13
#define AVANTI_FIRST_MEMBER    01
#define AVANTI_LAST_MEMBER     13
#define LX3_FIRST_MEMBER       32
#define LX3_LAST_MEMBER        45
                            /* Alcor Family */
#define ALCOR_FAMILY           15
#define ALCOR_FIRST_MEMBER     01
#define ALCOR_LAST_MEMBER      02


/*
 * Macros based on data in HWRPB
 */

/*
 * SROM code leaves some system parameters in the memory buffer starting at addr. 0
 * 'srominfo' sturcture defines these parameters and their relative locations
 * ( it should be updated later when more information is available from Greg)
 */
struct   srominfo 
	{                              
	__int64	x00;	/* 0x00 - */
	__int64	x08;	/* 0x08 - */
	__int64	x10;	/* 0x10 - */
	__int64	x18;	/* 0x18 - */
	__int64	x20;	/* 0x20 - */
	__int64	x28;	/* 0x28 - */
	__int64	x30;	/* 0x30 - */
	__int64	memsize;	/* 0x38 - memory size in MB */
	__int64	x40;	/* 0x40 - */
	__int64	x48;	/* 0x48 - */
	__int64	x50;	/* 0x50 - */
	__int64	x58;	/* 0x58 - */
	__int64	x60;	/* 0x60 - */
	__int64	x68;	/* 0x68 - */
	__int64	sysvar;		/* 0x70 - system member */
	__int64	x78;	/* 0x78 - */
	__int64	x80;	/* 0x80 - */
	__int64	systype;	/* 0x88 - system family */
	};

#define SROM_ADDR_SYSTYPE	0x88
#define SROM_ADDR_SYSVAR	0x70
#define SROM_ADDR_MEMSIZE	0x38

/*
 *    Use following to access the members of the structure 'srominfo'
 * struct srominfo *pSromInfo = NUL;
 * systype = pSromInfo->systype;
 * sysvar  = pSromInfo->sysvar;
 */

#define SROM_MEM_MEMSIZE      (*(int *)SROM_ADDR_MEMSIZE)                 

/*
 * SROM code leaves some system parameters in the memory buffer starting at addr. 0
 *  Define macros to access the system vars. left by SROM in mem. addr. 0
 */

#define SROM_SYS_FAMILY      (*(int *)SROM_ADDR_SYSTYPE)
#define SROM_SYS_MEMBER      (*(int *)SROM_ADDR_SYSVAR)                 
#define SROM_SYS_MEMBERS(first,last) ((first <= SROM_SYS_MEMBER) && (SROM_SYS_MEMBER <= last)) /*member range*/

#define SROM_SYS_LX3 ((SROM_SYS_FAMILY == AVANTI_FAMILY) && (SROM_SYS_MEMBERS(LX3_FIRST_MEMBER,LX3_LAST_MEMBER)))
/*
#define SROM_SYS_LX3 ((SROM_SYS_FAMILY == 0) && (SROM_SYS_MEMBER == 0))
*/



/*
 * Macros based on data in HWRPB
 */

#define HWRPB_SYS_FAMILY    (((struct HWRPB *)hwrpb)->SYSTYPE[0])
#define HWRPB_SYS_MEMBER    (((struct HWRPB *)hwrpb)->SYSVAR[0])
#define HWRPB_SYS_MEMBERS(first,last) ((first <= HWRPB_SYS_MEMBER) && (HWRPB_SYS_MEMBER <= last))  /*member range*/

#define HWRPB_SYS_LX3 ((HWRPB_SYS_FAMILY == AVANTI_FAMILY) && (HWRPB_SYS_MEMBERS(LX3_FIRST_MEMBER,LX3_LAST_MEMBER)))




