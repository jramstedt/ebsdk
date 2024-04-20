/* File:   scam_def.h	
 *
 * Copyright (C) 1995 by
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
 * Modifications:
 *	dwn	02-Oct-1995	Initial entry.
*/

#define ISOLATED                  (0)
#define NOBODY_HOME               (0xFFFF)
#define QUINTET_MASK              (0x1F)

/* Define the bits in SCAM Type Code fields 
*/
#define MAXIMUM_ID_CODE           (0x30)
#define ID_VALID                  (0x06)
#define SERIAL_NUMBER_AVAILABLE   (0x01)

/* SCAM Function Codes 
*/
#define SCAM_SYNC                 (0x1F)
#define SCAM_ISOLATE              (0x00)
#define SCAM_CONFIG_PROC_COMPLETE (0x03)

/* First Quintet action codes 
*/
#define SCAM_FQ_SET_ID_00           (0x18)  /* 2nd Quintet assigns 0-7  */
#define SCAM_FQ_SET_ID_01           (0x11)  /* 2nd Quintet assigns 8-15 */
#define SCAM_FQ_CLEAR_PRIORITY_FLAG (0x14)  /* with SCAM_SQ_CLEAR...    */
#define SCAM_FQ_LOCATE_COMMAND      (0x14)  /* with SCAM_SQ_LOCATE...   */

/* Second Quintet action codes 
*/
#define SCAM_SQ_CLEAR_PRIORITY_FLAG (0x18)  /* was on with reset or power */
#define SCAM_SQ_LOCATE_ON           (0x12)  /* Turn on drive light  */
#define SCAM_SQ_LOCATE_OFF          (0x0B)  /* Turn off drive light */

#define ASSIGNABLE_ID             (0x80)    /* meaning, ID can be assigned  */
#define PREASSIGNED_ID            (0x40)    /* SCAM_TABLE had an assignment */
#define DONT_ASSIGN_ID            (0x3F)

#define DEFER                     (0x01)
#define TERMINATE                 (0x02)

#define SCAM_ID_STRLEN            (0x20)

#define NULL                      (0)

#define NARROW                    (8)       /*  8 bits for Narrow SCSI */
#define WIDE                      (16)      /* 16 bits for  Wide  SCSI */

#define HOST_SCSI_ID              (7)       /* ID of the host adapter  */
#define CLOCK_SETTING             (0x33)    /* Nominal for 40Mhz clock */
#define MAX_WIDE_DATA_BITS        NARROW
#define DIFFERENTIAL_MODE         FALSE     /* Set for single-ended    */
#define IOPORT                    (0x3100)  /* I/O address of adapter  */

#define ISOLATED                  (0)
#define NOBODY_HOME               (0xFFFF)
#define QUINTET_MASK              (0x1F)

/* Define the bits in SCAM Type Code fields 
*/
#define MAXIMUM_ID_CODE           (0x30)
#define ID_VALID                  (0x06)
#define SERIAL_NUMBER_AVAILABLE   (0x01)

/* SCAM Function Codes 
*/
#define SCAM_SYNC                 (0x1F)
#define SCAM_ISOLATE              (0x00)
#define SCAM_CONFIG_PROC_COMPLETE (0x03)

/* First Quintet action codes 
*/
#define SCAM_FQ_SET_ID_00           (0x18)  /* 2nd Quintet assigns 0-7  */
#define SCAM_FQ_SET_ID_01           (0x11)  /* 2nd Quintet assigns 8-15 */
#define SCAM_FQ_CLEAR_PRIORITY_FLAG (0x14)  /* with SCAM_SQ_CLEAR...    */
#define SCAM_FQ_LOCATE_COMMAND      (0x14)  /* with SCAM_SQ_LOCATE...   */

/* Second Quintet action codes 
*/
#define SCAM_SQ_CLEAR_PRIORITY_FLAG (0x18)  /* was on with reset or power */
#define SCAM_SQ_LOCATE_ON           (0x12)  /* Turn on drive light  */
#define SCAM_SQ_LOCATE_OFF          (0x0B)  /* Turn off drive light */

#define ASSIGNABLE_ID             (0x80)    /* meaning, ID can be assigned  */
#define PREASSIGNED_ID            (0x40)    /* SCAM_TABLE had an assignment */
#define DONT_ASSIGN_ID            (0x3F)

#define DEFER                     (0x01)
#define TERMINATE                 (0x02)

#define SCAM_ID_STRLEN            (0x20)

#define NULL                      (0)

#define NARROW                    (8)       /*  8 bits for Narrow SCSI */
#define WIDE                      (16)      /* 16 bits for  Wide  SCSI */

#define HOST_SCSI_ID              (7)       /* ID of the host adapter  */
#define CLOCK_SETTING             (0x33)    /* Nominal for 40Mhz clock */
#define MAX_WIDE_DATA_BITS        NARROW
#define DIFFERENTIAL_MODE         FALSE     /* Set for single-ended    */
#define IOPORT                    (0x3100)  /* I/O address of adapter  */


