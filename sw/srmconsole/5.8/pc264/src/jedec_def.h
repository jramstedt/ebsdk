/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:08 by OpenVMS SDL EV1-31     */
/* Source:  24-FEB-2000 10:35:17 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]JEDEC_DEF.SD */
/********************************************************************************************************************************/
/* file:	jedec_def.sdl                                               */
/*                                                                          */
/* Copyright (C) 1998, by                                                   */
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
/* Abstract:	JEDEC specific definitions for Alpha firmware.              */
/*                                                                          */
/* Author:	Nigel J. Croxon          20-Oct-1998                        */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*** MODULE $jedec_spec ***/
typedef struct _JEDEC {
    unsigned char jedec_b_fru_spec [63];
    unsigned char jedec_b_checksum0to62 [1];
    unsigned char jedec_b_manuf_JEDEC_ID [8];
    unsigned char jedec_b_manuf_location [1];
    unsigned char jedec_b_manuf_part_class [3];
    unsigned char jedec_b_manuf_part_base [6];
    unsigned char jedec_b_manuf_part_variation [3];
    unsigned char jedec_b_manuf_part_revision [4];
    unsigned char jedec_b_manuf_part_space [2];
    unsigned char jedec_b_revision_code [2];
    unsigned char jedec_b_manuf_date_y [1];
    unsigned char jedec_b_manuf_date_m [1];
    unsigned char jedec_b_assembly_serialnum [4];
    unsigned char jedec_b_manuf_spec_alias [16];
    unsigned char jedec_b_manuf_spec_model [10];
    unsigned char jedec_b_manuf_spec_dec_JEDEC_ID [1];
    unsigned char jedec_b_revision_ro_data [1];
    unsigned char jedec_b_checksum64to126 [1];
    unsigned char jedec_b_sys_serialnumb [16];
    unsigned char jedec_b_tdd_log_head [4];
    unsigned char jedec_b_tdd_log_data [16];
    unsigned char jedec_b_sdd_log_ctrl [1];
    unsigned char jedec_b_sdd_log_block0 [44];
    unsigned char jedec_b_sdd_log_block1 [44];
    unsigned char jedec_b_dec_flag_id [1];
    unsigned char jedec_b_rev_rw_area [1];
    unsigned char jedec_b_checksum128to254 [1];
    } JEDEC;
typedef struct _JEDEC_WF {
    unsigned char jedec_b_manuf_JEDEC_ID [2];
    unsigned char jedec_b_manuf_location [1];
    unsigned char jedec_b_manuf_part_class [18];
    unsigned char jedec_b_manuf_date_y [1];
    unsigned char jedec_b_manuf_date_m [1];
    unsigned char jedec_b_assembly_serialnum [4];
    unsigned char jedec_b_manuf_spec_alias [16];
    unsigned char jedec_b_manuf_spec_model [10];
    unsigned char jedec_b_revision_ro_data [1];
    unsigned char jedec_b_revision_wo_data [1];
    unsigned char jedec_b_error_byte [1];
    } JEDEC_WF;
 
