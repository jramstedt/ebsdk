
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
__attribute__((unused)) static const char *rcsid = "$Id: kbd.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: kbd.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.20  1997/05/01  20:47:37  pbell
 * Removed an unnecessary uart.h reference.
 *
 * Revision 1.19  1997/02/21  03:35:43  fdh
 * Corrected code to read keyboard lock switch status.
 *
 * Revision 1.18  1996/05/22  21:02:00  fdh
 * Modified timing and retries when reading keyboard input.
 *
 * Revision 1.17  1996/01/18  20:56:12  fdh
 * Disabled read of mode register immediately after reset.
 *
 * Revision 1.16  1995/10/31  18:39:50  cruz
 * Moved kbd_error around so it's only compiled when the
 * KERNEL switch is defined.
 *
 * Revision 1.15  1995/10/30  16:55:12  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.14  1995/10/26  23:44:13  cruz
 * Fixed prototypes
 *
 * Revision 1.13  1995/10/12  22:51:56  fdh
 * Increase timeout delay for keyboard input.
 *
 * Revision 1.12  1995/10/10  20:06:57  fdh
 * Updated internal function prototypes.
 *
 * Revision 1.11  1995/10/03  03:21:37  fdh
 * Added kbd_locked().
 *
 * Revision 1.10  1995/09/12  21:35:20  fdh
 * Removed eight bit mas from character returned
 * from keyboard.
 *
 * Revision 1.9  1995/09/02  06:23:07  fdh
 * Included time.h
 *
 * Revision 1.8  1995/09/02  03:41:28  fdh
 * Removed obsoleted macros.
 *
 * Revision 1.7  1995/08/25  19:47:20  fdh
 * Make keyboard driver time out if the keyboard is still
 * sending data after the buffer has been flushed.  If a key
 * is being held down then don't init keyboard and graphics.
 *
 * Revision 1.6  1994/11/08  21:24:46  fdh
 * Corrected timeouts and retries in kbd driver.
 *
 * Revision 1.4  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.3  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.2  1994/06/03  18:36:04  fdh
 * Include prototypes for internal functions.
 *
 * Revision 1.1  1993/06/08  19:56:23  fdh
 * Initial revision
 *
 */

/* Tiny keyboard driver for use by simple
 * clients like the boot rom. Handles both the 84 and
 * the 101 key keyboards. Ignores the mouse and most of the
 * function keys. Handles both left and right control
 * and shift keys. This driver does not look a much like
 * Tom's, but most of Tom's intelectual effort lives
 * on it, for which I am grateful.
 *	dgc, 10-9-91
 */
#include "lib.h"
#include "xlate.h"

/* Internal function prototypes */
static int kbd_locked(void );
static void kbd_process(int scan_code);
static void kbd_leds(void );
static void kbd_ctl_cmd(int c);
static void kbd_ctl_output(int c);
static int kbd_input(void );
static int kbd_ctl_input(void );
static int kbd_output(int c);
static int kbd_output_noverify(int c);
static int kbd_84to101(int c);


/* Programmable Interrupt Controller (PIC) */
#define ICW1			0x20
#define OCW1			0x21
#define KCM     0x40
#define KKSW	0x80
#define	CMD	0x64
#define	RDI	0xC0

#define KBD_INT_ENABLE		0x02	/* 0 = enable */

/* 82C106 */
#define KBD_STAT	0x64
#define KBD_CMD		0x64
#define KBD_DATA	0x60

/* status */
#define	KBD_OBF		0x01
#define	KBD_IBF		0x02
#define	KBD_KBEN	0x10
#define	KBD_ODS		0x20
#define	KBD_GTO		0x40
#define	KBD_PERR	0x80

/* mode */
#define KBD_EKI		0x01
#define KBD_EMI		0x02
#define KBD_SYS		0x04
#define KBD_DKB		0x10
#define KBD_DMS		0x20
#define KBD_KCC		0x40

/* kbd ctl */
#define KBD_CTL_RDMODE	0x20
#define KBD_CTL_WRMODE	0x60
#define	KBD_CTL_TEST1	0xAA
#define	KBD_CTL_TEST2	0xAB
#define	KBD_CTL_DISABLE	0xAD
#define	KBD_CTL_ENABLE	0xAE
#define	KBD_CTL_RDOUT	0xD0
#define	KBD_CTL_WROUT	0xD1
#define KBD_CTL_WRMOUSE	0xD4
#define KBD_CTL_RDPORT  0xC0

/* kbd */
#define	KBD_RESET	0xFF
#define	KBD_MAKEBREAK	0xFC
#define	KBD_SETLEDS	0xED
#define	KBD_SELECTCODE	0xF0
#define	KBD_ENABLE	0xF4
#define	KBD_ACK		0xFA
#define	KBD_RESEND	0xFE
#define	KBD_DEFAULTS	0xF6

/* outport */
#define	HDLED		0x0002

/* prefix */
#define	F0SEEN		0x0001
#define	E0SEEN		0x0002

/* shift */
#define	LFLAG		0x0001
#define	RFLAG		0x0002

static	int	prefix_flags;
static	int	ctrl_flags;
static	int	shift_flags;
static	int	scroll_lock;
static	int	num_lock;
static	int	caps_lock;
static	int	kbd_char;
static	int	kbd_type;


int kbd_init(void)
{
	int	d;
	int	s;
	int	pic_ocw1;

	PRTRACE1("KBD_INT_ENABLE > OCW1\n");

	pic_ocw1 = inportb(OCW1);
	outportb(OCW1, pic_ocw1 | KBD_INT_ENABLE);

	PRTRACE1("Enable and test the keyboard\n");
	kbd_ctl_cmd(KBD_CTL_ENABLE);

/*
	Flush pending input.
*/
	PRTRACE1("Flush pending input.\n");
	d = 0;
	while(kbd_input() != -1) {
	  if (d++ > 29) { /* Should be just a bit more than the Buffer size */
	    printf("\n\tKeyboard initialization Failed\n");
	    return(FALSE); /* Give up eventually */
	  }
	}
	PRTRACE2("Flushed \"%d\" bytes out of Buffer.\n", d);

/*
	The keyboard is not initialized
	if the front panel lock switch is in the locked
	position. This lets you bring the console up on the
	serial port even if a keyboard is plugged in.
*/
#if 0
	PRTRACE1("Checking kbd enable switch\n");
	if (((d=inportb(KBD_STAT))&KBD_KBEN) == 0)
	  {
	    return (FALSE);
	  }
#endif

	PRTRACE1("kbd enable switch on\n");
/*
	Global state.
*/
	prefix_flags = 0;
	ctrl_flags = 0;
	shift_flags = 0;
	caps_lock = FALSE;
	num_lock = FALSE;
	scroll_lock = FALSE;
	kbd_char = -1;
	kbd_type = 84;

/*
	Enable and test the keyboard
	interface in the VTI chip. This seems to
	be the only way to get it going.
*/
	PRTRACE1("Enable and test the keyboard\n");
	kbd_ctl_cmd(KBD_CTL_ENABLE);
	usleep(500000);		/* Give the controller a chance to wake up */

#if 0
/*   Fails on the EB64.  Need to check this later. */
	PRTRACE1("Load Mode register into buffer\n");
	kbd_ctl_cmd(KBD_CTL_RDMODE);
	PRTRACE1("Read Mode register\n");
	s = kbd_ctl_input();
	PRTRACE2("Mode Register = %x\n", s);
#endif

	d = 0;
	do {
	  kbd_ctl_cmd(KBD_CTL_TEST1);
	  s=kbd_ctl_input();
	  PRTRACE2("kbd_ctl_input() returns: %x\n", s);
	  if (s == 0x55) break;
	  PRTRACE1("keyboard test1 FAILED.\n");
	  if (d++ > 10) return (FALSE);
	} while (s != 0x55);
	PRTRACE1("keyboard test1 Passed\n");

	d = 0;
	do {
	  kbd_ctl_cmd(KBD_CTL_TEST2);
	  s=kbd_ctl_input();
	  PRTRACE2("kbd_ctl_input() returns: %x\n", s);
	  if (s == 0x00) break;
	  PRTRACE1("keyboard test2 FAILED.\n");
	  if (d++ > 10) return (FALSE);
	} while (s != 0x00);
	PRTRACE1("keyboard test2 Passed\n");

/*
 *  we have to wake up the interface first before we can
 *  give the keyboard (p16) a jingle
 */

	PRTRACE1("wake up the interface\n");
	while (kbd_input() != -1) ;
	if (kbd_locked()) return FALSE;
	PRTRACE1("keyboard unlocked\n");

/*
	Flip off the HARD DISK light
	on the front panel. This is driven from
	an unused port on the 82C106.
*/
#if	1
	PRTRACE1("Flip off the HARD DISK light\n");
	kbd_ctl_cmd(KBD_CTL_RDOUT);
	d = kbd_ctl_input();
	kbd_ctl_cmd(KBD_CTL_WROUT);
	kbd_ctl_output(d&(~HDLED));
#endif
/*
	Reset keyboard. If the read times out
	then the assumption is that no keyboard is
	plugged into the machine.
*/
	PRTRACE1("Reset keyboard. If the read times out\n");
	if (kbd_output(KBD_RESET) != KBD_ACK)
		 return (FALSE);
	PRTRACE1("Keyboard reset.\n");

	s = kbd_input();
	PRTRACE2("kbd_input() returned: %x\n", s);
	if (s != 0xAA)
	  return (FALSE);
	PRTRACE1("Keyboard reset test passed\n");

	kbd_ctl_cmd(KBD_CTL_RDMODE);
	s = kbd_ctl_input();
	kbd_ctl_cmd(KBD_CTL_WRMODE);
	s &= ~(KBD_EKI|KBD_SYS|KBD_DKB|KBD_KCC);
	kbd_ctl_output(s);

/*
	Assume that we have a 101 key keyboard which
	has powered up in mode 2 (the wrong mode). Switch it into
	mode 3. If we really have an 84 key keyboard then the
	first byte will be ACK'ed as a NOP, and the second byte will
	be RESEND'ed as garbage. On the 101 key keyboard we also
	need to reset the mode of the right ALT and CTRL keys.
*/
	if (kbd_output(KBD_SELECTCODE) != KBD_ACK)
		 return (FALSE);
	PRTRACE1("Keyboard mode is correct\n");
	if (kbd_output_noverify(0x03) == KBD_ACK) {
		if (kbd_output(KBD_DEFAULTS) != KBD_ACK
		|| kbd_output(KBD_MAKEBREAK) != KBD_ACK
		||  kbd_output(0x39) != KBD_ACK
		||  kbd_output(KBD_MAKEBREAK) != KBD_ACK
		||  kbd_output(0x58) != KBD_ACK)
			return (FALSE);
		kbd_type = 101;
	}
	if (kbd_output(KBD_ENABLE) != KBD_ACK)
		 return (FALSE);

	PRTRACE1("Keyboard Initialized\n");
	return (TRUE);
}

#ifdef NOT_IN_USE
void kbd_reset_state(void)
{
	prefix_flags = 0;
	ctrl_flags = 0;
	shift_flags = 0;
	caps_lock = FALSE;
	num_lock = FALSE;
	scroll_lock = FALSE;
	kbd_char = -1;
}
#endif

int kbd_charav(void)
{
	int	s;
	int	d;

	if (kbd_char >= 0)
		return (TRUE);
	s = inportb(KBD_STAT);
	if ((s&KBD_OBF) == 0)
		return (FALSE);
	d = inportb(KBD_DATA);
	if ((s&(KBD_PERR|KBD_GTO|KBD_ODS)) != 0)
		return (FALSE);
	kbd_process(d);
	if (kbd_char >= 0)
		return (TRUE);
	return (FALSE);
}

int kbd_getc(void)
{
	int	c;

	while (kbd_charav() == FALSE)
		;
	c = kbd_char;
	kbd_char = -1;
	return (c);
}

static void kbd_process(int scan_code)
{
	int	c;
	int	prefix;

	if (scan_code == 0xE0) {
		prefix_flags |= E0SEEN;
		return;
	}
	if (scan_code == 0xF0) {
		prefix_flags |= F0SEEN;
		return;
	}
	prefix = prefix_flags;
	prefix_flags = 0;
	/* Macro key */
	if ((prefix&E0SEEN) != 0)
		return;
	if (kbd_type == 84)
		scan_code = kbd_84to101(scan_code);
	if (scan_code > 0x84)
		return;
	switch (xlate[scan_code].char_type) {
	case ASCII:
		if ((prefix&F0SEEN) != 0)
			break;
		c = xlate[scan_code].unshifted;
		if (shift_flags != 0)
			c = xlate[scan_code].shifted;
		if (ctrl_flags != 0) {
			if (c == ' ')
				c = 0x00;
			else if (c>='a' && c<='z')
				c -= 'a'-1;
			else if (c>=0x40 && c<=0x5F)
				c -= 0x40;
			if (c == MCTRL('S')) {
				scroll_lock = TRUE;
				kbd_leds();
			} else if (c == MCTRL('Q')) {
				scroll_lock = FALSE;
				kbd_leds();
			}
		} else if (caps_lock!=FALSE && c>='a' && c<='z')
			c -= 0x20;
		kbd_char = c;
		break;
	
	case NUMPAD:
		if ((prefix&F0SEEN) != 0)
			break;
		c = xlate[scan_code].unshifted;
		if (num_lock != FALSE)
			c = xlate[scan_code].shifted;
		kbd_char = c;
		break;

	case FUNCTION:
		if ((prefix&F0SEEN) != 0)
			break;
		/* ignore */
		break;

	case SCROLL:
		if ((prefix&F0SEEN) != 0)
			break;
		scroll_lock = !scroll_lock;
		kbd_leds();
		if (scroll_lock != FALSE)
			kbd_char = MCTRL('S');
		else
			kbd_char = MCTRL('Q');
		break;

	case CAPS:
		if ((prefix&F0SEEN) != 0)
			break;
		caps_lock = !caps_lock;
		kbd_leds();
		break;

	case NUMLOCK:
		if ((prefix&F0SEEN) != 0)
			break;
		num_lock = !num_lock;
		kbd_leds();
		break;

	case RSHIFT:
		if ((prefix&F0SEEN) != 0)
			shift_flags &= ~RFLAG;
		else
			shift_flags |=  RFLAG;
		break;

	case LSHIFT:
		if ((prefix&F0SEEN) != 0)
			shift_flags &= ~LFLAG;
		else
			shift_flags |=  LFLAG;
		break;


	case RCTRL:
		if ((prefix&F0SEEN) != 0)
			ctrl_flags &= ~RFLAG;
		else
			ctrl_flags |=  RFLAG;
		break;

	case LCTRL:
		if ((prefix&F0SEEN) != 0)
			ctrl_flags &= ~LFLAG;
		else
			ctrl_flags |=  LFLAG;
		break;

	case ALT:
		/* ignore */
		break;
	}
}

static int kbd_84to101(int c)
{
	if (c == 0x01)
		c = 0x47;
	else if (c == 0x03)
		c = 0x27;
	else if (c == 0x04)
		c = 0x17;
	else if (c == 0x05)
		c = 0x07;
	else if (c == 0x06)
		c = 0x0F;
	else if (c == 0x09)
		c = 0x4F;
	else if (c == 0x0A)
		c = 0x3F;
	else if (c == 0x0B)
		c = 0x2F;
	else if (c == 0x0C)
		c = 0x1F;
	else if (c == 0x11)
		c = 0x19;
	else if (c == 0x14)
		c = 0x11;
	else if (c == 0x58)
		c = 0x14;
	else if (c == 0x5D)
		c = 0x5C;
	else if (c == 0x76)
		c = 0x08;
	else if (c == 0x77)
		c = 0x76;
	else if (c == 0x79)
		c = 0x7C;
	else if (c == 0x7B)
		c = 0x84;
	else if (c == 0x7C)
		c = 0x57;
	else if (c == 0x7E)
		c = 0x5F;
	else if (c == 0x83)
		c = 0x37;
	else if (c == 0x84)
		c = 0x00;
	return (c&0x0FF);
}

static void kbd_leds(void)
{
	int	d;

	d = 0x00;
	if (scroll_lock != FALSE)
		d |= 0x01;
	if (num_lock != FALSE)
		d |= 0x02;
	if (caps_lock != FALSE)
		d |= 0x04;
	(void) kbd_output(KBD_SETLEDS);
	(void) kbd_output(d);
}

static void kbd_ctl_cmd(int c)
{
	while ((inportb(KBD_STAT)&KBD_IBF) != 0)
		;
	outportb(KBD_CMD, c);

	while((inportb(KBD_STAT)&KBD_IBF) != 0)
		;
}

static void kbd_ctl_output(int c)
{
	while ((inportb(KBD_STAT)&KBD_IBF) != 0)
		;
	outportb(KBD_DATA, c);
}

static int kbd_ctl_input(void)
{
	while ((inportb(KBD_STAT)&KBD_OBF) == 0)
		;
	return (inportb(KBD_DATA) & 0x0FF);
}

static int kbd_output(int c)
{
	int d;
	int n = 10; /* Resend this many times if necessary */
	do{
	  while ((d=inportb(KBD_STAT)&KBD_IBF) != 0)
	    ;
	  outportb(KBD_DATA, c);
	  if ((d=kbd_input()) != KBD_RESEND)
	    return (d);
	} while(--n);
	return (d);
}

static int kbd_output_noverify(int c)
{

	while ((inportb(KBD_STAT)&KBD_IBF) != 0)
		;
	outportb(KBD_DATA, c);
	return(kbd_input());
}

static int kbd_input(void)
{
	int	n;

	n = 100;		/* Wait up to 1 sec=(100*10ms) for input */
	do {
	  if ((inportb(KBD_STAT)&KBD_OBF) != 0)
	    return (inportb(KBD_DATA) & 0x0FF);
	  msleep(10);
	} while (--n);
	return (-1);
}

static int kbd_locked(void)
{
  kbd_ctl_cmd(RDI);
  if ((kbd_ctl_input()&KKSW) == 0) 
    return 1;
  return 0;
}

#ifdef	KERNEL
static  int	mouse_state;

static void kbd_error(char * error_msg , int s1);

static void kbd_error(char* error_msg, int s1)
{
	printf("KBD: %s %x\n", error_msg,s1);
}

int kbd_get_status(void)
{
   ub status_reg;

   status_reg = inportb(KBD_STAT);

   return( (int)(status_reg & 0xff ) );
}

void kbd_enable_intrs(void)
{
   ub mode;

   kbd_ctl_cmd( KBD_CTL_RDMODE );
   mode = kbd_input();
   mode |= (KBD_EKI);     /* mouse too for now */
   kbd_ctl_cmd( KBD_CTL_WRMODE );
   kbd_output( mode );

}

void kbd_disable_intrs(void)
{
   ub mode;

   kbd_ctl_cmd( KBD_CTL_RDMODE );
   mode = kbd_input();
   mode &= ~(KBD_EKI);    /* mouse too for now */
   kbd_ctl_cmd( KBD_CTL_WRMODE );
   kbd_output( mode );
}

int mouse_getc(void)
{
  int data = inportb( KBD_DATA );

  return( data );
}

void mouse_init(void)
{
	int i;
	int count;
	int reply[3];
	int status;
	int data;

	mouse_state = 1;

	kbd_ctl_cmd(0xa8);	/* Enable mouse in kbd controller. 	*/

/*	initQueue(&mqueue); */

	kbd_ctl_cmd(KBD_CTL_WRMOUSE);	/* Send data to mouse.		*/
	kbd_output(0xFF);		/* Reset.			*/

#ifdef LATER
	i=0; count = 0;
	while(count < 3){
		if(mouse_charav())
			reply[count++] = mouse_getc();
		else if(++i > 1000000){
			mouse_state = 0;
			kbd_error("Mouse init.",0);
			return;
		}
	}
#else
	i=0; count = 0;
	while(count < 3){
	  if( ++i > 1000000 ){
	    mouse_state = 0;
	    kbd_error( "Mouse init", 0 );
	    return;
	  }
	  status = inportb( KBD_STAT );
	  if( !(status & KBD_OBF ) )
	    continue;
	  if( status & KBD_ODS )
	    reply[count++] = inportb(KBD_DATA);
	}

#endif LATER
	PRTRACE2("Mouse init reset reply %x ",reply[0]);
	PRTRACE3("%x %x\n",reply[1],reply[2]);

	kbd_ctl_cmd(KBD_CTL_WRMOUSE);
	kbd_output(0xF4);		/* Tell mouse it's alive.	*/

#ifdef LATER
	i=0; count = 0;
	while(count < 1){
		if(mouse_charav())
			reply[count++] = mouse_getc();
		else if(++i > 1000000){
			mouse_state = 0;
			kbd_error("Mouse init.",0);
			return;
		}
	}
#else
	i=0; count = 0;
	while(count < 1){
	  if( ++i > 1000000 ){
	    mouse_state = 0;
	    kbd_error( "Mouse init", 0 );
	    return;
	  }
	  status = inportb( KBD_STAT );
	  if( !(status & KBD_OBF ) )
	    continue;
	  if( status & KBD_ODS )
	    reply[count++] = inportb(KBD_DATA);
	}


#endif LATER

	PRTRACE2("Mouse init enable reply %x\n",reply[0]);
}
#endif /* KERNEL */
