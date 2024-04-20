/*
 * kzpsa_def.h
 *
 * Copyright (C) 1994 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 * Abstract:	Definitions for KZPSA SIMPort SCSI Adapter
 *
 * Author:	Console Firmware Team
 *
 */
 
volatile struct kzpsa_pb {
    struct sim_pb pb;   		/* SIMPort port block */
    struct sim_regs *rp;		/* pointer to SIMPORT csrs base */
    void *flp;				/* pointer to KZPSA flash rom base */
    int pid;
    int status;
    int refcnt;
    struct SEMAPHORE init;
    struct SEMAPHORE shut;
    struct SEMAPHORE process;
    struct TIMERQ isr_t;
    struct SEMAPHORE owner_s;
    struct LOCK owner_l;
    unsigned long int int_pending;
    unsigned long int node_id;
    unsigned long int reinit;
    PSDLClassType4 HBAConfig;		/* parameters read from on-board ROM */
    } ;

/* SIMPort registers */

#define SIM_RESET_START		0xC

#define SIM_ARGUMENT_LO		0x20
#define SIM_ARGUMENT_HI		0x24
#define SIM_TEST_START		0x28
#define SIM_TEST_STATUS		0x2C

#define SIM_AMCSR_LO		0x100
#define SIM_AMCSR_HI		0x104
#define SIM_ABBR_LO		0x108
#define SIM_ABBR_HI		0x10C
#define SIM_DAFQIR_LO		0x110
#define SIM_DAFQIR_HI		0x114
#define SIM_DACQIR_LO		0x118
#define SIM_DACQIR_HI		0x11C

#define SIM_ASR_LO		0x140
#define SIM_ASR_HI		0x144
#define SIM_AFAR_LO		0x148
#define SIM_AFAR_HI		0x14C
#define SIM_AFPR_LO		0x150
#define SIM_AFPR_HI		0x154

#define SIM_ERR_LOG_N_LO	0x1C0
#define SIM_ERR_LOG_N_HI	0x1C4

#define SIM_CLRINT		0x300

