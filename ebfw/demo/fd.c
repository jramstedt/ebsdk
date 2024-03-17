
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

#ifndef LINT
static char *rcsid = "$Id: fd.c,v 1.1.1.1 1998/12/29 21:36:04 paradis Exp $";
#endif

/*
 * $Log: fd.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:04  paradis
 * Initial CVS checkin
 *
 * Revision 1.3  1995/08/25  20:27:27  fdh
 * Removed calls to uart_init() and included "lib.h".
 *
 * Revision 1.2  1994/08/05  20:18:18  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1993/06/08  19:56:46  fdh
 * Initial revision
 *
 */



/*
 * Floppy disk test program. Tests
 * both the floppy disk, and the DMA read and
 * write logic in the memory and cache.
 * Assumptions:
 *   Disk is 3.5 inch, 720K.
 *     This is MFM, 512 byte sectors, 250Kb/sec.
 *   Disk is unit A.
 *   DMA is byte wide.
 *   DMA requests on channel 2.
 *   INT requests on channel 6.
 *     This is IRQ14 at the Intel 82380.
 *     This is level 6 on controller B at te 82380.
 * This version contains the longword hack.
 */
#define	TRUE	1
#define	FALSE	0

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

char	iobuf[3*512];

main()
{
	int	s;
	char	buf[32];

	fdainit();
	fdaspinup();
	printf("\nfd: iobuf = %x\n", (int)iobuf);
	for(;;) {
		fdagets(buf);
		if (buf[0] != 0)
			fdadump(atoi(buf));
	}
}

void
fdagets(char buf[])
{
	int	i;
	int	c;

	printf("? ");
	i = 0;
	for (;;) {
		c = uart_getchar(1);
		if (c=='\r' || c=='\n') {
			printf("\n");
			buf[i] = 0;
			return;
		}
		if (c==0x7F || c=='\b') {
			if (i != 0) {
				printf("\b \b");
				--i;
			}
		} else {
			printf("%c", c);
			buf[i++] = c;
		}
	}
}

int
atoi(char *s)
{
	int	n;

	n = 0;
	while (*s != 0)
		n = 10*n + *s++ - '0';
	return (n);
}

void
fdadump(int s)
{
	int	i;
	int	j;
	int	c;
	int	flag;

	printf("block = %d\n", s);
	for (i=0; i<3*512; ++i)
		iobuf[i] = 0x00;
	flag = fdaread(&iobuf[512], s);
	if (flag == FALSE)
		printf("Read of %d failed!\n", s);
	else {
		printf("Sector %d\n", s);
		for (i=0; i<512; i+=16) {
			printf("%04x", i);
			for (j=0; j<16; ++j)
				printf(" %02x", iobuf[512+i+j]&0xFF);
			printf(" ");
			for (j=0; j<16; ++j) {
				c = iobuf[512+i+j]&0xFF;
				if (c>=0x21 && c<=0x7E)
					printf("%c", c);
				else
					printf(".");
			}
			printf("\n");
		}
	}
}

#define	SRT	0xE0			/* 4 mS				*/
#define	HUT	0x08			/* 256 mS			*/
#define	HLT	0x02			/* 4 mS				*/
#define	ND	0x00			/* Use DMA			*/

void
fdainit()
{
	int	i;
	char	cmd[1];

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

void
fdaspinup()
{
	outportb(FDADCR, DCRENAB|DCRNRES|UNITSEL|UNITMTR);
	fdasleep(1000);
}

void
fdaspindown()
{
	outportb(FDADCR, DCRENAB|DCRNRES);
}

int
fdaread(buf, sec)
char	buf[];
int	sec;
{
	return (fdaio(buf, sec, FDTOMEM));
}

int
fdawrite(buf, sec)
char	buf[];
int	sec;
{
	return (fdaio(buf, sec, MEMTOFD));
}

int
fdaio(buf, sec, type)
char	buf[];
int	sec;
int	type;
{
	int	dhead;
	int	dtrk;
	int	dsec;
	char	cmd[NCMD];
	int	flag;

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
	dtrk  = sec / 18;
	dsec  = sec % 18;
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
	outportb(DMACMD47, 0x00); /* 4, enable */
	outportb(DMACMD03, 0x00); /* 2, enable */
	outportb(DMAMODE47, 0xC0); /* 4, cascade */
	if (type == FDTOMEM)
		outportb(DMAMODE03, 0x46); /* 2, single */
	else
		outportb(DMAMODE03, 0x4A); /* 2, single */
/*
	outportb(DMAXMODE03, 0x0A);
*/
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
	outportb(DMAMASK47, 0x00); /* un-mask 4, cascade */
	outportb(DMAMASK03, 0x02); /* un-mask 2, floppy */
	cmd[0] = (type==FDTOMEM) ? NREAD : NWRITE;
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

void
fdacmd(cmd, ncmd)
char	cmd[];
int	ncmd;
{
	int	i;

	printf("cmd:");
	for (i=0; i<ncmd; ++i)
	  printf(" %02x", cmd[i]&0xFF);
	printf("\n");

	for (i=0; i<ncmd; ++i) {
		while ((inportbxt(FDAMSR)&(MSRRQM|MSRDIO)) != MSRRQM)
			;
		outportb(FDADR, cmd[i]);
	}
}

int
fdasts()
{
	int	nsts;
	int	byte;
	int	i;
	char	sts[NSTS];

	nsts = 0;
	for (;;) {
		while (((byte=inportbxt(FDAMSR))&MSRRQM) == 0)
			;
		if ((byte&MSRDIO) == 0)
			break;
		byte = inportbxt(FDADR);
		if (nsts < NSTS)
			sts[nsts++] = byte;
	}

	printf("sts:");
	for (i=0; i<nsts; ++i)
	  printf(" %02x", sts[i]&0xFF);
	printf("\n");

	if ((sts[0]&ST0IC) != ST0NT) {
		fdactrk = -1;
		return (FALSE);
	}
	return (TRUE);
}

void
fdawait()
{
	int	irr;

	do {
		outportb(PICBOCW, READIRR);
		irr = inportb(PICBIRR);
	} while ((irr&FDAMASK) == 0);
}

void
fdasleep(int nmsec)
{
	int	i;

	while (nmsec--) {
		i = 50000;
		do {
		} while (--i);
	}
}

