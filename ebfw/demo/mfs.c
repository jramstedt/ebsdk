
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
__attribute__((unused)) static const char *rcsid = "$Id: mfs.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: mfs.c,v $
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




#define	TRUE	1
#define	FALSE	0
#define REALFLOPPY 1

/* pic */
#define	PICBOCW	0x20
#define	PICBIRR	0x20
#define	READIRR	0x0A
#define	FDAMASK	0x40

/* dma */
#define DMACMD03 0x08
#define DMACMD47 0xD0
#define DMAMODE03 0x0B
#define DMAMODE47 0xD6
#define DMAXMODE03 0x40B
#define DMAXMODE47 0x4D6
#define	DMAOFFS	0x04
#define	DMALPAG	0x81
#define DMAHPAG 0x481
#define	DMACNT	0x05
#define	DMAHCNT	0x405
#define	DMACBP	0x0C
#define DMAMASK03 0x0A
#define DMAMASK47 0xD4

/* fda */
#define	FDADCR	0x03F2
#define	FDAMSR	0x03F4
#define	FDADR	0x03F5
#define	FDADRR	0x03F7
#define	FDADCB	0x03F7

/* dcr */
#define	DCRSELA	0x00
#define	DCRSELB	0x01
#define	DCRSELC	0x02
#define	DCRSELD	0x03
#define	DCRNRES	0x04
#define	DCRENAB	0x08
#define	DCRMTRA	0x10
#define	DCRMTRB	0x20
#define	DCRMTRC	0x40
#define	DCRMTRD	0x80

/* msr */
#define	MSRDIO	0x40
#define	MSRRQM	0x80

/* drr */
#define	DRR500	0x00
#define	DRR250	0x02

/* dcb */
#define	DCBNCHG	0x80

/* st0 */
#define	ST0IC	0xC0
#define	ST0NT	0x00
#define	ST0NR	0x08

/* st1 */
#define	ST1NW	0x02

/* cmd */
#define	NREAD	0x66
#define	NWRITE	0x45
#define	NRECAL	0x07
#define	NSEEK	0x0F
#define	NSINTS	0x08
#define	NSPEC	0x03

#define	FDTOMEM	0
#define	MEMTOFD	1

#define	NCMD	9
#define	NSTS	7

#define	UNITNUM	0
#define	UNITSEL	(DCRSELA)
#define	UNITMTR	(DCRMTRA)

int	fdactrk;

extern	void	fdainit();
extern	void	fdaspinup();
extern	void	fdaspindown();
extern	void	fdacmd();
extern	void	fdawait();
extern	void	fdasleep();
extern	void	fdadump();
extern	void	fdascribble();
extern	void	fdagets();

#define fdREAD  0		               /* read command */
#define fdWRITE 1			       /* write command */


leds(int x)
{
  *(unsigned int*)(0x201000000) = x&0xff;
  mb();
}

void main()
{
  register int  s, x, i,retry;
  register char	*iobuf = (char*) 0x400000;	       /* stick this up in 4 meg region */

  fdainit();
  outportb(FDADCR, DCRENAB|DCRNRES|UNITSEL|UNITMTR);
  fdasleep(1000);
  for (x=0;x<256;x++) {
    while (!(fdaio(&iobuf[x*512],x,FDTOMEM))&&(retry!=3))
      retry++;
  }
  outportb(FDADCR, DCRENAB|DCRNRES);
}

#define	SRT	0xE0			/* 4 mS				*/
#define	HUT	0x08			/* 256 mS			*/
#define	HLT	0x02			/* 4 mS				*/
#define	ND	0x00			/* Use DMA			*/

void fdainit()
{
  register int	i;
  register char	cmd[NCMD];
  
  fdactrk = -1;
  outportb(FDADCR, DCRENAB|DCRNRES);
  fdasleep(5);
  outportb(FDADRR, DRR250);
  cmd[0] = NSINTS;
  fdacmd(cmd, 1);
  (void) fdasts();
  cmd[0] = NSPEC;
  cmd[1] = SRT | HUT;
  cmd[2] = HLT |  ND;
  fdacmd(cmd, 3);
}

int fdaio(buf, sec, type)
     register char	buf[];
     register int	sec;
     register int	type;
{
  register int	dhead;
  register int	dtrk;
  register int	dsec;
  register char	cmd[NCMD];
  register int	flag;
  
  if (fdactrk < 0) {
    cmd[0] = NRECAL;
    cmd[1] = UNITNUM;
    fdacmd(cmd, 2);
    fdawait();
    cmd[0] = NSINTS;
    fdacmd(cmd, 1);
    if (fdasts() == FALSE) {
      /*
       * Assume that the error was a
       * "did not find track 0" error caused by
       * the fact that unless you put the controller into
       * extended track range mode, it will not backstep
       * by more than 77 tracks, which isn't enough to
       * get an 80 track drive all the way back.
       */
      cmd[0] = NRECAL;
      cmd[1] = UNITNUM;
      fdacmd(cmd, 2);
      fdawait();
      cmd[0] = NSINTS;
      fdacmd(cmd, 1);
      if (fdasts() == FALSE)
	return (FALSE);
    }
    fdactrk = 0;
    fdasleep(15);
  }
  dhead = 0;
  dtrk = sec / 18;
  dsec = sec % 18;
  if (dsec >= 9) {
    ++dhead;
    dsec -= 9;
  }
  if (dtrk != fdactrk) {
    cmd[0] = NSEEK;
    cmd[1] = (dhead<<2)|UNITNUM;
    cmd[2] = dtrk;
    fdacmd(cmd, 3);
    fdawait();
    cmd[0] = NSINTS;
    fdacmd(cmd, 1);
    if (fdasts() == FALSE)
      return (FALSE);
    fdactrk = dtrk;
    fdasleep(15);
  }
  /* control */
  outportb(DMACMD47, 0x00);		      /* 4, enable */
  outportb(DMACMD03, 0x00);		      /* 2, enable */
  outportb(DMAMODE47, 0xC0);		      /* 4, cascade */
  outportb(DMAMODE03, 0x46);		      /* 2, single */
  outportb(DMAXMODE03, 0x02);

  /* target address */
  outportb(DMACBP, 0x00);
  outportb(DMAOFFS, (int)buf >>  0);
  outportb(DMAOFFS, (int)buf >>  8);
  outportb(DMALPAG, (int)buf >> 16);
  outportb(DMAHPAG, (int)buf >> 24);
  /* count */
  outportb(DMACBP, 0x00);
  outportb(DMACNT, (512-1) >>  0);	
  outportb(DMACNT, (512-1) >>  8);	
  outportb(DMAHCNT, (512-1) >> 16);	
  /* enable */
  outportb(DMAMASK47, 0x00);		      /* un-mask 4, cascade */
  outportb(DMAMASK03, 0x02);		      /* un-mask 2, floppy */
  cmd[0] = NREAD;
  cmd[1] = (dhead<<2)|UNITNUM;
  cmd[2] = dtrk;
  cmd[3] = dhead;
  cmd[4] = dsec+1;
  cmd[5] = 0x02;
  cmd[6] = 9;
  cmd[7] = 0x1B;
  cmd[8] = 0xFF;
  fdacmd(cmd, 9);
  fdawait();
  flag = fdasts();
  return (flag);
}

void fdacmd(cmd, ncmd)
     register char	cmd[];
     register int	ncmd;
{
  register int	i;
  
  for (i=0; i<ncmd; ++i) {
    while ((inportbxt(FDAMSR)&(MSRRQM|MSRDIO)) != MSRRQM);
    outportb(FDADR, cmd[i]);
  }
}

int fdasts()
{
  register int	nsts;
  register int	byte;
  register int	i;
  register char	sts[NSTS];
  
  nsts = 0;
  for (;;) {
    while (((byte=inportbxt(FDAMSR))&MSRRQM) == 0);
    if ((byte&MSRDIO) == 0)
      break;
    byte = inportbxt(FDADR);
    if (nsts < NSTS)
      sts[nsts++] = byte;
  }
  if ((sts[0]&ST0IC) != ST0NT) {
    fdactrk = -1;
    return (FALSE);
  }
  return (TRUE);
}

void fdawait()
{
  register int	irr;
  
  do {
    outportb(PICBOCW, READIRR);
    irr = inportb(PICBIRR);
  } while ((irr&FDAMASK) == 0);
}

void fdasleep(int nmsec)
{
  register int	i;
  
  while (nmsec--) {
    i = 50000;
    do {
    } while (--i);
  }
}





