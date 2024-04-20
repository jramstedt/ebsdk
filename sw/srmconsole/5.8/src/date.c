/* file:	date.c
 *
 * Copyright (C) 1992 by
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      Date, set and display date from the toy clock.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *	ajb	13-Apr-1992
 *
 *  MODIFICATION HISTORY:
 *
 *	tna     11-Nov-1998     Conditionalized for Universe
 *
 *	jcc	21-Feb-1996	Added Cortex support for DS1386
 *
 *	bobf	12-Aug-1993	Insure new "month" value for DS1386
 *				does not alter the Osc. or Square Wave enables.
 *
 *	mdr	10-Aug-1993	Fixed month range check for Medulla.
 *
 *	bobf	09-Aug-1993	Translated the "generic" TOY structure
 *				to DS1386 register format
 *
 *	joes	01-Mar-1993	Added support for ds1386 and generalised TOY
 *				functions.
 *
 *	hcb	01-Dec-1992	Range check days and month in get_ds1287.
 *
 *	ajb	03-Aug-1992	Add a -bias, and require 4 digits when
 *				specifying years.
 *
 *--
 */

#include "cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:time.h"
#include "cp$inc:prototypes.h"

#define 	UI	unsigned int

extern struct LOCK spl_toy;

static int mlength [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define VMS_BIAS 1858

#if 0
char *days [] = {
	" ",
	msg_sunday,
	msg_monday,
	msg_tuesday,
	msg_wednesday,
	msg_thursday,
	msg_friday,
	msg_saturday
};
#endif

char *months [] = {
	" ",
	msg_january,
	msg_february,
	msg_march,
	msg_april,
	msg_may,
	msg_june,
	msg_july,
	msg_august,
	msg_september,
	msg_october,
	msg_november,
	msg_december
};

static int isleap (int year) {	/* true for a leap year, false otherwise */
	return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);
}

/*
 * Convert a date to the Julian day (origin 0).
 * month - origin 0
 * date - origin 1
 */
int day_julian (int year, int month, int datex) {
	int	julian;

	julian = 0;
	while (month-- > 0) {
	    if (month == 1) {
		julian += isleap (year) ? 29 : 28;
	    } else {
		julian += mlength [month];
	    }
	}
	return (julian + datex - 1);
}

/*
 * Generate the proper year given the limitations of the chip and the different
 * ways VMS and OSF want to store the year.
 */
int gen_year (int year) {
    if (year >= 95) {
    	return year + 1900;
    } else {
    	return year + 2000;
    }
}
	    
char *render_date (struct toy_date_time *toy, char *buf, int os_type)
{
    int year;

    if (os_type <= 1) {
	year = gen_year(toy->year);
	if (os_type == 1)
	    year -= 48;		/* unix is off 48 years in future */
    }
    if (os_type == 2) {
	year = toy->year;
	year += 1980;		/* nt base year is 1980 */
    }

    if (toy->month > 12)
	toy->month = 0;

    sprintf (buf, "%2d:%02d:%02d %s %s %d, %04d",
	toy->hours, 
	toy->minutes,
	toy->seconds,
	(toy->twentyfour_hour) ? "" : ((toy->pm_notam) ? "PM " : "AM "),
	months [toy->month],
	toy->day_of_month,
	year);
    return buf;
}

#if !TURBO
/*----------*/

unsigned char read_toy (struct FILE *fp, int offset) {
	unsigned char c;
	fseek (fp, offset, SEEK_SET);
	fread (&c, 1, 1, fp);
	return c;
}

unsigned char write_toy (struct FILE *fp, int offset, unsigned char c) {
	fseek (fp, offset, SEEK_SET);
	fwrite (&c, 1, 1, fp);
	return c;
}

/*
 * Generic form of get_"RTC#" function, calls to devices depending on platform.
 */
void get_date_time(struct toy_date_time *p)
{

#if MEDULLA || CORTEX || UNIVERSE
/* 
 * Note: The "generic" TOY fields are manipulated to the DS1386
 *	 register format.
 */

	struct ds1386 ds1386;

	get_ds1386(&ds1386);

	p->twentyfour_hour = (ds1386.hours & 0x40) ? FALSE : TRUE;  
	p->pm_notam = (ds1386.hours & 0x20) ? TRUE : FALSE;  
	p->seconds = (((ds1386.seconds & 0xf0)>>4)*10) +
		       (ds1386.seconds &0xf);

	p->seconds_alarm = 0;		/* no such field for ds1386 */

	p->minutes = (((ds1386.minutes & 0xf0)>>4)*10) +
		       (ds1386.minutes &0xf);

	p->minutes_alarm = ds1386.minutes_alarm;

	if (p->twentyfour_hour){
	    ds1386.hours &= 0x3f;
	    p->hours = (((ds1386.hours & 0xf0)>>4)*10) +
		       (ds1386.hours &0xf);
	}
	else
	{
	    ds1386.hours &= 0x1f; 	/* mask off am/pm bit as well */
	    p->hours = (((ds1386.hours & 0xf0)>>4)*10) +
		       (ds1386.hours &0xf);
	}

		
	p->hours_alarm = ds1386.hours_alarm;

        p->day_of_week = (((ds1386.day_of_week & 0xf0)>>4)*10) +
                         (ds1386.day_of_week &0xf);

        p->day_of_month = (((ds1386.day_of_month & 0xf0)>>4)*10) +
                       (ds1386.day_of_month &0xf);

        p->month = (((ds1386.month & 0x10)>>4)*10) +
                       (ds1386.month &0xf);

        p->year = (((ds1386.year & 0xf0)>>4)*10) +
                       (ds1386.year &0xf);	

#else
	struct ds1287 ds1287;

     	get_ds1287 (&ds1287);
	p->seconds = ds1287.seconds;
	p->seconds_alarm = ds1287.seconds_alarm;
	p->minutes = ds1287.minutes;
	p->minutes_alarm = ds1287.minutes_alarm;
	p->hours = ds1287.hours & 0x7f; /* mask off am/pm indicator */
	p->hours_alarm = ds1287.hours_alarm;
	p->day_of_week = ds1287.day_of_week;
	p->day_of_month = ds1287.day_of_month;
	p->month = ds1287.month;
	p->year = ds1287.year;
	p->twentyfour_hour = (ds1287.rb & 0x02) ?  TRUE : FALSE; 
	p->pm_notam = (ds1287.hours & 0x80) ? TRUE : FALSE; 

#endif

}

/*
 * Generic form of put_"RTC#" function, calls to devices depending on platform.
 */
void put_date_time(struct toy_date_time *p)
{

#if MEDULLA || CORTEX || UNIVERSE
/* 
 * Note: The "generic" TOY fields are manipulated to the DS1386
 *	 register format.
 */

	struct ds1386 ds1386;

	/* Medulla time updates always leave TOY in 24hr mode */

	get_ds1386(&ds1386); /* Inherit full device data */

	ds1386.seconds = ((((p->seconds) / (UI)10)&0xf)<<4)|
	((p->seconds)-((((p->seconds) / (UI)10)&0xf))*10);


	ds1386.minutes = (((p->minutes / (UI)10)&0xf)<<4)|
	((p->minutes)-(((p->minutes / (UI)10)&0xf))*10);

	ds1386.hours = (( (p->hours / (UI)10) &0xf)<<4)|
	((p->hours)-(((p->hours / (UI)10)&0xf))*10);
 	ds1386.hours &= 0x3f; /*ensure we go to 24hr mode */

	ds1386.day_of_week = p->day_of_week;

	ds1386.day_of_month = (((p->day_of_month / (UI)10)&0xf)<<4)|
	((p->day_of_month)-(((p->day_of_month / (UI)10)&0xf))*10);

	ds1386.month &= 0xc0;  /* clear old month ONLY */

        ds1386.month |= (0x3F & ((((p->month / (UI)10)&0x1)<<4)|
        ((p->month)-(((p->month / (UI)10)&0x1))*10)));

        ds1386.year = (((p->year / (UI)10)&0xf)<<4)|
        ((p->year)-(((p->year / (UI)10)&0xf))*10);

	put_ds1386(&ds1386);
#else
	struct ds1287 ds1287;

	get_ds1287(&ds1287); /* Inherit full device data */

	ds1287.seconds = p->seconds;                      
	ds1287.minutes = p->minutes;
	ds1287.hours = p->hours;
	ds1287.day_of_week = p->day_of_week;
	ds1287.day_of_month = p->day_of_month;
	ds1287.month = p->month;
	ds1287.year = p->year;

     	put_ds1287 (&ds1287);

#endif

}

#if !MEDULLA && !CORTEX && !UNIVERSE
/*
 * Read in the first 12 bytes (time, date, alarm, register A and B) 
 * from the clock chip
 */
int get_ds1287 (struct ds1287 *p) {
	char	*c;
	int	i;
        struct FILE *fp;

	fp = fopen ("toy", "r+");
	if (fp == NULL) 
	    return 0;
	c = p;

	    /* set the SET bit to keep the chip from changing */
	spinlock(&spl_toy);
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) | 0x80);
	
	fseek (fp, 0, SEEK_SET);
	for (i=0; i<13; i++) {
	    fread (c, 1, 1, fp);
	    c++;
	}

	/* clear the set bit */
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) & ~0x80);
	spinunlock(&spl_toy);

	fclose (fp);

	/* range check the month and day */
	if (p->month > 12)
	    p->month = 0;
	if (p->day_of_week > 7)
	    p->day_of_week = 0; 
	return p;
}

/*
 * Write the first 12 bytes (time, date, alarm, register A and B) 
 * to the clock chip
 */
int put_ds1287 (struct ds1287 *p) {
	char	*c;
	int	i;
        struct FILE *fp;

	fp = fopen ("toy", "r+");
	if (fp == NULL) 
	    return 0;
	c = p;

	    /* set the SET bit to keep the chip from changing */
	spinlock(&spl_toy);
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) | 0x80);

	fseek (fp, 0, SEEK_SET);
	for (i=0; i<13; i++) {
	    fwrite (c, 1, 1, fp);
	    c++;
	}

	/* clear the set bit */
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) & ~0x80);
	spinunlock(&spl_toy);

	fclose (fp);
	return p;
}
#endif

#if MEDULLA || CORTEX || UNIVERSE
/*
 * Read the contents from the clock chip
 */
int get_ds1386 (struct ds1386 *p) {
	char	*c;
	int	i;
        struct FILE *fp;

	fp = fopen ("toy", "r+");
	if (fp == NULL){
	 return 0;
	}
	c = p;

	/* clear the TE bit to keep the chip from changing */
	/* Coincidence: same bit in same offset reg as ds1287 */
	spinlock(&spl_toy);
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) & (~0x80));
	
	fseek (fp, 0, SEEK_SET);
	for (i=0; i<sizeof (struct ds1386); i++) {
	    fread (c, 1, 1, fp);
	    c++;
	}

	/* Set the TE bit */
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) | 0x80);
	spinunlock(&spl_toy);

	fclose (fp);

	/* range check the month and day */

	p->month &= 0x1f;	/* make sure you get only month field */

	if (p->month > 0x12)
	    p->month = 0;
	if (p->day_of_week > 7)
	    p->day_of_week = 0; 
	return p;
}

/*
 * Write in the contents to the clock chip
 */
int put_ds1386 (struct ds1386 *p) {
	char	*c;
	int	i;
        struct FILE *fp;

	fp = fopen ("toy", "r+");
	if (fp == NULL) return 0;
	c = p;

	/* Clear the TE bit to keep the chip from changing */
	/* Coincidence: same bit in same offset reg as ds1287 */
	spinlock(&spl_toy);
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) & (~0x80));


	fseek (fp, 0, SEEK_SET);
	for (i=0; i<sizeof (struct ds1386); i++) {
	    fwrite (c, 1, 1, fp);
	    c++;
	}

	/* Set the TE bit */
	write_toy (fp, 0x0b, read_toy (fp, 0x0b) | 0x80);
	spinunlock(&spl_toy);

	fclose (fp);
	return p;
}
#endif

#endif
