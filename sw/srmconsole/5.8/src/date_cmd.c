/* file:	date_cmd.c
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Date, set and display date from the toy clock.
 *
 *  AUTHORS:
 *
 *      J. Kirchoff
 *
 *  CREATION DATE:
 *  
 *	jrk	 5-Oct-1993
 *
 *  MODIFICATION HISTORY:
 *
 *--
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:time.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

static void date_error();
static int convertn();
static int isleap();
static int dow();
static int mod();

static int mlength [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

char *render_date (struct toy_date_time *toy, char *buf, int os_type);

/*+
 * ============================================================================
 * = date - Set or display the current time and date.                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Displays or modifies the current time and date. If you do not 
 *	specify any arguments, date displays the current date and time. 
 *	If you specify arguments, date modifies the arguments that you 
 *	specify in the TOY clock.
 *
 *	If you want to modify the date or time, you must specify at 
 *	least four digits, those that represent the hour and minute. 
 *	Omitted fields are inherited. When setting the date, the day 
 *	of the week is automatically generated.
 *  
 *   COMMAND FMT:	date 2 B 0 date
 *
 *   COMMAND FORM:
 *  
 *	date ( [<yyyymmddhhmm.ss>] )
 *  
 *   COMMAND TAG:	date 0 RXBZ date
 *
 *   COMMAND ARGUMENT(S):
 *
 *	<yymmddhhmm.ss> - Specifies the date and time string consisting
 *		of decimal pairs, where:
 *#o
 *		o 'yyyy' (0000-9999) is the year,
 *		o 'mm' (01-12) is the two digit month,
 *		o 'dd' (01-31) is the two digit day,
 *		o 'hh' (00-23) is the two digit hour,
 *		o 'mm' (00-59) is the two digit minute, and
 *		o 'ss' (00-59) is the two digit second.
 *#
 *   COMMAND OPTION(S):
 *
 *	-vms	- Specifies to bias the year by VMS base year. If year >= 95, year + 1900 else year + 2000;
 *	-linux	- Specifies to bias the year by VMS base year.
 *	-unix	- Specifies to bias the year by UNIX base year. Apply VMS then year - 48.
 *	-nt	- Specifies to bias the year by NT base year. NT base year is 1980.
 *	-osf	- Specifies to bias the year by UNIX base year.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>date 199208031029.00
 *	>>>date
 *	10:29:04  August  3, 1992
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	date ( int argc, char *argv[] )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
#define QOSF	0
#define QUNIX	1
#define QVMS	2
#define QNT	3
#define QTOY	4
#define QLINUX	5
#define QMAX	6
#define	QSTRING	"osf unix vms nt %dtoy linux"

int date (int argc, char *argv []) {
	int i, id, status;
	u_int os_type = 0;
	u_int toy_unit = 0xffff;
	struct toy_date_time toy;
	char *cp;
	int pm;
	char line [80];
	char time_string [20];
	int year;
	struct QSTRUCT qual [QMAX];

	/* determine which format to use */

	if (argc == -1) {
	    os_type = argv[0];
	    toy_unit = (os_type >> 16) & 0xffff;
	    os_type &= 0xffff;
	}
	if (argc == 0) {
	    os_type = argv;
	    toy_unit = (os_type >> 16) & 0xffff;
	    os_type &= 0xffff;
	}

	if (argc >= 1) {
	    status = qscan (&argc, argv, "-", QSTRING, qual);
	    if (status != msg_success) {
		err_printf (status);
		return status;
	    }
	    if ((qual [QOSF].present) || (qual [QUNIX].present) || (qual [QLINUX].present))
		os_type = 1;
	    if (qual [QNT].present)
		os_type = 2;
	    if (qual [QTOY].present)
		toy_unit = qual[QTOY].value.integer;
	}
	
	/* Read in current date */

#if TURBO
	if (toy_unit != 0xffff)
	    pci_get_date_time(&toy, toy_unit);
	else
#endif
	    get_date_time (&toy);		/* inherit existing fields */

	/*
	 * Report the current date.  Handle both 12 and 24 hour modes.
	 * Assume date is in binary (the chip also handles BCD).
	 */

	if (argc == 0) {
	    id = whoami();
#if TURBO
	    if (toy_unit != 0xffff)
		pprintf ("TOY %2d  %s\n", toy_unit, render_date (&toy, line, os_type));
	    else
#endif
		pprintf ("CPU %d/%d %s\n", id, id+1, render_date (&toy, line, os_type));
	    return 0;
	}
	if (argc == 1) {
	    printf ("%s\n", render_date (&toy, line, os_type));
	    return 0;
	}

	/* verify that string is not too long */

	if (strlen(argv[1]) > 15) {
	    date_error ();
	    return msg_failure;
	}

	strcpy(time_string, argv[1]);

	/* verify that only digits and a decimal point are present */

	if (strcount (time_string, "0123456789.") != strlen (time_string)) {
	    date_error ();
	    return msg_failure;
	}

	/*
	 * Chop off the seconds if they exist
	 */
	toy.seconds = 0;			/* default seconds to 0 */
	if (cp = strchr (time_string, '.')) {
	    toy.seconds = atoi (cp+1);
	    *cp = 0;
	}
	    
	/*
	 * Parse the rest of the date
	 */
	cp = time_string;
	switch (strlen (time_string)) {
	case 12:
	    year = convertn (4, cp);
	    cp += 4;
	    if (os_type == 1)
		year += 48;		/* unix is 48 years in the future */
	    if (os_type <= 1) {
		if (year > 2000)
		    toy.year = year - 2000;
		else
		    toy.year = year - 1900;
	    }
	    if (os_type == 2)
		toy.year = year - 1980;	/* nt base year is 1980 */
	case 8:
	    toy.month = convertn (2, cp);
	    cp += 2;
	case 6:
	    toy.day_of_month = convertn (2, cp);
	    cp += 2;
	case 4:
	    toy.hours = convertn (2, cp);
	    if (toy.hours > 23)
		toy.hours = 0;
	    cp += 2;
	    toy.minutes = convertn (2, cp);
	    if (toy.minutes > 59)
		toy.minutes = 0;
	    cp += 2;
	    break;
	default:
	    date_error ();
	    return msg_failure;
	}         

	/*
	 * Set the day of the week
	 */
	toy.day_of_week = 1 + dow (
	    year,
	    toy.month,
	    toy.day_of_month
	);

	/*
	 * Write out the new date
	 */
#if TURBO
	if (toy_unit != 0xffff)
	    pci_put_date_time(&toy, toy_unit);
	else
#endif
	    put_date_time (&toy);

	return msg_success;
}

static void date_error () {
	pprintf ("date: date is not in the form [yyyy[mm[dd]]]hhmm[.ss]\n");
}

static int convertn (int n, char *s) {
	int x;
	x = 0;
	while (n--) {
	    x *= 10;
	    x += *s - '0';
	    s++;
	}
	return x;
}


/*----------*/
static int isleap (int year) {	/* true for a leap year, false otherwise */
	return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);
}

/*
 * Calculate which weekday a date is, with Sunday being day 0.
 * Months are origin 1
 * date is origin 1
 */
static int dow (int year, int month, int date) {
	long int	daycount;
	month--;

	daycount = (year - 1582) * 365	/* # of intervening years	*/
		+ mod (1582, year, 4)	/* # of years divisble by 4	*/
		- mod (1582, year, 100) /* # of years divisible by 100	*/
		+ mod (1582, year, 400) /* # of years divisible by 400	*/
	;

	daycount -= (isleap (year)) ? 1 : 0;

	return (daycount		/* number of days		*/
		+ 5			/* Jan 1, 1582			*/
		+ day_julian (year, month, date)) % 7
	;
}

/*
 * Calculate number of integers between [lower, upper) that are divisble
 * by modulus.  Boundaries are inclusive/exclusive.
 *
 */
static int mod (int lower, int upper, int modulus) {
	lower /= modulus;
	upper /= modulus;
	return ((upper <= lower) ? 0 : (upper - lower));
}

