
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
__attribute__((unused)) static const char *rcsid = "$Id: lp.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: lp.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:49  fdh
 * Initial revision
 *
 */



/*-----------------------------------------------------------------*/
/*                                                                 */
/*  Combo chip line printer driver for Ultrix  cjm 09/26/91        */
/*                                                                 */
/*-----------------------------------------------------------------*/

#define LP_DATA       0x3BC
#define LP_STATUS     0x3BD
#define LP_CNTRL      0x3BE
#define LP_REG_SELECT 0x170
#define LP_REG_WINDOW 0x171
#define LP_CNTRL0     0x69
#define LP_CNTRL1     0x6A

/* some lp status register bit definitions here */
#define LP_IRQ        0x4
#define LP_NOERROR    0x8
#define LP_SEL        0x10
#define LP_PAPEROUT   0x20
#define LP_NOTACK     0x40
#define LP_READY      0x80

/* some lp control register bit definitions here */
#define LP_STROBE     0x1
#define LP_AUTOFF     0x2
#define LP_INIT       0x4
#define LP_ONLINE     0x8
#define LP_IRQEN      0x10
#define LP_DIRECTION  0x20

/* some vti control0 register bit definitions here */
#define LP_SYSBDEN    0x1
#define LP_FDCSEN     0x2
#define LP_COMAEN     0x4
#define LP_COMADEF    0x8
#define LP_LPTEN      0x10
#define LP_LPTDEF0    0x20
#define LP_LPTDEF1    0x40
#define LP_EMODE      0x80

#define LP_BOGUS      0x1
#define LP_SUCCESS    0x0
#define LP_RETRY      0x3

#define MAXCOL	132
#define CAP	1

/* bits for state */
#define	OPEN		1	/* device is open */
#define	TOUT		2	/* timeout is active */
#define	MOD		4	/* device state has been modified */
#define	ASLP		8	/* awaiting draining of printer */
#define BUF_SIZE        512     /* max character buffer */

#define SPIN(x) for(i=0; i<x; i++ )
char cbuf[BUF_SIZE]; 		/* character buffer */
char *head,*tail;		/* head and tail pointers */
int lp_debug =1;		/* debug switch */



/*---------------------------------------------------------------*/

print_init()
{				/* init maxcol variable */
  int i;
				/* set to extended mode PS2 */ 
  outVti(LP_REG_SELECT,LP_CNTRL0);
  outVti(LP_REG_WINDOW,inVti(LP_REG_WINDOW)&~LP_EMODE);
  if (lp_debug==1) printf("[clp] extented mode enabled\n");
				/* init the controller reg, no ints yet */
  outVti(LP_CNTRL,LP_ONLINE);	/* select printer, drive init low */
  SPIN(1000);			/* wait for unit to wake up */
  outVti(LP_CNTRL,LP_ONLINE|LP_INIT|LP_AUTOFF);
  if (lp_debug==1) printf("[clp] line printer enabled\n");
}

print_out(int n)
{
  char *cpnt = &cbuf[0];
  register int i,s,count=n;

  while (n > 0) {		        /* while data still available */
    outVti(LP_DATA,*cpnt++);		/* send data out */
    outVti(LP_CNTRL,LP_ONLINE|LP_INIT|LP_AUTOFF|LP_STROBE);
    SPIN(1000);				/* clock speed specific! set for ev3 */
    outVti(LP_CNTRL,LP_ONLINE|LP_INIT|LP_AUTOFF);
    n--;				/* decrement the counter */
    while ((inVti(LP_STATUS)&LP_READY)!=LP_READY); 
  }
  if (lp_debug==1) printf("[clp] sent %d characters to printer\n",count);
  return 0;				/* return success */
}


main()
{
  int x,place=0;

  print_init();

  while (1) {
    for (x=0;x<512;x++)		/* stuff with spaces */
      cbuf[x]=0x20;
    cbuf[78]=0xa;
    cbuf[79]=0xd;
/*    strcpy(&cbuf[place++],"Theta printer test"); */
    if (place==60) place = 0;
    print_out(80);
  }
}








