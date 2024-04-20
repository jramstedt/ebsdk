/* file:	sound.c
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      sound (speaker output) command
 *
 *  AUTHORS:
 *
 *      Stephen Shirron, Judy Gold
 *
 *  CREATION DATE:
 *  
 *      17-Dec-1993
 *
 *  MODIFICATION HISTORY:
 *
 *      jeg     17-Dec-1993	Initial entry.
 *
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ctype.h"
#include	"cp$src:common.h"
#include	"cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"

#if RAWHIDE
#define eisa_inportb(v) inportb(0,v)
#define eisa_outportb(v,c) outportb(0,v,c) 
#endif

/*+
 * ============================================================================
 * = sound - cause the speaker to emit a sound (or a tune).		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Sound enables the speaker output.  Either a tune, continuous tone, or 
 *	a tone of specific frequency and duration may be emitted.
 *  
 *   COMMAND FMT: sound 2 0 0 sound
 *
 *   COMMAND FORM:
 *  
 *	sound ( [-frequency <value>] [-duration <length>] ) 
 *  
 *   COMMAND TAG: sound 0 RXBZ sound
 *
 *   COMMAND ARGUMENT(S):
 *
 *	None
 *
 *   COMMAND OPTION(S):
 *
 *	-frequency <value> - relative pitch of a sound.  Larger number = higher
 *			     sound
 *	-duration <length> - duration of a sound.  1000 = ~one second
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>>sound		; "The Yellow Rose of Texas" will be played
 *	>>>sound -f 100         ; continuous tone will sound
 *	>>>sound -f 400 -d 1000	; tone will last for ~1 second, 2000 for 2 sec
 *~
 *
 * FORM OF CALL:
 *  
 *	sound (argc, *argv[])
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      A sound will be emitted from the system speaker.
 *
-*/
#define QFREQ 0
#define QDUR  1
#define QMAX  2
	
int sound (int argc, char *argv[]) {
    int freq;
    int dura;
    int icnt;
    struct QSTRUCT qual [QMAX];
    int status;

    status = qscan (&argc, argv, "-", "%dfrequency %dduration", qual);
    if (status != msg_success) return status;

    /* can only have a duration with a given frequency */
    if(qual[QDUR].present && !qual[QFREQ].present)
	return msg_insuff_params;

    /* play "Yellow Rose of Texas" when no qualifiers are present */
    if (!qual[QFREQ].present && !qual[QDUR].present) {
	yellow ();
	return 0;
    }

    /* play a tone of continuous duration if only the -f qualifier is present */
    if (qual[QFREQ].present && !qual[QDUR].present) {
	freq = qual[QFREQ].value.integer;
	icnt = 1193000 / freq;
	eisa_outportb (0x61, eisa_inportb (0x61) | 0x03);
	eisa_outportb (0x43, 0xb6);
	eisa_outportb (0x42, icnt>>0);
	eisa_outportb (0x42, icnt>>8);
	return 0;
    }
    /* play a tone of specified duration with both -f and -d qualfiers */
    else {
	freq = qual[QFREQ].value.integer;
	dura = qual[QDUR].value.integer;
	icnt = 1193000 / freq;
	eisa_outportb (0x61, eisa_inportb (0x61) | 0x03);
	eisa_outportb (0x43, 0xb6);
	eisa_outportb (0x42, icnt>>0);
	eisa_outportb (0x42, icnt>>8);
	krn$_sleep (dura);
	eisa_outportb (0x61, eisa_inportb (0x61) & ~0x03);
	return 0;
    }
    return 0;
}

void msleep(int ms)
{
  krn$_sleep(ms/4);
}

void tone(int period, int level, int time)
{
  eisa_outportb(0x43, 0xb6);
  eisa_outportb(0x42, period & 0xff);
  eisa_outportb(0x42, (period >> 8) & 0xff);
  msleep(time);
  eisa_outportb(0x43, 0xb2);
  eisa_outportb(0x42, 0);
  eisa_outportb(0x42, 0);
}

/*
#define f4e  (6250000 / 706)
#define f6d  (6250000 / 585)
#define f82  (6250000 / 658)
#define f86  (6250000 / 495)
#define f8b  (6250000 / 397)
#define f9c  (6250000 / 331)
#define fa6  (6250000 / 528)
#define fe3  (6250000 / 353)
#define ff6  (6250000 / 440)
*/
#define f4e  (625000 / 706)
#define f6d  (625000 / 585)
#define f82  (625000 / 658)
#define f86  (625000 / 495)
#define f8b  (625000 / 397)
#define f9c  (625000 / 331)
#define fa6  (625000 / 528)
#define fe3  (625000 / 353)
#define ff6  (625000 / 440)

void yellow()
{
/*
  int f4e = 706;
  int f6d = 585;
  int f82 = 658;
  int f86 = 495;
  int f8b = 397;
  int f9c = 331;
  int fa6 = 528;
  int fe3 = 353;
  int ff6 = 440;
*/
  int tbase = 50; /* was 21 */

  eisa_outportb(0x61, 0x03);  /* turn on gate and speaker data for counter 2 */
  tone(f8b,-9, tbase*10);
  tone(fe3,-9, tbase*10);
  tone(f9c,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f8b,-9, tbase*30);
  tone(f8b,-9, tbase*10);
  tone(ff6,-9, tbase*20);
  tone(f8b,-9, tbase*36);
  msleep ( tbase * 4);
  tone(fe3,-9, tbase*20);
  tone(f9c,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(fa6,-9, tbase*30);
  tone(f6d,-9, tbase*10);
  tone(f82,-9, tbase*54);
  msleep ( tbase * 6);
  tone(f82,-9, tbase*20);
  tone(f82,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f82,-9, tbase*20);
  tone(f82,-9, tbase*20);
  tone(f6d,-9, tbase*38);
  msleep ( tbase * 2);
  tone(fa6,-9, tbase*20);
  tone(f86,-9, tbase*20);
  tone(fa6,-9, tbase*20);
  tone(f6d,-9, tbase*30);
  tone(f82,-9, tbase*10);
  tone(f6d,-9, tbase*52);
  msleep ( tbase * 8);
  tone(f8b,-9, tbase*10);
  tone(fe3,-9, tbase*10);
  tone(f9c,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f8b,-9, tbase*30);
  tone(f8b,-9, tbase*10);
  tone(ff6,-9, tbase*20);
  tone(f8b,-9, tbase*28);
  msleep ( tbase * 4);
  tone(fe3,-9, tbase*8);
  tone(f9c,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(fa6,-9, tbase*30);
  tone(f6d,-9, tbase*10);
  tone(f82,-9, tbase*54);
  msleep ( tbase * 6);
  tone(f8b,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f4e,-9, tbase*20);
  tone(f4e,-9, tbase*30);
  tone(f4e,-9, tbase*10);
  tone(f4e,-9, tbase*20);
  tone(f82,-9, tbase*38);
  msleep ( tbase * 2);
  tone(f6d,-9, tbase*20);
  tone(fa6,-9, tbase*20);
  tone(f8b,-9, tbase*20);
  tone(f82,-9, tbase*30);
  tone(f6d,-9, tbase*10);
  tone(fa6,-9, tbase*60);
}
