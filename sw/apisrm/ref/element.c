/* file:	element.c
 *
 * Copyright (C) 1996, 1997 by
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
 *      Alpha Firmware
 *
 *  ABSTRACT:
 *
 *	Parse element from a delimited list
 *
 *  AUTHORS:
 *
 *      Bill Luquette
 *
 *  CREATION DATE:
 *  
 *      10-mar-1995
 *
 *  MODIFICATION HISTORY:
 *
 *	seh	15-Jan-1996	Corrections to command header for SRM update.
 *
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

/*+
 * ============================================================================
 * = element - Parse element from a delimited list.                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Element is a utility for extracting an element from a string of 
 *   	elements on a single line or from an array in the form of columnized
 *	data.
 *
 *   COMMAND FMT: element 4 0 0 element
 *
 *   COMMAND FORM:
 *  
 *	element ([-i <index>] [-d <delimiter>] [-l <line>] [-t <terminator>])
 *
 *   COMMAND TAG: element 0 RXBZ element
 *  
 *   COMMAND ARGUMENT(S):
 *
 *      none          - stdin is parsed 
 *
 *   COMMAND OPTION(S):
 *
 *	i - index of element to be returned   default is 0
 *	d - delimiter character(s).           default is <space><tab>
 *	t - type of output line terminator    default is "/n"; -t = " "
 *	l - line number of stdin to parse     default is 0 (all lines)
 *
 *   COMMAND EXAMPLE(S):
 *
 *	Using element to get a list of console devices.
 *~
 *	>>> show device
 *	polling for units on kfmsa0, slot 1, xmi0...
 *	dua1.1.0.1.2       RF3101$DIA1               RF72
 *	dua3.3.0.1.2       RF3103$DIA3               RF72
 *	dub0.0.1.1.2       R2WUIC$DIA0               RF72
 *	dub5.5.1.1.2       RF3111$DIA5               RF31
 *	>>> show device | grep -v "polling" | element
 *	dua1.1.0.1.2
 *	dua3.3.0.1.2
 *	dub0.0.1.1.2
 *	dub5.5.1.1.2
 *~
 *	Using element to get the 2nd column of the show device command.
 *~
 *	>>> show device | grep -v "polling" | element -i 1
 *	RF3101$DIA1
 *	RF3103$DIA3
 *	R2WUIC$DIA0
 *	RF3111$DIA5
 *~
 *	Using element to get a list of abbreviated device names
 *~
 *	>>> show device | grep -v "polling" | element -d "."
 *	dua1
 *	dua3
 *	dub0
 *	dub5
 *~
 *	Using "-l" to get the first device name.
 *~
 *	>>> show device | grep -v "polling" | element -d "." -l 1
 *	dua1
 *~
 *	Using "-l" to get the second device name.
 *~
 *	>>>  show device | grep -v "polling" | element -d "." -l 2
 *	dua3
 *	>>>
 *~
 *	Using -t to change the line terminator from a (Carriage_Return/
 *	Line_Feed) to a (Space).
 *~
 *	>>> sho conf
 * 
 *	      Name     Type    Rev   Mnemonic
 *	LSB
 *	2+    KA7AA    (8002)  0000  ka7aa0
 *	7+    MS7AA    (4000)  0000  ms7aa0
 *	8+    IOP      (2000)  0003  iop0
 * 
 *	C2 XMI                       xmi0
 *	1+    KFMSA    (0810)  A1A6  kfmsa0
 *	6+    DWLMA    (102A)  0003  dwlma0
 *	A+    KFMSA    (0810)  A4A6  kfmsa1
 *	B+    KDM70    (0C22)  1E01  kdm700
 *   	D+    DEMNA    (0C03)  0802  demna0
 *   	E+    DEMNA    (0C03)  0802  demna1
 *
 *	>>> sho conf | element -i 1 -t " "
 *
 *	Type KA7AA MS7AA IOP XMI KFMSA DWLMA KFMSA KDM70 CIXCD DEMNA >>>
 *	>>>
 *~
 *	Using -d to specify multiple delimiters.
 *~ 
 * 	>>> cat test_file
 * 	1 2 3 4 5 6
 * 	2 3 4 5 6 7
 * 	3 4 5 6 7 8
 * 	4 5 6 7 8 9
 *~ 
 *~ 
 * 	>>> cat test_file | element  -d "5"
 *	1 2 3 4
 *	2 3 4
 *	3 4
 *	4
 *~ 
 *~ 
 * 	>>> cat test_file | element  -d "25"
 *	1
 *	2 3 4
 *	3 4
 *	4
 *	>>>
 *~
 *~ 
 * 	>>> dynamic
 * 	zone     zone       used    used       free    free       util-   high
 * 	address  size       blocks  bytes      blocks  bytes      zation  water
 * 	-------- ---------- ------- ---------- ------- ---------- ------- ----------
 * 	0008C050 524288     231     229952     11      294368      43 %   370656
 * 	000F5180 6291456    36      11136      4       6280352      0 %   11168
 * 	00824D60 58569184   1       32         1       58569184     0 %   0
 *~ 
 * 	Using element to return the size of memzone in hex bytes.
 *~
 * 	>>> eval  -x  ` dynamic | element -i 1 -l 6`
 * 	037db1e0
 *~ 
 *	Using element to return the size of memzone in decimail megabytes.
 *~ 
 * 	>>>  eval  ` dynamic | element -i 1 -l 6` 1024  1024 \* /
 * 	55
 *~ 
 *
 * FORM OF CALL:
 *  
 *	element( argc, *argv[] )
 *
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 *      int argc - number of command line arguments passed by the shell
 *      char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
#define	QI	0
#define	QD	1
#define	QL	2
#define	QT	3
#define	QMAX	4

int element (int argc, char *argv []) 
{
    struct QSTRUCT qual [QMAX];
    struct PCB	*pcb;      

    char line[256];
    char *str;
    char elem[256];
    char *del;			/* delimiter */
    char terminator[80];
    int cnt, index, num, size, i;

    /*
     * parse command qualifiers
     */
    qscan (&argc, argv, "-", "%di %sd %dl %st", qual);

    /*
     * Get index, delimiter
     */
    if (qual [QI].present) index = qual [QI].value.integer;
	else index = 0;
    if (qual [QD].present) del = qual [QD].value.string;
	else del = "	 ";
    if (qual [QL].present) num = qual [QL].value.integer;
	else num = 0;
    if (qual [QT].present) strcpy(terminator, "%s ");
	else strcpy(terminator, "%s\n");

    pcb = getpcb();

    if (strcmp("tta0", pcb->pcb$a_stdin_name) == 0)
	return msg_success;

    for (cnt = 0; (fgets (line, sizeof(line), pcb->pcb$a_stdin) != NULL);) {
	cnt++;
	size = strlen(line);
	if ((strelement(line,index,del,elem) != NULL) && ((num == 0) | (cnt == num)))
               printf(terminator, elem);
    }

    return msg_success;
}
