/*
 * file:	console_hal_macros.h
 *
 * Copyright (C) 1990, 1991, 1992, 1993 by
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
 *
 *
 * Abstract:	The code contains macros to help abstract the hardware
 *              that the console uses.
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *	
 *	dwr	01-Jul-1993	Add CPU_NUM & CPU_SLOT macros.
 *
 *	djm	27-May-1993	Initial. 
 */


#if !TURBO
#if NEON||RUBY              
#define SYS_BUS_CNT 9
#define NODE_SIZE 0x400000
#define IO_SLOT_BASE 8
#define SYSBUS LEPLSB 
#define $BUSREG(reg) LSB_R_L/**/reg.LSB_L_L/**/reg
#define $BUSBITS(reg,field) LSB_R_L/**/reg/**/.LSB_R_L/**/reg/**/_BITS.LSB_V_/**/field
#endif    
#endif

#if TURBO
#undef SYS_BUS_CNT
#define SYS_BUS_CNT 9     
#define NODE_SIZE 0x400000
#define IO_SLOT_BASE 4
#define BSB 9
#define SYSBUS TLSB
#define $BUSREG(reg) TLSB_R_TL/**/reg.TL/**/reg/**/$L_REG
#define $BUSBITS(reg,field) TLSB_R_TL/**/reg/**/.TL/**/reg/**/$R_FIELD.TL/**/reg/**/$V_/**/field
#define CPU_NUM (whoami()%2)
#define CPU_SLOT (whoami()/2)
#endif

#if RAWHIDE
#undef SYS_BUS_CNT
#define SYS_BUS_CNT 8
#define NODE_SIZE 0x400000
#define IO_SLOT_BASE 4
#define BSB 9
#define SYSBUS MCBUS
#define $BUSREG(reg) TLSB_R_TL/**/reg.TL/**/reg/**/$L_REG
#define $BUSBITS(reg,field) TLSB_R_TL/**/reg/**/.TL/**/reg/**/$R_FIELD.TL/**/reg/**/$V_/**/field
#define CPU_NUM whoami()
#define CPU_SLOT whoami()
#endif

#if CALYPSO
#define SYS_BUS_CNT 16
#define NODE_SIZE 0x80000
#define IO_SLOT_BASE 8
#define SYSBUS LEPLSB
#define $BUSREG(reg) LSB_R_L/**/reg.LSB_L_L/**/reg
#define $BUSBITS(reg,field) LSB_R_L/**/reg/**/.LSB_R_L/**/reg/**/_BITS.LSB_V_/**/field
#endif

extern int sys_bus_adr[];
extern int mbx_ptr_adr[];
extern int sys_high_adr;
extern char *sys_bus_name;

#define $SYS_BUS_ADR($slot) ( sys_bus_adr[$slot] )
#define MBX_PTR_ADR($slot) ( mbx_ptr_adr[$slot - IO_SLOT_BASE] )

#define write1c(x) write_csr_32(x,read_csr_32(x))
