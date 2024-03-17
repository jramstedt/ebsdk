#ifndef __FLOPPY_H_LOADED
#define __FLOPPY_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id: floppy.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 * $Log: floppy.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1996/08/20  02:48:43  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.8  1995/10/03  02:36:29  fdh
 * Added declaration for init_pic().
 *
 * Revision 1.7  1995/10/02  22:09:25  cruz
 * Moved some prototypes back to floppy.c
 *
 * Revision 1.6  1995/09/15  19:28:33  cruz
 * New for new floppy driver.
 *
 * Revision 1.5  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/22  15:22:35  rusling
 * Fixed up minor OSF build problem.  Changed fdacmd()
 * definition.
 *
 * Revision 1.3  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.2  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/06/08  19:56:13  fdh
 * Initial revision
 *
 */

/*
 *  Index into the FloppyDriveTypes table which contains parameters
 *  specific to the type of drive:  Double Density, High Density and
 *  Extended Density.  Note that the order is important since there's
 *  an assumption in the code that counts on the lower density drives
 *  having a lower index than the higher ones. 
 *  Please note that the indices must match those of the disk type and
 *  vice-versa.
 */ 
#define DD_DRIVE_TYPE           0
#define HD_DRIVE_TYPE           1
#define ED_DRIVE_TYPE           2

/*
 *  This type will contain all the information we need for accessing
 *  a floppy drive.
*/
typedef struct {
    LONG        DriveType;              /* Index into drive type table. */
    LONG        CurrentDriveType;       /* Changes with disk density.   */
    LONG        CurrentTrack;           /* Current track.               */
    BOOLEAN     DoNotUseDMAAccess;      /* TRUE for non-DMA mode.       */
    UCHAR       DriveNumber;            /* Drive number 0-3.            */
} FLOPPY_DRIVE_INFO;

/*
 *  Index into the FloppyDiskTypes table which contains parameters
 *  specific to the type of disk:  Double Density, High Density and
 *  Extended Density.  Note that the order is important since there's
 *  an assumption in the code that counts on the lower density disks
 *  having a lower index than the higher ones.
 *  Please note that the indices must match those of the drive type and
 *  vice-versa.
 */ 
#define DD_DISK_TYPE            0
#define HD_DISK_TYPE            1
#define ED_DISK_TYPE            2


extern
DBM_STATUS
InitializeFloppyDrive (
                       IN FLOPPY_DRIVE_INFO *DriveInfo
                       );

extern
DBM_STATUS
ReadLogicalSector (
                   IN FLOPPY_DRIVE_INFO *DriveInfo,
                   IN ULONG DiskType,
                   IN ULONG LogicalSector,
                   IN UCHAR *Destination
                   );

extern
DBM_STATUS
WriteLogicalSector (
                    IN FLOPPY_DRIVE_INFO *DriveInfo,
                    IN ULONG DiskType,
                    IN ULONG LogicalSector,
                    IN UCHAR *Source
                   );

extern
VOID
SpinDownFloppyDrive (
                     IN FLOPPY_DRIVE_INFO *DriveInfo 
                     );

extern void init_pic(void);

#endif /* __FLOPPY_H_LOADED */



