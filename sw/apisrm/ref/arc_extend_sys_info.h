/* file:	arc_extend_sys_info.h
 *
 * Copyright (C) 1993 by
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
 * Abstract:	ARC ExtendedSystemInformation definitions.
 *
 * Author:	Judith E. Gold
 *
 * Modifications:
 *
 */

#if SABLE	/* dummy values for now */
#if GAMMA
#define PROCESSOR_ID 21164		/* The processor type - EV5*/
#else
#define PROCESSOR_ID 21064		/* The processor type - EV4*/
#endif
#define PROCESSOR_REVISION 1		/* The processor revision number.*/
#define PROCESSOR_PAGE_SIZE 8192	/* The processor page size, in bytes.*/
#define SYSTEM_REVISION 1		/* The system revision number*/
#endif

#if ALCOR	/* dummy values for now */
#define PROCESSOR_ID 21164		/* The processor type - EV5 */
#define PROCESSOR_REVISION 1		/* The processor revision number.*/
#define PROCESSOR_PAGE_SIZE 8192	/* The processor page size, in bytes.*/
#define SYSTEM_REVISION 1		/* The system revision number*/
#endif

#if TURBO	/* dummy values for now */
#define PROCESSOR_ID 21164		/* The processor type - EV5 */
#define PROCESSOR_REVISION 1		/* The processor revision number.*/
#define PROCESSOR_PAGE_SIZE 8192	/* The processor page size, in bytes.*/
#define SYSTEM_REVISION 1		/* The system revision number*/
#endif

#if MIKASA	/* dummy values for now */
#define PROCESSOR_ID 21164		/* The processor type - EV5 */
#define PROCESSOR_REVISION 1		/* The processor revision number.*/
#define PROCESSOR_PAGE_SIZE 8192	/* The processor page size, in bytes.*/
#define SYSTEM_REVISION 1		/* The system revision number*/
#endif

#if RAWHIDE	/* dummy values for now */
#define PROCESSOR_ID 21164		/* The processor type - EV5 */
#define PROCESSOR_REVISION 1		/* The processor revision number.*/
#define PROCESSOR_PAGE_SIZE 8192	/* The processor page size, in bytes.*/
#define SYSTEM_REVISION 1		/* The system revision number*/
#endif

