#ifndef __TIME_LOADED
#define __TIME_LOADED	1

/* file: time.h --  ANSI C definitions and structures for timekeeping	    */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
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
/* Abstract:	Data structures for keeping time.  This module is intended  */
/* 		to comply with the ANSI C standard (X3J11)		    */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/* 								 	    */
/*	ajb	04-Apr-1992	Initial entry				    */
/* 								 	    */
/*	joes	03-Mar-1993	Added generic time struct toy_date_time and */
/*				struct for DS1386. TOY acesses should now   */ 
/*				be made through the get_date_time() and     */
/*				put_date_time() generic functions in date.c */


#define MAX_CLOCK_SPD_300 310

/*
 * The structure of the dallas 1287 clock chip
 */
struct ds1287 {
	unsigned char seconds;
	unsigned char seconds_alarm;
	unsigned char minutes;
	unsigned char minutes_alarm;
	unsigned char hours;
	unsigned char hours_alarm;
	unsigned char day_of_week;
	unsigned char day_of_month;
	unsigned char month;       
	unsigned char year;
	unsigned char ra;
	unsigned char rb;
	unsigned char rc;
	unsigned char rd;
	unsigned char ram [50];
};

/*
 * The structure of the dallas 1386 clock chip
 * N.B. The remaining 32704 bytes of nvram are accessed through the nvram
 * driver. This is to maintain compatibility with previous TOY implementations
 */
struct ds1386 {
	unsigned char	ticks;
	unsigned char	seconds;
	unsigned char	minutes;
	unsigned char	minutes_alarm;
	unsigned char	hours;
	unsigned char	hours_alarm;
	unsigned char	day_of_week;        
	unsigned char	day_alarm;
	unsigned char	day_of_month;
	unsigned char	month;
	unsigned char	year;
	unsigned char	command;
	unsigned char	watch_dog_alarm_ticks;
	unsigned char	watch_dog_alarm_secs; 
	unsigned char	ram [50];
};

/*
 * Generic TOY data structure
 */
struct toy_date_time {
	unsigned char seconds;
	unsigned char seconds_alarm;
	unsigned char minutes;
	unsigned char minutes_alarm;
	unsigned char hours;
	unsigned char hours_alarm;
	unsigned char day_of_week;
	unsigned char day_of_month;
	unsigned char month;
	unsigned char year;
	unsigned char twentyfour_hour;	/* set (true) if 24 hour clock */
	unsigned char pm_notam;		/* set (true) if time is PM */
};

#endif				/* __TIME_LOADED */
