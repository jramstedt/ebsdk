/*
 * Copyright (C) 1997 by
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
 * Abstract:	Logout frame definitions for Tsuanami
 *
 * Author:	Stephen F. Shirron
 */

struct common_logout {
    uint32 size;
    uint32 flag;
    uint32 proc_offset;
    uint32 sys_offset;
    uint32 mchk_code;
    uint32 frame_rev;
    uint64 i_stat;
    uint64 dc_stat;
    uint64 c_addr;
    uint64 dc1_syndrome;
    uint64 dc0_syndrome;
    uint64 c_stat;
    uint64 c_sts;
    uint64 mm_stat;
    } ;

struct tsunami_common_logout {
    uint64 os_flags;
    uint64 cchip_dirx;
    uint64 cchip_misc;
    uint64 pchip0_err;
    uint64 pchip1_err;
    } ;

struct logout_crd {
    struct common_logout common;
    struct tsunami_common_logout tsunami_common;
    } ;

struct logout {
    struct common_logout common;
    uint64 exc_addr;
    uint64 ier_cm;
    uint64 isum;
    uint64 reserved_0;
    uint64 pal_base;
    uint64 i_ctl;
    uint64 process_context;
    uint64 reserved_1;
    uint64 reserved_2;
    struct tsunami_common_logout tsunami_common;
    } ;
