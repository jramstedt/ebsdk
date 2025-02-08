
/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: getver.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: getver.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1995/12/15  21:51:37  cruz
 * Updated to obtain the firmware version from the newly created
 * variable "dbm_version".
 *
 * Revision 1.6  1995/11/15  16:07:18  cruz
 * Updated Firmware version to 2.0
 *
 * Revision 1.5  1995/04/20  18:38:08  cruz
 * Updated EBSDK version to 1.5
 *
 * Revision 1.4  1995/02/28  15:08:05  fdh
 * Include <string.h> and provide a return value for main.
 *
 * Revision 1.3  1995/02/27  22:46:44  cruz
 * Changed code to avoid using LONGs which caused
 * problems with the scanf routine.
 *
 * Revision 1.2  1995/02/27  19:02:13  cruz
 * Use int instead of long to get around a problem
 * with the mod operator.
 *
 * Revision 1.1  1995/02/27  18:18:52  cruz
 * Initial revision
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char compile_date[], compile_time[], dbm_version[];

char *FirmwareVersion = dbm_version;
char *FirmwareBuildDate = compile_date;
char *FirmwareBuildTime = compile_time;

int main ()
{
    char Buffer[25];
    char BuildTimeStamp[13];
    char Colon;
    char MonthString[5];
    char *MonthName[] =  {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int MonthNumber;
    int Day;
    int Year;
    int Hour;
    int Minute;
    int MajorVersion;
    int MinorVersion;
    unsigned int DateLong;
    unsigned int TimeLong;

    /*
    // Isolate the major, minor, and platform version numbers
    // (major.minor-platform)
    */
    sscanf(FirmwareVersion, "%u.%u",
           &MajorVersion,
           &MinorVersion);

    /*
    // Construct the BuildTimeStamp (yymmdd.hhmm)
    */

    strcpy(Buffer,FirmwareBuildDate);
    strcat(Buffer," ");
    strcat(Buffer,FirmwareBuildTime);

    sscanf(Buffer,"%s %d %d %d %c %d",
       MonthString,
       &Day,
       &Year,
       &Hour,
       &Colon,
       &Minute);

    for (MonthNumber = 0;
     MonthNumber < sizeof(MonthName)/sizeof(MonthName[0]);
     MonthNumber++) {
        if (strcmp(MonthString,MonthName[MonthNumber]) == 0) {
            MonthNumber++;
            break;
        }
    }

    sprintf(BuildTimeStamp,"%02d%02d%02d.%02d%02d",
        Year%100,
        MonthNumber,
        Day,
        Hour,
        Minute);

    sscanf(BuildTimeStamp,"%d.%d", &DateLong, &TimeLong);

    /*
    // print the info
    */
    printf("%02.2d%02.2d%02.2d%02.2d%02.2d%02.2d%02.2d%02.2d\n", 
           MajorVersion, 
           MinorVersion, 
           0,
           Year%100, 
           MonthNumber,
           Day,
           Hour,
           Minute);
    printf("Version: %s %s\n", 
            FirmwareVersion, 
            BuildTimeStamp);

    return(EXIT_SUCCESS);
}
