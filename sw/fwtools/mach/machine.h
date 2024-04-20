/*
 * *****************************************************************
 * *                                                               *
 * *    Copyright (c) Digital Equipment Corporation, 1991, 1996    *
 * *                                                               *
 * *   All Rights Reserved.  Unpublished rights  reserved  under   *
 * *   the copyright laws of the United States.                    *
 * *                                                               *
 * *   The software contained on this media  is  proprietary  to   *
 * *   and  embodies  the  confidential  technology  of  Digital   *
 * *   Equipment Corporation.  Possession, use,  duplication  or   *
 * *   dissemination of the software and media is authorized only  *
 * *   pursuant to a valid written license from Digital Equipment  *
 * *   Corporation.                                                *
 * *                                                               *
 * *   RESTRICTED RIGHTS LEGEND   Use, duplication, or disclosure  *
 * *   by the U.S. Government is subject to restrictions  as  set  *
 * *   forth in Subparagraph (c)(1)(ii)  of  DFARS  252.227-7013,  *
 * *   or  in  FAR 52.227-19, as applicable.                       *
 * *                                                               *
 * *****************************************************************
 */
/*
 * HISTORY
 */
/*	
 *	@(#)$RCSfile: machine.h,v $ $Revision: 1.1 $ (DEC) $Date: 1999/03/29 22:17:41 $
 */ 
/*
 */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 *	Machine independent machine abstraction.
 *	Copyright (C) 1986, Avadis Tevanian, Jr.
 */

#ifndef	_MACH_MACHINE_H_
#define _MACH_MACHINE_H_

#ifdef	KERNEL
#include <cpus.h>
#endif	/*KERNEL*/

#include <mach/machine/vm_types.h>
#include <mach/boolean.h>

/*
 *	For each host, there is a maximum possible number of
 *	cpus that may be available in the system.  This is the
 *	compile-time constant NCPUS, which is defined in cpus.h.
 *
 *	In addition, there is a machine_slot specifier for each
 *	possible cpu in the system.
 */

struct machine_info {
	int		major_version;	/* kernel major version id */
	int		minor_version;	/* kernel minor version id */
	int		max_cpus;	/* max number of cpus compiled */
	int		avail_cpus;	/* number actually available */
	vm_size_t	memory_size;	/* size of memory in bytes */
};

typedef struct machine_info	*machine_info_t;
typedef struct machine_info	machine_info_data_t;	/* bogus */

typedef int	cpu_type_t;
typedef int	cpu_subtype_t;

/* need to match defines in sys/dk.h for CP_* */
#define CPU_STATE_MAX		5
#define CPU_STATE_USER		0
#define CPU_STATE_NICE		1
#define CPU_STATE_SYSTEM	2
#define CPU_STATE_IDLE		3  /* all states with > number are idle! */
#define CPU_STATE_WAIT		4  /* see ./kernel/kern/clock_prim.c */

struct machine_slot {
	boolean_t	is_cpu;		/* is there a cpu in this slot? */
	cpu_type_t	cpu_type;	/* type of cpu */
	cpu_subtype_t	cpu_subtype;	/* subtype of cpu */
	boolean_t	running;	/* is cpu running */
	long		cpu_ticks[CPU_STATE_MAX];
	int		clock_freq;	/* clock interrupt frequency */
	boolean_t	error_restart;	/* set during hardware error restart */
	char    	*cpu_panicstr;          /* panic string */
	struct thread *cpu_panic_thread;	/* thread that panicked */
};

typedef struct machine_slot	*machine_slot_t;
typedef struct machine_slot	machine_slot_data_t;	/* bogus */

#ifdef	KERNEL
extern struct machine_info	machine_info;
extern struct machine_slot	machine_slot[NCPUS];
#ifndef __alpha
extern vm_offset_t		interrupt_stack[NCPUS];
#endif /* __alpha */
#endif	/*KERNEL*/

/*
 *	Machine types known by all.
 */

#define CPU_TYPE_VAX		((cpu_type_t) 1)
#define CPU_TYPE_ROMP		((cpu_type_t) 2)
#define CPU_TYPE_MC68020	((cpu_type_t) 3)
#define CPU_TYPE_NS32032	((cpu_type_t) 4)
#define CPU_TYPE_NS32332        ((cpu_type_t) 5)
#define CPU_TYPE_NS32532        ((cpu_type_t) 6)
#define CPU_TYPE_I386		((cpu_type_t) 7)
#define CPU_TYPE_MIPS		((cpu_type_t) 8)
#define	CPU_TYPE_MC68030	((cpu_type_t) 9)
#define CPU_TYPE_MC68040	((cpu_type_t) 10)
#define CPU_TYPE_PA_RISC        ((cpu_type_t) 11)
#define CPU_TYPE_ARM		((cpu_type_t) 12)
#define CPU_TYPE_MC88000	((cpu_type_t) 13)
#define CPU_TYPE_SPARC		((cpu_type_t) 14)
#define CPU_TYPE_ALPHA		((cpu_type_t) 15)


/*
 *	Machine subtypes (these are defined here, instead of in a machine
 *	dependent directory, so that any program can get all definitions
 *	regardless of where is it compiled).
 */

/*
 *	VAX subtypes (these do *not* necessary conform to the actual cpu
 *	ID assigned by DEC available via the SID register).
 */

#define CPU_SUBTYPE_VAX780	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_VAX785	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_VAX750	((cpu_subtype_t) 3)
#define CPU_SUBTYPE_VAX730	((cpu_subtype_t) 4)
#define CPU_SUBTYPE_UVAXI	((cpu_subtype_t) 5)
#define CPU_SUBTYPE_UVAXII	((cpu_subtype_t) 6)
#define CPU_SUBTYPE_VAX8200	((cpu_subtype_t) 7)
#define CPU_SUBTYPE_VAX8500	((cpu_subtype_t) 8)
#define CPU_SUBTYPE_VAX8600	((cpu_subtype_t) 9)
#define CPU_SUBTYPE_VAX8650	((cpu_subtype_t) 10)
#define CPU_SUBTYPE_VAX8800	((cpu_subtype_t) 11)
#define CPU_SUBTYPE_UVAXIII	((cpu_subtype_t) 12)

/*
 *	Alpha subtypes (these do *not* necessary conform to the actual cpu
 *	ID assigned by DEC available via the SID register).
 */

#define CPU_SUBTYPE_ALPHA_ADU		((cpu_subtype_t) 1)
#define CPU_SUBTYPE_DEC_4000		((cpu_subtype_t) 2)
#define CPU_SUBTYPE_DEC_7000		((cpu_subtype_t) 3)
#define CPU_SUBTYPE_DEC_3000_500	((cpu_subtype_t) 4)
#define CPU_SUBTYPE_DEC_3000_400	((cpu_subtype_t) 5)
#define CPU_SUBTYPE_DEC_10000		((cpu_subtype_t) 6)
#define CPU_SUBTYPE_DEC_3000_300        ((cpu_subtype_t) 7)
#define CPU_SUBTYPE_DEC_2000_300        ((cpu_subtype_t) 8)
#define CPU_SUBTYPE_DEC_2100_A500	((cpu_subtype_t) 9)
#define CPU_SUBTYPE_DEC_AXPVME_64       ((cpu_subtype_t) 10)
/* #define CPU_SUBTYPE_DEC_MORGAN	((cpu_subtype_t) 11) */ /* cancelled */
#define CPU_SUBTYPE_DEC_AXPPCI_33	((cpu_subtype_t) 11)
#define CPU_SUBTYPE_DEC_AVANTI          ((cpu_subtype_t) 13)
#define CPU_SUBTYPE_DEC_MUSTANG         ((cpu_subtype_t) 14)
#define CPU_SUBTYPE_DEC_800_5		((cpu_subtype_t) 15)
#define CPU_SUBTYPE_DEC_21000_800      ((cpu_subtype_t) 16)
#define CPU_SUBTYPE_DEC_1000		((cpu_subtype_t) 17)
#define CPU_SUBTYPE_DEC_21000_900      ((cpu_subtype_t) 18)
#define CPU_SUBTYPE_EB66		((cpu_subtype_t) 19)
#define CPU_SUBTYPE_EB64P		((cpu_subtype_t) 20)
#define CPU_SUBTYPE_DEC_2100A_A500	((cpu_subtype_t) 24)
#define CPU_SUBTYPE_EB164		((cpu_subtype_t) 26)
#define CPU_SUBTYPE_DEC_1000A		((cpu_subtype_t) 27)

/*
 *	ROMP subtypes.
 */

#define CPU_SUBTYPE_RT_PC	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_RT_APC	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_RT_135	((cpu_subtype_t) 3)

/*
 *	68020 subtypes.
 */

#define CPU_SUBTYPE_SUN3_50	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_SUN3_160	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_SUN3_260	((cpu_subtype_t) 3)
#define CPU_SUBTYPE_SUN3_110	((cpu_subtype_t) 4)
#define CPU_SUBTYPE_SUN3_60	((cpu_subtype_t) 5)

#define CPU_SUBTYPE_HP_320	((cpu_subtype_t) 6)
	/* 16.67 Mhz HP 300 series, custom MMU [HP 320] */
#define CPU_SUBTYPE_HP_330	((cpu_subtype_t) 7)
	/* 16.67 Mhz HP 300 series, MC68851 MMU [HP 318,319,330,349] */
#define CPU_SUBTYPE_HP_350	((cpu_subtype_t) 8)
	/* 25.00 Mhz HP 300 series, custom MMU [HP 350] */
#define CPU_SUBTYPE_APOLLO_3000 ((cpu_subtype_t) 9)
#define CPU_SUBTYPE_APOLLO_4000 ((cpu_subtype_t) 10)

/*
 *	32032/32332/32532 subtypes.
 */

#define CPU_SUBTYPE_MMAX_DPC	    ((cpu_subtype_t) 1)	/* 032 CPU */
#define CPU_SUBTYPE_SQT		    ((cpu_subtype_t) 2)
#define CPU_SUBTYPE_MMAX_APC_FPU    ((cpu_subtype_t) 3)	/* 32081 FPU */
#define CPU_SUBTYPE_MMAX_APC_FPA    ((cpu_subtype_t) 4)	/* Weitek FPA */
#define CPU_SUBTYPE_MMAX_XPC_FPU    ((cpu_subtype_t) 5)	/* 532 +'381 FPU */
#define CPU_SUBTYPE_MMAX_XPC_FPA    ((cpu_subtype_t) 6)	/* 532 +580+WTL3164 */
#define CPU_SUBTYPE_MMAX_RES1		/* Reserved */
#define CPU_SUBTYPE_MMAX_RES2		/* Reserved */

/*
 *	80386 subtypes.
 */

#define CPU_SUBTYPE_AT386	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_EXL		((cpu_subtype_t) 2)
#define CPU_SUBTYPE_SQT86	((cpu_subtype_t) 3)

/*
 *	Mips subtypes.
 */

#define CPU_SUBTYPE_MIPS_R2300	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_MIPS_R2600	((cpu_subtype_t) 2)
#define CPU_SUBTYPE_MIPS_R2800	((cpu_subtype_t) 3)
#define CPU_SUBTYPE_MIPS_R2000a	((cpu_subtype_t) 4)
/* dgd -- addition for 3max support */
#define CPU_SUBTYPE_MIPS_R3000a ((cpu_subtype_t) 5)

/*
 * 	MC68030 subtypes
 */

#define CPU_SUBTYPE_NeXT	((cpu_subtype_t) 1) 
	/* NeXt thinks MC68030 is 6 rather than 9 */
#define CPU_SUBTYPE_HP_340	((cpu_subtype_t) 2) 
	/* 16.67 Mhz HP 300 series [HP 332,340] */
#define CPU_SUBTYPE_HP_360	((cpu_subtype_t) 3) 
	/* 25.00 Mhz HP 300 series [HP 360] */
#define CPU_SUBTYPE_HP_370	((cpu_subtype_t) 4) 
	/* 33.33 Mhz HP 300 series [HP 370] */
#define CPU_SUBTYPE_APOLLO_2500 ((cpu_subtype_t) 5)
#define CPU_SUBTYPE_APOLLO_3500 ((cpu_subtype_t) 6)
#define CPU_SUBTYPE_APOLLO_4500 ((cpu_subtype_t) 7)


/*
 *	PA_RISC subtypes  Hewlett-Packard HP-PA family of
 *	risc processors 800 series workstations.
 *	Port done by Hewlett-Packard
 */

#define CPU_SUBTYPE_PA_RISC_840 ((cpu_subtype_t) 0x004)
#define CPU_SUBTYPE_PA_RISC_825 ((cpu_subtype_t) 0x008)
#define CPU_SUBTYPE_PA_RISC_835 ((cpu_subtype_t) 0x00a)
#define CPU_SUBTYPE_PA_RISC_850 ((cpu_subtype_t) 0x00c)
#define CPU_SUBTYPE_PA_RISC_855 ((cpu_subtype_t) 0x081)
#define CPU_SUBTYPE_PA_RISC_810 ((cpu_subtype_t) 0x100)
#define CPU_SUBTYPE_PA_RISC_815 ((cpu_subtype_t) 0x103)
#define CPU_SUBTYPE_PA_RISC_710 ((cpu_subtype_t) 0x300)
#define CPU_SUBTYPE_PA_RISC_720 ((cpu_subtype_t) 0x200)
#define CPU_SUBTYPE_PA_RISC_730 ((cpu_subtype_t) 0x202)
#define CPU_SUBTYPE_PA_RISC_750 ((cpu_subtype_t) 0x201)

/* 
 * 	Acorn subtypes - Acorn Risc Machine port done by
 *		Olivetti System Software Laboratory
 */

#define CPU_SUBTYPE_ARM_A500_ARCH	((cpu_subtype_t) 1)
#define CPU_SUBTYPE_ARM_A500		((cpu_subtype_t) 2)
#define CPU_SUBTYPE_ARM_A440		((cpu_subtype_t) 3)
#define CPU_SUBTYPE_ARM_M4		((cpu_subtype_t) 4)
#define CPU_SUBTYPE_ARM_A680		((cpu_subtype_t) 5)

/*
 *	MC88000 subtypes - Encore doing port.
 */

#define CPU_SUBTYPE_MMAX_JPC	((cpu_subtype_t) 1)

/*
 *	Sun4 subtypes - port done at CMU
 */

#define CPU_SUBTYPE_SUN4_260		((cpu_subtype_t) 1)
#define CPU_SUBTYPE_SUN4_110		((cpu_subtype_t) 2)

/* Macros for 3rd party CPU support: 
 * Alpha AXP CPU vendors should Contact Digital for a vendor-id, and use
 * the macro definitions provided here  in order to insure uniqueness of    
 * their cpu id's with  respect to Digital and other 3rd parties.       
 * Digital's vendor id is 0; refer to hal/cpuconf.h for Digital macro
 * definitions. 
 *
 * Example: Assume Vendor XYZ is  assigned Vendor ID 1 by Digital.
 * 	    Assume Vendor XYZ has two platforms, identified as 1 and 2 below.
 * 
 * 	 #define XYZ_VENDOR_ID 1
 *	 #define VENDOR_ID  XYZ_VENDOR_ID
 *
 *	 #define XYZCPU1 MAKESID(1)
 *	 #define XYZCPU2 MAKESID(2)
*/

#define SIDBITS        18      /* system id bits */           
#define VIDBITS        14      /* vendor id bits */           
#define MAKESID(sid)  ((VENDOR_ID << SIDBITS) | sid)


#endif	/*_MACH_MACHINE_H_*/
