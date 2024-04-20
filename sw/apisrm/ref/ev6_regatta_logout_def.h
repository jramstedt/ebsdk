/*
 * Copyright (C) 1998, 1999 by
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
 * Abstract:	680 Logout frame and Error Halt definitions for Regatta
 *
 * Author:	Harold Buckingham
 */

struct regatta_logout_680 {
#if WINDJAMMER
    uint64 cchip_dirx;
    uint64 tig_smr0;
    uint64 tig_smr1;
    uint64 tig_esr;
#endif
#if CLIPPER
    uint32 size;
    uint32 flag;
    uint32 proc_offset;
    uint32 sys_offset;
    uint32 mchk_code;
    uint32 frame_rev;
    uint64 os_flags;
    uint64 cchip_dirx;
    uint64 tig_smir;
    uint64 tig_cpuir;
    uint64 tig_psir;
    uint64 lm78_isr;
    uint64 door_open;
    uint64 temp_warning;
    uint64 fan_ctrl_fault;
    uint64 power_down_code;
    uint64 reserved_1;
#endif
    } ;

#define SYS_FRAME_HEADER__REV   1;
#define SYS_EVENT__TYPE  2;
#define SYS_FRAME_HEADER__CLASS 5;

struct sys_event_frame_header {
    unsigned short length;
    unsigned short class;
    unsigned short type;
    unsigned short revision;
    uint64 timestamp;
    unsigned int frame_length;
    unsigned int frame_count;
    };

#define ERR_FRAME_HEADER__REV   1;
#define ERR_EVENT__TYPE  3;
#define ERR_FRAME_HEADER__CLASS 5;

struct err_halt_frame_header {
    unsigned short length;
    unsigned short class;
    unsigned short type;
    unsigned short revision;
    unsigned int halt_code;
    unsigned int reserved;
    uint64 timestamp;
    unsigned int frame_length;
    unsigned int frame_count;
    };

#define REGATTA_FRAME_CPU__TYPE  1;
#define REGATTA_FRAME_SYS__TYPE  2;
#define REGATTA_FRAME_EVENT__TYPE  3;
#define REGATTA_FRAME__REV   1;
#define REGATTA_FRAME__CLASS 12;

struct regatta_frame_header_subpkt {
    unsigned short length;
    unsigned short class;
    unsigned short type;
    unsigned short revision;
    uint64 whami;
    };

#define ENTRY_TERM__REV   1;
#define ENTRY_TERM__TYPE  0;
#define ENTRY_TERM__CLASS 0;

struct entry_term {
    unsigned short length;
    unsigned short class;
    unsigned short type;
    unsigned short revision;
    };
