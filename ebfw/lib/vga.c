
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
__attribute__((unused)) static const char *rcsid = "$Id: vga.c,v 1.1.1.1 1998/12/29 21:36:12 paradis Exp $";
#endif

/*
 * $Log: vga.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:12  paradis
 * Initial CVS checkin
 *
 * Revision 2.9  1996/05/16  14:54:29  fdh
 * Removed use of the inportb alias.
 *
 * Revision 2.8  1996/05/15  20:38:28  fdh
 * Modified vgaputc() to output TABS.
 *
 * Revision 2.7  1996/05/07  23:54:01  fdh
 * Replaced WaitUs with usleep.
 *
 * Revision 2.6  1995/10/31  16:46:44  cruz
 * Clean up for lint.
 *
 * Revision 2.5  1995/10/30  16:57:13  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 2.4  1995/10/26  23:40:25  cruz
 * Lots of cleanup: fixed prototypes, added casting, removed
 * dead code.
 *
 * Revision 2.3  1995/09/02  06:22:26  fdh
 * Included time.h
 *
 * Revision 2.2  1995/08/25  19:47:20  fdh
 * Implemented vgasetup() to be used with VGA Bios
 * emulation.
 *
 * Revision 2.1  1994/11/08  21:29:05  fdh
 * Re-worked by DRT and others...
 *
 * Revision 1.5  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/21  15:47:42  rusling
 * Fixed up WNT compiler warnings.
 *
 * Revision 1.2  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.1  1993/06/08  19:56:26  fdh
 * Initial revision
 *
 */

/*
**++
**  ABSTRACT:
**
**	This module contains all of the VGA support code
**
**  AUTHORS:
**
**      Don Rice - With the help of many people.  This module is has been tested
**	and verified on the ATI PCI device on Morgan, both of the Paradis
**	controllers and the S3 board.  For some reason the initialization
**	numbers that are in this code work on all of the variations and due
**	mostly to the effort of Rob Orlov.
**
**
**  CREATION DATE:     1-Jan-1993
**
**  MODIFICATION HISTORY:
**
**	29-Jun-1993 - the roll in of the ATI numbers that make this module
**	compatible with several VGA controllers.  It just goe to show VGA is
**	really VGA.
**
**	30-Jun-1993 - DTR.  Continuation of the code started yesterday.  From
**	what I can tell, there is no need for the newvga stuff so I pulled all
**	of the conditionals for it.  It appears that the values now used in this
**	code work on the S3, the ATI ISA board, the ATI on board device on
**	Morgan, the two versions of the Paradise controller.  There are more
**	that need to be tried but from all indications this code seems to work.
**
**	12-jul-1993 - DTR.  Got rid of the funny way of setting the vga up.
**	The addresses have been converted from Jensen style to normal ISA
**	I/O addressing and ISA memory addressing.
**
**	5-Aug-1993 - DTR.  Added in the code to support the Matrox MGA graphics
**	adaptor.  The init code can be completely disbled by removing or
**	changing the definition of matrox.
**	
**	
**/

#include "lib.h"
#include "time.h"

#ifndef USENEW
#define LOADGR
#else
#define ATI9X16 /* Only 1 choice currently in USENEW */
#endif /* USENEW */


#define	NROW	25
#define	NCOL	80

/*
 * ISA cycle type masks.
 */

#define	LOBYTE	 (0x00000000)
#define	HIBYTE	 (0x00000001)
#define ENC_BIT2 (0x0000002)

#define B_EN1    (0x0000000)

/*
 * Configuration.  The conditionals here are not currently used.  The values
 *from the ATI device seem to work with all that we have tried.
 */

#define	ATI688000   1
/*	 
**  In order to support all of the board properly, extended mode 3 has been
**  disabled for the time being.  If time allows, ATI extended mode 3 will be
**  worked on later.  Mode 3+ requires VGACLKBITS to be 5
*/	
#define	VGACLKBITS 1

#define	VGA8BIT	1			/* VGA is 8 bit.		*/
#undef VGA8BIT

/*
 * VGA memory and I/O addresses. Any addresses
 * that depend on the value of the IOA bit in the
 * miscellaneous output register have the appropriate value
 * for IOA=1; that is, they have the 0x03Dx form.
 */

#define	VGAGSEG	0xA0000
#define	VGAASEG	0xB8000
#define	VGABASE	VGAASEG


#define GENMO_W 0x03C2      /* misc output write */
#define GENMO_R 0x03CC      /* misc output read */

#define	GENS0	0x3c2
#define	GENS1	0x3da

#define	VGAMORW	0x03C2
#define	VGAMORR	0x03CC
#define	VGAFCRW	0x03DA
#define	VGAFCRR	0x03CA
#define	VGAIS0R	0x03C2
#define	VGAIS1R	0x03DA

#define	VGASEQA	0x03C4
#define	VGASEQD	0x03C5
#define	VGACRTA	0x03D4
#define	VGACRTD	0x03D5
#define	VGAGFXA	0x03CE
#define	VGAGFXD	0x03CF
#define	VGAATRA	0x03C0
#define	VGAATRD	0x03C0
#define	VGADACW	0x03C8
#define	VGADACR	0x03C7
#define	VGADACD	0x03C9
#define	VGADACM	0x03C6

#define GENENA	0x46e8
#define GENVS	0x0102

#define	MEM_CFG	0x5eee
#define	MISC_OPTIONS	0x36ee

/*
 * Colours.
 * 00 Black
 * 01 Blue
 * 02 Green
 * 03 Cyan
 * 04 Red
 * 05 Magenta
 * 06 Brown
 * 07 Light gray
 * 08 Dark gray
 * 09 Light blue
 * 0A Light green
 * 0B Light cyan
 * 0C Light red
 * 0D Light magenta
 * 0E Yellow
 * 0F White
 */
#define	FGROUND	0x0F
#define	BGROUND	0x01
#define	BORDER	0x01

/*
 * Initialization tables.
 */
#define	NSINIT	5
#define	NCINIT	25
#define	NGINIT	9
#define	NAINIT	21

/* the information in the comments is my attempt to document what is really */
/* happening with the values in the table and is taken from the ATI 688000  */
/* technical reference manual */

ub	vgasinit[NSINIT] = {	/* sequencer registers */
	/* 0x09,*/
	0x01,	/* 00: disable character clock and display request.*/
	0x00,	/* 01: clk mode. screen on, every clock, 9-dot. */
	0x03,	/* 02: mapmask. enable maps 1:0. */
	0x00,	/* 03: charmap sel. 1st 8k for a & b. */
	0x02	/* 04: mem mode. Mem >64k. */
};

ub	vgacinit[NCINIT] = {	/* CRTC registers */
	0x5F,	0x4F,	0x50,	0x82,	0x55,
	0x81,	0xBF,	0x1F,	0x00,	0x4F,
	0x0d,	0x0e,	0x00,	0x00,	0x00,
	0x00,	0x9C,	0x8E,	0x8F,	0x28,
	0x1F,	0x96,	0xB9,	0xA3,	0xFF
};

ub	vgaginit[NGINIT] = {	/* graphic ctl regs */
	0x00,	0x00,	0x00,	0x00,	0x00,
	0x10,	0x0E,	0x00,	0xFF
};

ub	vgaainit[NAINIT] = {
	0x00,	0x01,	0x02,	0x03,	0x04,
	0x05,	0x06,	0x07,	0x08,	0x09,
	0x0A,	0x0B,	0x0C,	0x0D,	0x0E,
	0x0F,	0x04,	BORDER,	0x0F,	0x08,
	0x00
};


#ifndef USENEW
extern ub	vga8x16xx[];
extern ub	vga8x16gl[];

#ifdef LOADGR
extern	ub	vga8x16gr[];
#endif

#else
/* USENEW */
#ifdef	ATI9X16
extern	ub	ATI_font_table[];
#endif
#endif

/*
 * Colour map.
 */
struct	{
  ub	r;
  ub	g;
  ub	b;
}	vgadac[16] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x7f },
	{ 0x00, 0x7f, 0x00 },
	{ 0x00, 0x7f, 0x7f },
	{ 0x7f, 0x00, 0x00 },
	{ 0x7f, 0x00, 0x7f },
	{ 0x7f, 0x7f, 0x00 },
	{ 0x7f, 0x7f, 0x7f },
	{ 0x1f, 0x1f, 0x1f },
	{ 0x00, 0x00, 0xff },
	{ 0x00, 0xff, 0x00 },
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0x00, 0x00 },
	{ 0xff, 0x00, 0xff },
	{ 0xff, 0xff, 0x00 },
	{ 0xff, 0xff, 0xff }
};

int	vgarow;
int	vgacol;
long	vgaloc;

#define	MATROX 1

#if	MATROX

#define	    MATROX_REV	0x248
#define	    MATROX_INTREG   0x1c00
#define	    TITAN_ID 0xa26817		/* it's really a2681700 but chip    */
					/* rev is in low byte and as long   */
					/* as I see titan it doesn't matter */

# define    VGA_ATTRIB_ADDR     0x3c0
# define    VGA_ATTRIB_WRITE    0x3c0
# define    VGA_ATTRIB_READ     0x3c1
# define    VGA_MISC            0x3c2
# define    VGA_SEQ_ADDR        0x3c4
# define    VGA_SEQ_DATA        0x3c5
# define    VGA_FEAT            0x3da
# define    VGA_GRAPHIC_ADDR    0x3ce
# define    VGA_GRAPHIC_DATA    0x3cf
# define    VGA_CRT_ADDR        0x3d4
# define    VGA_CRT_DATA        0x3d5
# define    VGA_AUX_ADDR        0x3de
# define    VGA_AUX_DATA        0x3df

#define BT485_WADR_PAL			0x00
#define BT485_RADR_PAL			0x0c
#define BT485_WADR_OVL			0x10
#define BT485_RADR_OVL			0x1c
#define BT485_COL_PAL			0x04
#define BT485_COL_OVL			0x14
#define BT485_PIX_RD_MSK		0x08
#define BT485_CMD_REG0			0x18
#define BT485_CMD_REG1			0x20
#define BT485_CMD_REG2			0x24
#define BT485_STATUS			0x28
#define BT485_CUR_RAM 			0x2c
#define BT485_CUR_XLOW			0x30
#define BT485_CUR_XHI 			0x34
#define BT485_CUR_YLOW			0x38
#define BT485_CUR_YHI 			0x3c
#define BT485_CMD_REG3			0x28

int addrTitan;
int addrDac;
int addrDubic;

#define vgaRam 0xa0000

#define _outp outportb
#define _inp inportb

int MAPSEL[] = {
    0xac000,		/* be carefull with this one.  If the base is a0000 */
			/* then this one isn't there. */
    0xc8000,
    0xcc000,
    0xd0000,
    0xd4000,
    0xd8000,
    0xdc000,
    0
};

#ifdef DEBUG
static ub readreg(ui sel , ui off);
static void writereg(ui sel , ui off , ub dat);
#endif


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      wrDacReg writes the matrox MGA DAC with the specified value.
**
**  FORMAL PARAMETERS:
**
**      reg:
**	    register number to write as a reference to the DAC space.
**	 
**	donne:
**	    data to write.  The tag is derived from the original code from
**	    Matrox.
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The BT485 register is modified
**
**  DESIGN:
**
**      None
**
**
**
**
**
**--
*/
void wrDacReg(int reg, ui donne)
{
    outmemb(addrDac+reg, (ub) donne);
}


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      wrTitanReg writes the specified value to the titan register
**
**  FORMAL PARAMETERS:
**
**      reg:
**	    titan register to write as a reference to the Titan address space.
**	 
**	donne:
**	    data to write.  The tag is derived from the original code from
**	    Matrox.
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The titan register is modified
**
**  DESIGN:
**
**      None
**
**
**
**
**
**--
*/ 
void wrTitanReg(int reg, ui donne)
{
    outmemb(addrTitan+reg, (ub)donne);
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      wrDubic write the specified data to the Matrox board Dubic register
**	file. 
**
**  FORMAL PARAMETERS:
**
**      reg:
**	    Dubic register to write as a reference to the Dubic address space.
**	 
**	donne:
**	    data to write.  The tag is derived from the original code from
**	    Matrox.
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The Dubic register is modified
**
**  DESIGN:
**
**      None
**
**
**--
*/
void wrDubicReg(int reg, ui donne)
{
    outmemb(addrDubic+reg, (ub) donne);
}



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      rdTitanReg read the specified titan register
**
**  FORMAL PARAMETERS:
**
**      reg:
**	    titan register to read as a reference to the titan address space
**
**  RETURN VALUE:
**
**      integer value for the contents of teh register.  Only the low byte has
**	meaning as the register is only an 8 bit register.
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      None
**
**
**
**
**
**--
*/
int rdTitanReg(int reg)
{
    return(inmemb(addrTitan+reg));
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      SetVgaEn enable the VGA capabilities of the MATROX MGA board.  This
**	routine must be called before trying to program the VGA portion of 
**	the board.  Once this is done the board behaves just like a standard 
**	VGA.  A complete description of what this code does is contained in the
**	Matrox Specification for the device.  The routine used here is an almost
**	cut and paste of the routine received from the Matrox company.  The only
**	part of the code that changed was the actual register access.
**
**  FORMAL PARAMETERS:
**
**	none
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The VGA mode on the Titan module is enabled.
**
**  DESIGN:
**
**      None
**
**--
*/

void SetVgaEn(void)
{
    int	temp;

    printf("SetVgaEn entered with addrtitan = %08.8x\n",addrTitan);
    wrTitanReg(0x247,0x8d);
    temp = rdTitanReg(0x253) | 0x10;
    wrTitanReg(0x253,temp);
    wrTitanReg(0x247,0x00);

    wrDubicReg(0,0);
    wrTitanReg(0x240,1);
    usleep(1);

    wrDubicReg(4,0);
    wrDubicReg(8,0);
    wrDubicReg(8,0x80);
    wrDubicReg(8,0);
    wrDubicReg(8,0x81);
    wrDubicReg(4,8);

    wrTitanReg(0x240,0);
    usleep(1);

    wrDacReg(BT485_CMD_REG0, 0x80); /* sequence d'acces cmd 3 */
    wrDacReg(BT485_WADR_PAL, 0x01);
    wrDacReg(BT485_CMD_REG1, 0);
    wrDacReg(BT485_CMD_REG2, 0);
    wrDacReg(BT485_CMD_REG3, 0);
    wrDacReg(BT485_CMD_REG0, 0x00); /* sequence d'acces cmd 3 */
    wrTitanReg(0x251,0x05);
}


#ifdef NOT_IN_USE
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      setupDac performs the dac operations to the BT485 DAC on the Matrox MGA
**	board.  The code currently does not use it because the existing dac init
**	works.  The code writes to the address located in the Matrox board.  I
**	am not sure the reason but I am leaving this code in just in case I need
**	it.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  RETURN VALUE:
**
**      None
**
**  SIDE EFFECTS:
**
**      The BT485 DAC is initialized
**
**  DESIGN:
**
**      None
**
**
**
**
**
**--
*/
void setupDac(void)
{
	int i;
	wrDacReg(BT485_WADR_PAL, 00);
	for(i = 0; i < 0x100; i ++)
	{
	    wrDacReg(BT485_COL_PAL, vgadac[i].r);
	    wrDacReg(BT485_COL_PAL, vgadac[i].g);
	    wrDacReg(BT485_COL_PAL, vgadac[i].b);
	}
}
#endif


/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      locate_matrox scans the address range of the matrox MGA board to see if
**	there is a valid board out there.  It does this by scanning the valid
**	address table and reading the three bytes of data at the base plus the
**	offset to the id.  If the device is present then the data returned will
**	be 0xa2681700 but we don't look at the low byte because that is the
**	version level and as long as it responds with the id it should be
**	sufficient.
**
**  FORMAL PARAMETERS:
**
**      none
**
**  RETURN VALUE:
**
**      Returns the base address or zero.  The code also sets up the base addres
**	of the matrox registers for use later by other routines.
**
**  SIDE EFFECTS:
**
**      none
**
**  DESIGN:
**
**      None
**
**--
*/
int locate_matrox(void)
{

    int	rev_data = 0;
    int	map_index = 0;
    int	loop;
    int	BASE;
    
    addrTitan = 1;	/* forces the loop first time */    

    while ( (BASE = MAPSEL[map_index]) && rev_data != TITAN_ID)
    {
	
	addrTitan = BASE + 0x1c00;

	rev_data = 0;

	for (loop = 3;  loop > 0;  loop--)
	{
	    rev_data <<= 8;
	    rev_data |= (rdTitanReg(MATROX_REV + loop) & 0xFF);
	}		

	if (rev_data != TITAN_ID)
	{
	    map_index++;
	}
    }

    if (BASE)
    {
	addrTitan = BASE | 0x1c00;
	addrDac = BASE | 0x3c00;
	addrDubic = BASE | 0x3c80;
    }
    return (BASE);
}
#endif

#ifdef DEBUG1 
test_main(void)
{
	uart_init();
	vgainit();
}
#endif

/*
 * Initialize the VGA. When all of this
 * is completed the VGA is in 80x25 alphanumeric mode,
 * using an 8x16 character cell, the screen is blank (with the
 * cursor at (0, 0), and a font has been loaded into
 * VRAM map 2.
 */
void vgainit(void)
{
    register int	i;
#ifdef DEBUG
    int j;
#endif

    /*	 
    **  the very first thing we need to do is to configure the ATI device as a
    **	VGA device.  The chip comes up totally disabled.  First we read teh id
    **	at config address 20 and if the id is 41581002 then we have the ati
    **	chip and we have to write the config command data to 3 so that memory
    **	and io are enable in standard VGA mode.  One other test is done to make
    **	sure that the ATI chip is enabled and that is to check config address
    **	offset 8 bit 16:23 for 80(16).  If this is an 80 then the chip should be
    **	left alone as the base video is not enabled.
    */	 

#if MATROX
    if ((i = locate_matrox()) != 0)    
    {
	SetVgaEn();   
    }
#endif

#if 0
    config_data = rdcfg(0x100000, 3);

    if (config_data == 0x41581002)
    {
	/* pick up the enable bit */
    	config_data = rdcfg(0x100008, 3);  
	(void)wrtcfg(0x100004, 3, 3);
    }
#endif

/*	 
**  1.  The first step is to perform a global enable of the device.  The reason
**  there are two separate functions is not clear.
*/	 


  outportb(GENENA, 0x16);		/* Global enable.		*/
  outportb(GENVS, 0x01);
  outportb(GENENA, 0x0e);

#ifdef ARMEN
printf("VI: entering VGA init \n");
#endif

/*									   
** -------------------------------------------------------------------------
**  SPECIAL NOTE.  Although this next section specifically mentions the ATI
**  controller, it doesn't have any adverse effects on other controllers.
** -------------------------------------------------------------------------
*/	 

/*	 
**  2. The second step is to establish the miscellaneous options for the
**  controller.  This can vary from controller to controller but the numbers
**  used now appear to work on all of the VGA systems tested.
*/	 

    i = inportb(MISC_OPTIONS);		/* pick up existing value */
    i |= (0x20 | 0x80);			/* enable 16 bit coprocessor i/o */
    i &= ~0x02;				/* diable host_8_ena (16 bit host   */
					/* data i/o enabled */
    i |= 0x04;				/* 1MB installed on ALPHA Target */
    outportb(MISC_OPTIONS,i);		/* new data to miscellaneous	    */
					/* options */    
    /*	 
    **  Step 3 enable the extended mode registers and setup the memory aperature
    */	 

    /*	 
    **  set VGA extended port address.  (ATI says we need this even for base VGA
    **	compatible operations)
    */	     

    /*	 
    **  Select 0x1ce, offset 2 as the location for the extended VGA registers.
    */	 

    outportb(VGAGFXA, 0x50);
    outportb(VGAGFXD, 0xce);
    outportb(VGAGFXA, 0x51);
    outportb(VGAGFXD, 0x81);

    /*	 
    **  Now select ATI extended register 39 and enable 16-bit mode.
    */	 

    outportb(0x1ce, 0xb9);  /* select reg address */
    outportb(0x1cf, 0x40);  /* enable 16 bit mode */

    /*	 
    **  This next section really needs to be carefully looked at.  Whether it
    **	gets done or not is dependent on the option type and where it lives in
    **	the system
    */	 

    i = inportw(MEM_CFG);	/* existing value */
    i |= (0x02);		/* select 4MB aperature.  This is benign in */
				/* the context of standalone but could be   */
				/* disasterous in the os context */

    outportw(MEM_CFG, i);	/* back out. In some options this is just a */
				/* long nop. */

#ifdef ARMEN
printf("VI: should have just init a bunch of stuff\n");
#endif


    /*	 
    **  Step 3 is to set up the initial sequence values.
    */	 
    for (i=0; i<NSINIT; ++i)
    {
	outportb(VGASEQA, i);		/* see comments in the table.. */
	outportb(VGASEQD, vgasinit[i]);
    }

    /*	 
    **  the placement of this is critical.  It must come before everything that
    **	follows or you don't know what is happening
    */	 


    i = 0x63 | ((VGACLKBITS & 3) << 2);	/* set basic clock from the selcted */
					/* clock information */

    outportb(GENMO_W, i);
    outportb(VGAGFXA, 0x06);		/* set up for gra06 */
    outportb(VGAGFXD, 0x0e);		/* according to the book this odd   */
					/* maps are selected and display    */
					/* buffer mapped into 0xb8000 */

    outportb(VGASEQA, 0x00);		/* address setup */
    outportb(VGASEQD, 0x03);		/* end sync reset */

    outportb(VGACRTA, 0x11);
    outportb(VGACRTD, 0x0e);		/* horizontal scan count that	    */
					/* triggers the end of the V	    */
					/* retrace Pulse */

    /*	 
    **  step 4 is to setup the CRTC register values.  See table and Spec.
    */	 

    for (i=0; i<NCINIT; ++i)
    {
	outportb(VGACRTA, i);		/* see table. */
	outportb(VGACRTD, vgacinit[i]);
    }

#ifdef ARMEN
printf("VI: should have just init a bunch more stuff\n");
#endif


    /*	 
    **  Step 5 is to init the attribute controller.
    */	 

    inportb(VGAIS1R);		/* reset the pointer */

    for (i=0; i<NAINIT; ++i)
    {
	outportb(VGAATRA, i);
	outportb(VGAATRD, vgaainit[i]);
    }

    /*	 
    **  Step 6 is to init the graphics controller
    */	 
    for (i=0; i<NGINIT; ++i)
    {
	outportb(VGAGFXA, i);
	outportb(VGAGFXD, vgaginit[i]);
    }


    /*	 
    **  Step 7 is to set up the VGA clocks.  The value for the time being is set
    **	to 5 but can be changed if need be.
    */	 


#if VGACLKBITS			/* only touch the clocks is there is a	    */
				/* reason to.  The only controller that hsa */
				/* has been verified with extended mode is  */
				/* the ATI688000 */

    outportb(0x1ce,0xbe);	/* sel reg address (ATI3E, pickey	    */
				/* encoding! */
    i = inportb(0x1cf);	 
    i &= ~0x10;			/* clear bit [4] */

#if (VGACLKBITS & 0x08)	        /* check to see if msb set and assert it    */
				/* into the proper position.  This could    */
				/* have been hard coded but it makes more   */
				/* sence to do it this way. */
    {
    	i |= 0x10;
    }
#endif
   
    outportb(0x1ce,0xbe);	/* select reg address (ATI 3e) */
    outportb(0x1cf,i);


    outportb(0x1ce, 0xb9);	/* ATI39 again */
    i = inportb(0x1cf);

    i &= ~2;

#if (VGACLKBITS & 0x04)	        /* if bit 2 is set then we need to make	    */
				/* sure it is in the proper spot.  As with  */
				/* the above value this could have been	    */
				/* hard coded but now it can be changed as  */
				/* need be. */
    {
    	i |= 0x02;
    }
#endif

    outportb(0x1ce, 0xb9);
    outportb(0x1cf, i);		/* re-write */
#endif


    i = inportb(GENMO_R);
    i &= ~0xc;		    /* clear bits 3:2 */

    i |= ((VGACLKBITS & 3) << 2);    /*	low order 2 bits are placed into    */
				     /*	clock select field of register	    */

    outportb(GENMO_W,i);	/* re-write and cause clk select strobe */

    /*	 
    **  step 8 is to set up the dac registers
    */	 

    outportb(VGADACM, 0xFF);	/* dac mask */

    i = inportb(GENS1);		/* comment here says fool attrx */
    
    outportb(VGAATRA, 0x20);

    outportb(VGADACW, 0x00);	/* dac index setup */

    for (i=0; i<16; ++i)
    {
	outportb(VGADACD, vgadac[i].r >> 2);
	outportb(VGADACD, vgadac[i].g >> 2);
	outportb(VGADACD, vgadac[i].b >> 2);
    }

    for (i=0;i<0x18;i++)
    {
	outportb(VGACRTA,i);
	printf("CRTC %x:%x  ",i,inportb(VGACRTD));
	if (i&1) printf("\n");
    }

    vgalcgen();
    vgaerase();
    vgarow = 0;
    vgacol = 0;
    vgasetloc();

#ifdef ARMEN
printf("VI: should have just init even more stuff\n");
#endif

#ifdef DEBUG
    {
      char scroll[]="Now Scrolling.. \n";
      int j;
      j = 0x20;
      for (i=0;i<2000;i++)
	{
	  if (j>0x7e) j=0x20;
	  vgaputc(j++);
	}
      /* hang loose for a bit, then scroll.. */
      for (i=0;i<25;i++)
	{
	  volatile ui j;
	  for (j=0;j<0xfffff;j++) 
	    {
	      ;
	    }
	  for (j=0;scroll[j]!='\0';j++)
	    {
	      vgaputc(scroll[j]);
	    }
	}
    }
#endif /*DEBUG*/

#ifdef ARMEN
    printf("VI: exiting\n");
#endif


}

void vgasetup(void)
{

/*
 *  Place attribute controller into normal mode.
 *  i.e. Connected to CRT controller.
 */
    outportb(VGAATRA, 0x20);

#if 0
      vgalcgen(); /* Load custom Fonts */
#endif
      vgaerase(); /* Initialize Background */
}

/* load the fonts */
void vgalcgen(void)
{
    register long	a;
    register int	i;
    register int	j;
    register ub  *cp;


#ifdef DEBUG
    printf("vgalcgen\n");
    for (i=0;i<0xfffff;i++)
    {
	;
    }
#endif


/*									    
** -------------------------------------------------------------------------
**  SPECIAL NOTE.  Although this next section uses the values for the ATI
**  controller, it doesn't have any adverse effects on other controllers.
** -------------------------------------------------------------------------
*/	 

    /*	 
    **  enable the font load stuff by brute force.  This really should be in a
    **	table but we'll try it this way
    */	 

    outportb(VGASEQA, 0x00);
    outportb(VGASEQD, 0x01);	/* disable character clock and display	    */
				/* requests */

    outportb(VGAGFXA, 0x04);
    outportb(VGAGFXD, 0x02);	/* read map, plane 2 */

    outportb(VGAGFXA, 0x05);
    outportb(VGAGFXD, 0x00);	/* Graphics mode, read mode 0, write mode 0 */

    outportb(VGAGFXA, 0x06);
    outportb(VGAGFXD, 0x04);	/* display buffer at a0000 for 64k graphics */
				/* mode */

    outportb(VGASEQA, 0x02);	/* map mask: enable map 2*/
    outportb(VGASEQD, 0x04);

    outportb(VGASEQA, 0x04);
    outportb(VGASEQD, 0x04);	/* not odd/even. not all of memory,	    */
				/* graphics */

    outportb(VGASEQA, 0x00);	/* end sync reset */
    outportb(VGASEQD, 0x03);


#ifdef DEBUG
    printf("vgalcgen: just tried to enable the fonts\n");
#endif

    a = VGAGSEG;
#ifdef DEBUG
    printf("vgalcgen: a = %x\n",a);
#endif
    for (i=0; i<256; ++i)
    {

/* the following says to use the old 8x16 fonts */
#ifndef USENEW
	cp = &vga8x16xx[0];

	if (i>=0x20 && i<=0x7F)
	{
	    cp = &vga8x16gl[16 * (i-0x20)];
	}
#ifdef DEBUG1
	printf("vgalcgen: a = %x\n",a);
#endif

#ifdef LOADGR
	else if (i>=0xA0 && i<=0xFF)
	{
	    cp = &vga8x16gr[16 * (i-0xA0)];
	}
#endif

	/* 16 lines */
	for (j=0; j<16; j+=2)
	{
	    /* 8 bits */
#ifdef DEBUG1
	    printf("vgalcgen: cp = %x j = %x\n",cp,j);
#endif
	    outmemb((ui)(a|LOBYTE), (ui)cp[j+0]);
	    outmemb((ui)(a|HIBYTE), (ui)cp[j+1]);
	    a += (ENC_BIT2);
	}
	/* 16 unused scan lines must be padded */
	for (j=0; j<16; j+=2)
	{
	    /* byte cycles */
	    outmemb((ui)(a|LOBYTE), (ui)0xFF);
	    outmemb((ui)(a|HIBYTE), (ui)0xFF);
	    a += (ENC_BIT2);
	}
#endif

/* this conditional allows new fonts to be used.  The fonts supported thus  */
/* far are  ATI 9 x 16 font.  As more fonts are available enter them here.  */

#ifdef	USENEW
#ifdef	ATI9X16

	cp = &ATI_font_table[i*32];

	/* 32 lines */
	for (j=0; j<32; j+=2)
	{
	    /* 8 bits */
#ifdef DEBUG1
	    printf("vgalcgen: cp = %x j = %x\n",cp,j);
#endif
	    outmemb(a|LOBYTE, cp[j+0]);
	    outmemb(a|HIBYTE, cp[j+1]);
	    a += (ENC_BIT2);
	}

#endif    
#endif
    }				
#ifdef DEBUG
    printf ("Char Gen Loaded ");
#endif

    /* this disable the font load operation and sets up for the rest of the */
    /* code to work */

    outportb(VGASEQA, 0x00);
    outportb(VGASEQD, 0x01);	/* disable character clock and display	    */
				/* requests */

    outportb(VGAGFXA, 0x04);
    outportb(VGAGFXD, 0x00);	/* read map, enabled based on gra08 */

    outportb(VGAGFXA, 0x05);
    outportb(VGAGFXD, 0x10);	/* odd/even addressing */

    outportb(VGAGFXA, 0x06);
    outportb(VGAGFXD, 0x0e);	/* display buffer at b8000 for 32k graphics */
				/* mode */

    outportb(VGASEQA, 0x02);	/* map mask: enable map 1:0*/
    outportb(VGASEQD, 0x03);

    outportb(VGASEQA, 0x04);
    outportb(VGASEQD, 0x02);	/* Extended memory, charcter map from seq03 */

    outportb(VGASEQA, 0x00);	/* end sync reset */
    outportb(VGASEQD, 0x03);
}

void vgaerase(void)
{
register long	a;
register int	i;
#ifdef DEBUG
    printf("vgaerase\n");
    for (i=0;i<0xfffff;i++)
    {
	;
    }
#endif


    a = VGABASE;
    for (i=0; i<NROW*NCOL; ++i)
    {
	outmemb((ui)(a|LOBYTE), (ui) ' ');
	outmemb((ui)(a|HIBYTE), (ui)(BGROUND<<4)|(FGROUND<<0));
	a += (ENC_BIT2);
    }
}


void vgaputc(register int c)
{
  register int	i;
  register int	j;
  register long	a1;
  register long	a2;

  switch (c) {
  case '\b':
    if (vgacol != 0)
      {
	--vgacol;
	vgasetloc();
      }
    break;

  case '\r':
    vgacol=0;
    vgasetloc();
    break;

  case '\n':
    vgacol = 0;
    if (vgarow < NROW-1)
      {
	++vgarow;
      }
    else
      {
	a1 = VGABASE;
	/* (NCOL%8) == 0 */
	a2 = VGABASE + (NCOL<<1);
	for (i=0; i<NROW-1; ++i)
	  {
	    for (j=0; j<NCOL; ++j)
	      {
#if VGA8BIT
		outmemb(a1|HIBYTE, inmemb(a2|HIBYTE)&0xFF);
		outmemb(a1|LOBYTE, inmemb(a2|LOBYTE)&0xFF);
#else
		outmemw((ui)a1, inmemw((ui)a2)&0xFFFF);
#endif
		a1 += (ENC_BIT2);
		a2 += (ENC_BIT2);
	      }
	  }
	for (j=0; j<NCOL; ++j)
	  {
#if VGA8BIT
	    outmemb(a1|LOBYTE, ' ');
	    outmemb(a1|HIBYTE, (BGROUND<<4)|(FGROUND<<0));
#else
	    outmemw((ui)a1, (ui)(BGROUND<<12)|(FGROUND<<8)|' ');
#endif
	    a1 += (ENC_BIT2);
	  }
      }
    vgasetloc();
    break;

  case '\t':
    do {
      vgaputc(' ');
    } while ((vgacol%8) != 0);
    break;

  default:
    if (isprint(c)) {
      if (vgacol >= NCOL)
	vgaputc('\n');
#if VGA8BIT
      outmemb(vgaloc|LOBYTE, c&0xFF);
      outmemb(vgaloc|HIBYTE, (BGROUND<<4)|(FGROUND<<0));
#else
      outmemw((ui)vgaloc, (ui)(BGROUND<<12)|(FGROUND<<8)|(c&0xFF));
#endif
      if (vgacol < NCOL-1)
	{
	  ++vgacol;
	  vgasetloc();
	}
      else
	{
	  vgasetloc();
	  ++vgacol;
	}
    }
  }
}

void vgasetloc(void)
{
  register int	offs;
  offs = (NCOL*vgarow) + vgacol;
  vgaloc = VGABASE + (offs<<1); /* ends up being <<8, which is every other */
				/* location (skipping the attr bytes..) */
  outportb(VGACRTA, 0x0E);
  outportb(VGACRTD, (offs>>8)&0xFF);
  outportb(VGACRTA, 0x0F);
  outportb(VGACRTD, (offs>>0)&0xFF);
}

/* read the config of the VGA card.. */

#define CRTC 0x3d4

#ifdef	DEBUG

char pr_3cf[][6] = {
        "PR0A","PR0B","PR1","PR2","PR3","PR4","PR5"
                };
char pr_3b5[][6] = {
        "PR10","PR11","PR12","PR13","PR14","PR15","PR16","PR17"
        };
char pr_3c5[][6] = {
        "PR20","PR21"
        };
char pr_3c5a[][6] = {
        "PR30","PR31","PR32"
        };


static dumpvga(void)
{
	ui i;

	/* first, unlock all the registers */
	writereg(0x3ce,0xd,0x2); /* PR3 */
	writereg(0x3ce,0xf,0x5); /* PR5 */
        writereg(0x3d4,0x29,0x85); /* PR10 */
        for (i=0;i<0x19;i++)
		{
		printf("CRTC_x%02x:%02x\t",i,readreg(CRTC,i));
		if ((i&3)==3) printf("\n");
		}

	printf("MiscOut:%02x\t",inportb(0x3cc));
	printf("InSt0:%02x\t",inportb(0x3c2));
	printf("InSt1:%02x\t",inportb(0x3da));
	printf("FeaCtl:%02x\n",inportb(0x3ca));
	for (i=1;i<5;i++)
		{
		printf("SEQ_%02x:%02x\t",i,readreg(0x3c4,i));
		}
	printf("\n");
	for (i=0;i<9;i++)
		{
		printf("GCTL_%02x:%02x\t",i,readreg(0x3ce,i));
		if ((i&3)==3) printf("\n");
		}
        printf("CRTstat:%02x\n",inportb(0x3da));
        /* PR registers.. */
        for (i=9;i<0x10;i++)
                {
                printf("%s:%02x\t",pr_3cf[i-9],readreg(0x3ce,i));
                if(((i-9)&3)==3) printf("\n");
                }
        printf("\n");
        for (i=0x29;i<0x31;i++)
                {
                printf("%s:%02x\t",pr_3b5[i-0x29],readreg(0x3d4,i));
                if(((i-0x29)&3)==3) printf("\n");
                }
	printf("PR32:%02x\n",readreg(0x3c4,0x12));


}


/* read a register whose data register immediately follows the address
  register */

static ub readreg(ui sel, ui off)
{
	outportb(sel,off);
	return((ub)inportb(sel+1));
}

/* write a register whose data register immediately follows the address
  register */
static void writereg(ui sel,ui off,ub dat)
{
	outportb(sel,off);
	outportb(sel+1,dat);
	return;
}
#endif
