/* file:	printf.c
 *
 * Copyright (C) 1992, 1993 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *
 *	This module implements various printf functions.  All variants of
 *	printf share the common formatting language:
 *~
 *	%s	Print variable length string.  String is null terminated.
 *
 *	%c	Print character.
 *
 *	%b	print unsigned number in binary
 *
 *	%d	Print signed number in decimal with a default of 5 digits.
 *
 *	%u	print unsigned decimal number.
 *
 *	%x	Print hex number with a default of 8 digits of precision.  As
 *		many bytes will be converted as necessary to satisfy the
 *		precision specifier.  Print out any alpha characters in
 *		the same case as X.
 *
 *	%o	Print number in octal notation.
 *
 *	%z	Don't print terminating nulls in sprintf.
 *
 * [[-]width[.precision]]
 *
 *		    All of the above percent commands may also have an
 *		optional width.precision specifier.
 *
 *		    If just a number is present between the percent sign
 *		and the conversion specifier, the number is treated as a
 *		field width.  The precison will then be the number of
 *		significant digits in the number or string.
 *
 *		    If the minus sign is present the result is left justified
 *		in the field, otherwise the result is right justified.
 *
 *		    Width is the total field width of the converted data
 *		structure.  The conversion won't exceed this width.
 *		If the field width has a leading zero on it (i.e. %08X), then
 *		leading zeros will appear on the conversion.  Otherwise 
 *		leading zeros will be suppressed.
 *
 *		    Precision is the number of significant characters to be
 *		converted.  This number can't exceed the field width.  Results
 *		are undefined if it does.
 *
 *		    Both the width and the precison may be encoded as asterisks
 *		in the format string, in which case the data is obtained from
 *		the argument list.
 *
 *		    The width.precision specification occurs between the 
 *		percent sign and the format type, i.e %-12.3s would left
 *		justify the first 3 characters of the string s in a field
 *		12 columns wide.  %023.3X would print the lower 3 nibbles
 *		of a number in hex, right justifying this number in a 23
 *		column field.  Leading zeros would be printed, and all
 *		non-numeric digits would be in upper case.
 *~
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      01-Jan-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	twp	30-Mar-1996	Conditionalize for RHMIN
 *
 *	mdr	30-Sep-1993	add Medulla Debug support 
 *
 *	jrk	20-Sep-1993	Remove message numbers
 *
 *	jrk	13-Sep-1993	Overlay integration
 *
 *	jrk	09-Sep-1993	add pcrlf routine
 *
 *	jeg	01-Jun-1993	Filled in addr_trans() field in PF_CONTEXT with
 *				a no-op routine, no_translate().  This field is
 *				used to translate virtual addresses into 
 *				physical addresses in the ARC console context.
 *
 *      dda     18-Feb-1993     Implemented environ var d_report = "off" flag by
 *                              adding code in err_printf and err_printf_nolock
 *                              routines to check for absence of "SUMM" or 
 *                              "FULL" flags in PCB->diag_evs word.
 *
 *	pel	13-Oct-1992	err_printf & err_printf_nolock; do like qprintf
 *				if not screen mode & doing pre-prompt stuff to
 *				eliminate garbled output messages.
 *
 *	ajb	05-Aug-1992	Tee all err_printf output to the event logger.
 *
 *	ajb	10-Feb-1992	Work around sign extention bug in 014 compiler
 *				in convert_number.
 *
 *	ajb	29-Jan-1992	accept and convert 64 bit integers
 *
 *	ajb	12-Dec-1991	Remvoe all references to messages numbers.
 *				Keep the %m construct as a synonym for %s
 *				until all sources that use %m are changed.
 *
 *	ajb	11-Dec-1991	Change some names to avoid conflicts with
 *				the message name space.
 *
 *	ajb	17-May-1991	Added %z specifier.  This should be present as
 *				the first specifier in the format string.  Its
 *				presence suppresses the insertion of the
 *				terminating null in sprintf.
 *
 *	ajb	25-Feb-1991	Insure sprintf buffer is null terminated even
 *				when pass null strings.
 *
 *	pel	20-Feb-1991	add err_printf_nolock
 *
 *	mrc	05-Feb-1991	Increase number of arguments to printf to 24,
 *				and add COPY_PRINTF_ARGS macro.
 *
 *	ajb	24-Jan-1991	Let drivers translate newlines
 *
 *	ajb	21-Jan-1991	Buffer output on regular printfs
 *
 *	pel	06-Dec-1990	add f/printf_nolock functions.
 *
 *	pel	30-Nov-1990	add err_printf function.
 *
 *	pel	28-Nov-1990	access stdin/out/err differently.
 *
 *	pel	20-Nov-1990	Add fprintf.
 *
 *	ajb	31-Oct-1990	Use more portable constructs for variable 
 *				length argument lists.
 *
 *	ajb	11-Oct-1990	Special case certain messages
 *
 *	ajb	02-Oct-190	Add %m specifier
 *
 *	ajb	11-Sep-1990	Fix %d specifier
 *
 *	pel	29-Aug-1990	Make the output of printf contiguous.
 *
 *	ajb	24-Aug-1990	Use tt_out_polled for pprintf
 *
 *	ajb	23-Aug-1990	Use nargvalue, not nargloc.  Latter construct
 *				is a VAXism.
 *
 *	ajb	07-Jul-1990	Autodoc updates
 *--
 */


#include	"cp$src:platform.h"
#include	"cp$src:ctype.h"
#include	"cp$src:stddef.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:common.h"
#include	"cp$src:ovly_def.h"
#include	"cp$src:printf.h"
#include	"cp$src:varargs.h"
#include	"cp$src:tt_def.h"
#include	"cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

#define LOCAL_DEBUG 0

#define LF 10
#define CR 13
/*
 * if flag == 0, normal printf
 * if flag != 0, do pprintf for printf and err_printf
 */
int print_debug_flag = 0;
int print_enable_page = 0;	/* no page mode to start */
int print_cpu_flag = -1;	/* all cpus */

/* Checks diag_evs word for SUMM and FULL*/
#define ev_mask (m_rpt_summ|m_rpt_full)   

#if MODULAR
extern int display_ForegroundColor;
#endif

extern int screen_mode;
extern struct TTPB *console_ttpb;
#if !TURBO
extern int cpip;	/* cpu powerup in progress */
#else
extern int pipp;
#endif

extern int null_procedure ();
char specifierlist [] = "sxdcbumoz";	/* in no particular order */

#define	SPECIFIER_BIAS	0x100	/* must be bigger than character set size */

extern int diag$r_evs;
extern struct LOCK spl_printf;
#if TURBO
extern int running_from_bcache;
#endif

static void lock_printf(void)
{
#if TURBO
    if (running_from_bcache)
	lock_tlsb();
    else
	spinlock (&spl_printf);
#else
    spinlock (&spl_printf);
#endif
}
static void unlock_printf(void)
{
#if TURBO
    if (running_from_bcache)
	unlock_tlsb();
    else
	spinunlock (&spl_printf);
#else
    spinunlock (&spl_printf);
#endif
}

/*----------*/
INT nextarg (struct PF_CONTEXT *ptx) {

	return va_arg (ptx->argptr, INT);
}

void pcrlf()
{
	printf("\n");
}

/*
 * When printing, we align the strings on multiples
 * of PC_ALIGNMENT columns.
 */
#define PC_ALIGNMENT	16
#define PC_SCREEN_WIDTH	80

void column_print(char *string, int *column)
{
    unsigned int len;
    unsigned int padding;

    len = strlen(string);
/*
 * Calculate the padding required to get us to the next aligned column.
 */
    padding = PC_ALIGNMENT - *column % PC_ALIGNMENT;
    if (*column == 0)
	padding = 0;
/*
 * Advance to the next line if we'll exceed the current line
 */
    if (*column && ((len + *column + padding) >= PC_SCREEN_WIDTH)) {
	printf("\n");
	*column = 0;
	padding = 0;
    }

    printf("%*.*s%s", padding, padding, "", string);
    *column += padding + len;
}

/*
 * All printf output is funneled through this routine.  It sends the current
 * characters to the defined output routine.  This routine keeps a cached copy
 * for use by the event logger (qprintf, err_printf, err_print_nolog).
 */
void printf_putchar (struct PF_CONTEXT *ptx, unsigned char c) {

	/* keep a cached copy */
	if (ptx->ttbuf_index < sizeof (ptx->ttbuf)) {
	    ptx->ttbuf [ptx->ttbuf_index++] = c;
	}

	/* Write it to the event log if told to. */
	if (ptx->el_write)
	    el_writeline (&c, 1);

	/* call the printf specific back end */
	(*ptx->out) (ptx, c);

	ptx->count++;
}

/*----------*/
void store_file (struct PF_CONTEXT *ptx, char c)
{
	if (c == LF)
	    printf_flush (ptx);

	if (ptx->ttbuf_index >= sizeof (ptx->ttbuf)) {
	    printf_flush (ptx);
	}
}

/*----------*/
void store_tt_poll (struct PF_CONTEXT *ptx, unsigned char c)
{
    tt_out_polled (c);
#if MODULAR
    if (c == LF)
	printf_more(ptx->fp, 1);
#endif
}


/*----------*/
void store_buf (struct PF_CONTEXT *ptx, char c) {
	*ptx->obuf++ = c;
}

/*----------*/
#if MODULAR
static char more_msg[] = "--More-- (SPACE - next page, ENTER - next line, Q - quit)";
static char bold_on[] = {'\33', '\133', '\61', '\155', '\0'};
static char bold_off[] = {'\33', '\133', '\60', '\155', '\0'};
static char red_on[] = {'\33', '\133', '\63', '\61', '\155', '\0'};
static char foreground_on[] = {'\33', '\133', '\63', '\67', '\155', '\0'};
static char clr_msg[] = "\033[2K\033[256D";
static char crlf_msg[] = "\r\n";

void printf_msg(struct TTPB *ttpb, char *msg, int polled)
{
    char *msg_ptr;
    char c;

    msg_ptr = msg;

    while (1) {
	c = *msg_ptr++;
	if (c == 0)
	    break;
	while (!ttpb->txready (ttpb->port));
	ttpb->txsend (ttpb->port, c);
    }
}

int printf_getchar(struct TTPB *ttpb, int polled)
{
    struct TTYQ *rxqp;
    char c;

    if (polled) {
	while (!ttpb->rxready(ttpb->port));
	c = ttpb->rxread(ttpb->port);
    } else {
	rxqp = &ttpb->rxq;
	while (rxqp->in == rxqp->out)
	    krn$_wait(&rxqp->ready);
	c = rxqp->buf[rxqp->out];
	rxqp->out = (rxqp->out + 1) & rxqp->qmask;
    }
    return c;
}

void printf_more(struct FILE *fp, int polled)
{
    int attrib;
    int console_page = 0;
    int console_type = -1;
    struct TTPB *ttpb;
    char c;

    if (!print_enable_page)
	return;
/*
 * Set console_type
 *	0 = video terminal
 *	1 = hardcopy terminal
 *	2 = graphics in video mode
 *	3 = graphics in hardcopy mode
 */
    if (polled) {
	ttpb = console_ttpb;
	    console_page = ttpb->page;
	    console_type = ttpb->type;
	    ttpb->linecnt++;
    } else {
	if ((strncmp (fp->iob.name, "tta0", 4) == 0) ||
	    (strncmp (fp->iob.name, "tga", 3) == 0) ||
	    (strncmp (fp->iob.name, "vga", 3) == 0)) {
	    ttpb = fp->ip->misc;
	    console_page = ttpb->page;
	    console_type = ttpb->type;
	    ttpb->linecnt++;
	    if (strncmp (fp->iob.name, "tga", 3) == 0)
		console_type |= 2;
	    if (strncmp (fp->iob.name, "vga", 3) == 0)
		console_type |= 2;
	}
    }

    if (console_page != 0) {
	if (ttpb->linecnt >= console_page) {

/* print more msg */

	    switch (console_type) {
		case 0:			/* serial video */
		    printf_msg(ttpb, bold_on, polled);
		    printf_msg(ttpb, more_msg, polled);
		    printf_msg(ttpb, bold_off, polled);
		    break;

		case 2:			/* graphics video, make red */
		    foreground_on[3] = display_ForegroundColor + '0';
		    printf_msg(ttpb, red_on, polled);
		    printf_msg(ttpb, more_msg, polled);
		    printf_msg(ttpb, foreground_on, polled);
		    break;
		case 1:			/* serial hardcopy */
		case 3:			/* graphics hardcopy */
		    printf_msg(ttpb, more_msg, polled);
		    break;
	    }

/* wait for input, space - allow next page, cr - allow 1 line */
/*		   sp - next page, ^D - next page, 0 - turn off page mode */
/*		   ^C - done, ^P - done, ^Y - done. */

	    while (1) {
		c = printf_getchar(ttpb, polled);

		if ((c == 0x10) ||		/* control-p */
		    (c == 0x19) ||		/* control-y */
		    (c == 0x03) ||		/* control-c */
		    (c == 'Q') ||
		    (c == 'q')) {
			control_c_action (ttpb);
			ttpb->linecnt = 0;
			break;
		}
		if (c == 4) {		/* ^D */
		    ttpb->linecnt = 0;
		    break;
		}
		if (c == '0') {		/* zero */
		    ttpb->linecnt = 0;
		    ttpb->page = 0;
		    break;
		}
		if (c == ' ') {		/* space */
		    ttpb->linecnt = 0;
		    break;
		}
		if (c == CR)		/* cr */
		    break;
	    }

/* clear more msg */

	    switch (console_type) {
		case 0:			/* serial video */
		case 2:			/* graphics video */
		    printf_msg(ttpb, clr_msg, polled);
		    break;
		case 1:			/* serial hardcopy */
		case 3:			/* graphics hardcopy */
		    printf_msg(ttpb, crlf_msg, polled);
		    break;
	    }
	}
    }
    return;
}
#endif

/*----------*/
void printf_flush (struct PF_CONTEXT *ptx)
{
    int lf_seen = 0;

    if (ptx->ttbuf_index) {

	if (ptx->ttbuf [ptx->ttbuf_index-1] == LF)
	    lf_seen = 1;

	fwrite (ptx->ttbuf, 1, ptx->ttbuf_index, ptx->fp);
	ptx->ttbuf_index = 0;
    }
#if MODULAR
    if (lf_seen)
	printf_more(ptx->fp, 0);
#endif
}

/*---------*/
/* do not perform any va to pa mapping */
UINT no_mapping (UINT va)
 {
	return va;
 }

/*+
 * ============================================================================
 * = sprintf - in memory formatted output                                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Perform printf style functions into a memory resident buffer.
 *  
 * FORM OF CALL:
 *  
 *	sprintf (buffer, format)
 *  
 * RETURNS:
 *
 *	n - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 * 	char *buffer - address of destination buffer
 *	const char *format - address of format string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int sprintf (char *buf, char *format, protoarg int va_alist)
{
	struct PF_CONTEXT	ptx;


	va_start (ptx.argptr);
	ptx.el_write = 0;
	ptx.obuf = buf;
	ptx.format = format;
	ptx.out = (void *) store_buf;
	ptx.addr_trans = (void *) no_mapping;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	common_print (&ptx);

	if (ptx.suppress_null) {
	    ;
	} else {
	    *ptx.obuf = 0;
	}

	va_end (ptx.argptr);
	return ptx.count;
}

/*+
 * ============================================================================
 * = printf - formated output                                                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Formatted output to the standard output channel.
 *  
 * FORM OF CALL:
 *  
 *	printf (format)
 *  
 * RETURNS:
 *
 *	x - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 *	const char *format - address of format string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int printf (char *format, protoarg int va_alist)
{
	struct PF_CONTEXT	ptx;

	va_start (ptx.argptr);
	ptx.el_write = 0;
        ptx.pcb = getpcb();
	ptx.format = format;

    if (print_debug_flag)
	ptx.out = (void *) store_tt_poll;
    else
	ptx.out = (void *) store_file;

	ptx.addr_trans = (void *) no_mapping;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	ptx.fp = ptx.pcb->pcb$a_stdout;

    if (print_debug_flag)
	lock_printf();

	common_print (&ptx);

    if (print_debug_flag) {
	krn$_micro_delay(10000);
	unlock_printf();
    }

    if (!print_debug_flag)
	printf_flush(&ptx);

	va_end (ptx.argptr);

	return ptx.count;
}


/*+
 * ============================================================================
 * = fprintf - formated output to a specified file 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Formatted output to a specified output channel.
 *  
 * FORM OF CALL:
 *  
 *	fprintf (fp, format)
 *  
 * RETURNS:
 *
 *	x - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp    - address of file descriptor
 *
 *	const char *format - address of format string
 *
-*/
int fprintf (struct FILE *fp, char *format, protoarg int va_alist)
{
	struct PF_CONTEXT	ptx;

	va_start (ptx.argptr);
	ptx.el_write = 0;
	ptx.fp = fp;
	ptx.pcb = getpcb();
	ptx.format = format;
	ptx.out = (void *) store_file;
	ptx.addr_trans = (void *) no_mapping;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	common_print (&ptx);
	printf_flush(&ptx);
	va_end (ptx.argptr);
	return ptx.count;
}

/*+
 * ============================================================================
 * = qprintf - print to the event logger                                      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 * Qprintf is a printf variant that prints characters into the event logger
 * (EL driver).  It is unique in that it can be used in any context, including
 * interrupt service routines and early on in the console when the file system
 * and dynamic memory are not present.
 *
 * Qprintf, unlike other printf variants, has a strong line orientation, which
 * is a reflection of the underlying event logger (el driver).  Well behaved
 * callers of qprintf will always terminate the string with a newline, and
 * never exceed a total of MAX_ELBUF (140) characters.
 *
 * Calls that don't have a terminating newline at the end of the format
 * string will have one put there by qprintf.  As a result, callers that use
 * multiple qprintfs for one line of output will get at least as many lines as
 * calls to qprintf.
 *  
 * FORM OF CALL:
 *  
 *	qprintf (format)
 *  
 * RETURNS:
 *
 *	x - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 *	const char *format - address of format string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int qprintf( char *format, protoarg int va_alist ) {
	struct PF_CONTEXT ptx;
	int id;

	va_start (ptx.argptr);
	ptx.el_write = 1;
	ptx.format = format;

	id = whoami();

	/*
	 * Tee the output of qprintf to the screen if we're not in screen
	 * mode and we're on the powerup path.
	 */
#if !TURBO
	if (!screen_mode && (cpip & (1 << id)))
	    ptx.out = store_tt_poll;
	else
	    ptx.out = (void *) null_procedure;
#else
	if (!screen_mode && (pipp & (1 << id)) && (print_cpu_flag & (1 << id)))
	    ptx.out = store_tt_poll;
	else
	    ptx.out = (void *) null_procedure;
#endif
	
	ptx.addr_trans = (void *) no_mapping;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	lock_printf();
	common_print (&ptx);
	unlock_printf();

	va_end (ptx.argptr);

	return ptx.count;
}

/*+
 * ============================================================================
 * = pprintf - synchronous formatted output to the tty device                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is identical to printf in function except that the output
 *	is directed to the tty.  TTY i/o is handled in a strictly synchronous
 *	polled mode so that printf functions are available in restricted
 *	execution environments.
 *  
 * FORM OF CALL:
 *  
 *	pprintf (format)
 *  
 * RETURNS:
 *
 *	x - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 *	const char *format - address of format string
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int pprintf (char *format, protoarg int va_alist)
{
	struct PF_CONTEXT	ptx;

	va_start (ptx.argptr);
	ptx.el_write = 0;
	ptx.format = format;
	ptx.out = (void *) store_tt_poll;
	ptx.addr_trans = (void *) no_mapping;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	lock_printf();
	common_print (&ptx);
	krn$_micro_delay(10000);
	unlock_printf();
	va_end (ptx.argptr);
	return ptx.count;
}

/*+
 * ============================================================================
 * = err_printf - formated output to stderr or to terminal		      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Formatted output to the stderr output channel if it exists.  Otherwise
 *	format the output to the terminal device in polled mode.
 *  
 * FORM OF CALL:
 *  
 *	err_printf ( format)
 *  
 * RETURNS:
 *
 *	x - number of characters written, not inlcuding terminating null
 *       
 * ARGUMENTS:
 *
 *	const char *format - address of format string
 *
-*/
int err_printf (char *format, protoarg int va_alist) {
	struct PF_CONTEXT	ptx;
	struct PCB      *pcb;


	pcb = getpcb();

	/* Exit if D_REPORT environ var. has been set to "off" */
	if (diag$r_evs & m_rpt_off)
	    return 0;

	va_start (ptx.argptr);
	ptx.el_write = 1;
	ptx.format = format;
	ptx.count = 0;
	ptx.ttbuf_index = 0;
	ptx.pcb = getpcb();
	ptx.addr_trans = (void *) no_mapping;

	/*
	 * Tee the output to the screen if we're not in screen
	 * mode and we're on the powerup path.
	 */
#if !TURBO
	if (   (cpip & (1 << whoami ())) ||
	       (ptx.pcb->pcb$a_stderr == NUL) ) {
#else
	if (   (pipp & (1 << whoami ())) ||
	       (ptx.pcb->pcb$a_stderr == NUL) ) {
#endif
	    ptx.out = store_tt_poll;
	    if (print_debug_flag)
		lock_printf();

	    common_print (&ptx);

	    if (print_debug_flag) {
		krn$_micro_delay(10000);
		unlock_printf();
	    }
	} else {
	    ptx.fp = ptx.pcb->pcb$a_stderr;

	    if (print_debug_flag)
		ptx.out = (void *) store_tt_poll;
	    else
		ptx.out = (void *) store_file;

	    common_print (&ptx);

	    if (!print_debug_flag)
		printf_flush(&ptx);
	}


	va_end (ptx.argptr);
	return ptx.count;
}


/*+
 * ============================================================================
 * = next_token - fetch and decode next printf format token                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine parses printf format strings into tokens, filling in a
 *	common data structure in the process.
 *  
 * FORM OF CALL:
 *  
 *	next_token (ptx)
 *  
 * RETURNS:
 *
 *	x - token value
 *       
 * ARGUMENTS:
 *
 * 	struct PF_CONTEXT *ptx - pointer to printf context
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int next_token (struct PF_CONTEXT *ptx)
{
	int	specifier;
	unsigned char	c;

	ptx->leading_zero = 0;
	ptx->left_justify = 0;
	ptx->upper_case = 0;
	ptx->def_width = 0;
	ptx->def_precision = 0;

	/*
	 * Return if just a vanilla character
	 */
	c = *ptx->format++;
	if (c != '%') {
	    return c;
	}

	/*
	 * See if field needs to be left justified
	 */
	c = *ptx->format++;
	if (c == '-') {
	    ptx->left_justify = 1;
	    c = *ptx->format++;
	}

	/*
	 * field width is specified in argument list
	 */
	if (c == '*') {
	    ptx->width = nextarg (ptx);
	    ptx->def_width = 1;
	    if (ptx->width < 0) {
		ptx->width *= -1;
		ptx->left_justify = 1;
	    }
	    c = *ptx->format++;

	/*
	 * Field width is in the format string
	 */
	} else {
	    ptx->width = 0;
	    ptx->leading_zero = c == '0';
	    while (isdigit (c)) {
		ptx->width *= 10;
		ptx->width += c - '0';
		c = *ptx->format++;
		ptx->def_width = 1;
	    }
	}


	/*
	 * Skip optional decimal point
	 */
	if (c == '.') {
	    c = *ptx->format++;
	}

	/*
	 * Precision is passed in the argument list
	 */
	if (c == '*') {
	    if ((ptx->precision = nextarg (ptx)) < 0) {
		ptx->precision = -ptx->precision;
	    }
	    c = *ptx->format++;
	    ptx->def_precision = 1;

	/*
	 * Precision is in the format string
	 */
	} else {
	    ptx->precision = 0;
	    while (isdigit (c)) {
		ptx->precision *= 10;
		ptx->precision += c - '0';
		c = *ptx->format++;
		ptx->def_precision = 1;
	    }
	}

	if (tolower (c) == 'l') {
	    c = *ptx->format++;
	}

	/*
	 * Test for upper case conversion, and return token.
	 */
	ptx->upper_case = isupper (c);
	specifier = tolower (c);
	if (strchr (specifierlist, specifier)) {
	    return specifier + SPECIFIER_BIAS;
	}
	return c; /* unrecognized */
}

/*+
 * ============================================================================
 * = common_print - routine common to all printf variants                     =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is common to all printf variants.  Each variant sets up
 *	a data structure, PF_CONTEXT, and then calls this common routine.  At
 *	this level, all i/o redirection is hidden.  Since printf is reentrant,
 *	and the context lives on the caller's stack, the address of the context
 *	must be passed down to all subroutines that this routine calls.
 *  
 * FORM OF CALL:
 *  
 *	common_print (ptx)
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 * 	struct PF_CONTEXT *ptx - pointer to context for this call
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void common_print (struct PF_CONTEXT *ptx) {
	unsigned int ch;
	int	done;
/*
 * Translate the string if translatable.  This translation special
 * cases msg_success and msg_failure.
 */
	ptx->format = translate (ptx->format);
	ptx->suppress_null = 0;	
	done = 0;
	do {
	    switch (ch = next_token (ptx)) {
	    case 0:
		done = 1;
		break;

	    case 'c' + SPECIFIER_BIAS:
		    ch = nextarg (ptx);
		    printf_putchar (ptx, ch);
		    break;

	    case 'b' + SPECIFIER_BIAS:
		    convert_number (ptx, 2, 0);
		    break;
	    case 'x' + SPECIFIER_BIAS:
		    convert_number (ptx, 16, 0);
		    break;
	    case 'o' + SPECIFIER_BIAS:
		    convert_number (ptx, 8, 0);
		    break;

	    case 'u' + SPECIFIER_BIAS:
		    convert_number (ptx, 10, 0);
		    break;

	    case 'd' + SPECIFIER_BIAS:
		    convert_number (ptx, 10, 1);
		    break;

	    case 's' + SPECIFIER_BIAS:
		    convert_string (ptx, 0);
		    break;

  	    case 'm' + SPECIFIER_BIAS:
		    convert_string (ptx, 1);
		    break;

	    case 'z' + SPECIFIER_BIAS:
		    ptx->suppress_null = 1;
		    break;

	    default:		/* regular character	*/
		printf_putchar (ptx, ch);
		break;
	    }
	} while (!done);
}


#define min3(a,b,c) (min (min(a,b), min(b,c)))

/*+
 * ============================================================================
 * = convert_string - string conversion for printf                            =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Perform a string conversion for printf.
 *
 * FORM OF CALL:
 *  
 *	convert_string (ptx)
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 * 	struct PF_CONTEXT *ptx - pointer to context for this call
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void convert_string (struct PF_CONTEXT *ptx, int flag) {
	char	*s, *pa_s;
	int	action;
	int	lb, rb;		/* left and right blanks */
	int	w, p, l, c;

	/* do any required va to pa mapping */
	s = (void *) nextarg (ptx);
	pa_s = (*ptx->addr_trans)((UINT)s);  

	if (flag) pa_s = translate (pa_s);   /* pa is stored in pa_s */
	l = strlen (pa_s);
        w = ptx->width;
	p = ptx->precision;

	action = (ptx->left_justify  << 3) |
		 (ptx->leading_zero  << 2) |
		 (ptx->def_width     << 1) |
		 (ptx->def_precision << 0)
	;

	if (ptx->def_precision) {
	    c = min (l, p);
	} else {
	    c = l;
	}

	lb = rb = 0;
	switch (action) {

	case 0:						/* %S		*/
	case 4:						/* %0S		*/

	case 1:						/* %.pS		*/
	case 5:					break;	/* %0.pS        */

	case 2:						/* %wS		*/
	case 6:		lb = max(0,w-l);	break;	/* %0wS         */

	case 3:						/* %w.pS	*/
	case 7:		lb = max(0,w-c);	break;	/* %0w.pS       */

	case 8:						/* %-S		*/
	case 12:					/* %-0S         */

	case 9:						/* %-.pS        */
	case 13:					/* %-0.pS       */

	case 10:					/* %-wS		*/
	case 14:					/* %-0wS        */

	case 11:					/* %-w.pS	*/
	case 15:	rb = max(0,w-c);	break;	/* %-0w.pS      */
	}

	store_bytes (ptx, ptx->leading_zero ? '0' : ' ', lb);
	while (l--  &&  c--) {
	    printf_putchar (ptx, *pa_s);
	    pa_s++;
	}
	store_bytes (ptx, ' ', rb);
}

/*+
 * ============================================================================
 * = print_progress - Print a progress message.                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This routine is to be used as a simple common progress message
 *	to be used by console routines.
 *
 * FORM OF CALL:
 *  
 *	print_progress ()
 *  
 * RETURNS:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	None
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int p_prg_cnt=0;				/*Global counter*/

void print_progress () 
{
/* Use err_printf so the message doesn't end up on std out */
#if 0
    char s[16] = "|/-\\|/-\\";
    err_printf("%c\b",s[(p_prg_cnt++)%4]);
#else
    err_printf(".", 0);
    p_prg_cnt++;
    if ((p_prg_cnt & 63) == 0) {
	p_prg_cnt = 0;
	err_printf("\n", 0);
    }
#endif
}

int clear_progress()
{
    int cnt;
    cnt = p_prg_cnt;
    p_prg_cnt=0;
    return cnt;
}

void remove_progress()
{
    while (p_prg_cnt--)
	err_printf("\b", 0);
}

/*+
 * ============================================================================
 * = convert_number - unsigned numeric conversion for printf                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Perform a unsigned numeric conversion for printf.  Take into account
 *	leading zeros, left justification and explicit field and precision 
 *	specifiers.
 *
 * FORM OF CALL:
 *  
 *	convert_number (ptx, radix, signed)
 *  
 * RETURNS:
 *
 *	msg_success - Normal completion.
 *       
 * ARGUMENTS:
 *
 * 	struct PF_CONTEXT *ptx - per call printf context
 *	unsigned int radix - conversion radix
 *	int signed - do a signed conversion
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
void convert_number (struct PF_CONTEXT *ptx, int radix, int sconvert) {
	char		digitbuf [sizeof (INT) * 8 + 1];
	UINT		digit;
	int		slen;
	UINT		number;
	int		action;
	int		lb;	/* left blanks		*/
	int		rb;	/* right blanks		*/
	int		lz;	/* leading zeros	*/
	int		negative;
	int		*lp, *hp;

	/*
	 *  ajbfix : HACK HACK HACK
	 *
	 * The following works around an 014 bug whereby a LDL is generated
	 * and is not followed by a zapnot when loading unsigned 64 bit ints.
	 * We explicitly zap the high order 32 bits iff the field width is 8
	 * or less, AND the upper 32 are a proper sign extention of the 
	 * lower 32.
	 *
	 * But, don't do the above for signed decimal conversions!
	 */
	number = nextarg (ptx);
	if (((radix != 10) || !sconvert) &&
	    ((ptx->def_precision && (ptx->precision <= 8))	||
	     (ptx->def_width && (ptx->width <= 8))		||
	    (!ptx->def_precision && !ptx->def_width))) {
	    lp = & number;
	    hp = lp + 1;
	    if ((*hp == -1) && (*lp < 0)) {
		*hp = 0;
	    }
	}

	negative = 0;
	if (sconvert && ((INT) number < 0)) {
	    negative = 1;
	    number = - (INT) number;
	}

	/*
	 * convert to ascii.  The following loop has the desired side
	 * effect of suppressing leading zeros, except if the number
	 * is 0, in which case, a single '0' appears in the output.
	 */
	slen = 0;
	do {
	    digit = number % radix;
	    /* ajbfix: hack to work around ots$div_ul bug */
	    switch (radix) {
	    case 2: number = number >> 1L; break;
	    case 8: number = number >> 3L; break;
	    case 16: number = number >> 4L; break;
	    default: number /= radix; break;
	    }
	    digitbuf [slen] = (digit > 9) ? (digit + '0' + 7) : (digit + '0');
	    if (!ptx->upper_case) {
		digitbuf [slen] = tolower (digitbuf [slen]);
	    }
	    slen++;
	} while (number  && (slen < sizeof digitbuf));

	if (negative) {
	    digitbuf [slen++] = '-';
	    ptx->leading_zero = 0;
	}
	digitbuf [slen] = 0;

	action = (ptx->left_justify  << 3) |
		 (ptx->leading_zero  << 2) |
		 (ptx->def_width     << 1) |
		 (ptx->def_precision << 0)
	;

	/*
	 *		Left	Leading	Explict	Explict
	 *	Case	Justify	Zeroes	Width	Precisn	Example
	 *	----    -------	-------	-------	-------	-------
	 *	0	0	0	0	0	%X		
	 *	1	0	0	0	1	%.pX
	 *	2	0	0	1	0	%wX
	 *	3	0	0	1	1	%w.pX
	 *	4	0	1	0	0	%0X
	 *	5	0	1	0	1	%0.pX
	 *	6	0	1	1	0	%0wX
	 *	7	0	1	1	1	%0w.pX
	 *	8	1	0	0	0	%-X
	 *	9	1	0	0	1	%-.pX
	 *	10	1	0	1	0	%-wX
	 *	11	1	0	1	1	%-w.pX
	 *	12	1	1	0	0	%-0X
	 *	13	1	1	0	1	%-0.pX
	 *	14	1	1	1	0	%-0wX
	 *	15	1	1	1	1	%-0w.pX
	 */

	rb = lb = lz = 0;
	switch (action) {
	case 0: break;
	case 1: lb = ptx->width - slen; break;
	case 2: lb = ptx->width - slen; break;
	case 3: lb = ptx->width - slen; break;
	case 4: break;
	case 5: lz = ptx->width - slen; break;
	case 6: lz = ptx->width - slen; break;
	case 7: lz = ptx->width - slen; break;
	case 8: break;
	case 9: rb = ptx->width - slen; break;
	case 10: rb = ptx->width - slen; break;
	case 11: rb = ptx->width - slen; break;
	case 12: break;
	case 13: lz = ptx->width - slen; break;
	case 14: lz = ptx->width - slen; break;
	case 15: lz = ptx->precision - slen; rb = ptx->width - ptx->precision; break;
	}

	/*
	 * stuff out the leading blanks and zeros
	 */
	store_bytes (ptx, ' ', lb);
	store_bytes (ptx, '0', lz);

	/*
	 * Send out the converted number in the reverse order it was built
	 * in.
	 */
	while (slen--) printf_putchar (ptx, digitbuf [slen]);

	/*
	 * Do any right padding required
	 */
	store_bytes (ptx, ' ', rb);
}

/*----------*/
void store_bytes (struct PF_CONTEXT *ptx, int c, int count) {
	while (count-- > 0) {
	    printf_putchar (ptx, c);
	}
}

#if LOCAL_DEBUG

char	*string [] = {
	"this is a test string",
	"small str",
	""
};

char	*string_formats [] = {
	"|%s|\n",
	"|%.10s|\n",
	"|%20s|\n",
	"|%20.10s|\n",
	"|%0s|\n",
	"|%0.10s|\n",
	"|%020s|\n",
	"|%020.10s|\n",
	"|%-s|\n",
	"|%-.10s|\n",
	"|%-20s|\n",
	"|%-20.10s|\n",
	"|%-0s|\n",
	"|%-0.10s|\n",
	"|%-020s|\n",
	"|%-020.10s|\n",
	""
};

char	*numeric_formats [] = {
	"%x\n",
	"%.4x\n",
	"%8x\n",
	"%8.4x\n",
	"%0x\n",
	"%0.4x\n",
	"%08x\n",
	"%08.4x\n",
	"%-x\n",
	"%-.4x\n",
	"%-8x\n",
	"%-8.4x\n",
	"%-0x\n",
	"%-0.4x\n",
	"%-08x\n",
	"%-08.4x\n",
	"",
};

int	numbers [] = {
	0xfeedface,
	0x1234,
	0x12345,
	0xffffffff
};

/*
 * The primary testing methodology is to compare this version of printf with
 * the C RTL version.
 */
void main (int argc, char *argv []) {
	int	i, j;
	char	buf [128];

	myprintf ("character test %c\n", 'A');

	myprintf ("%s%08x adj: %08x_%08x   free: %08x_%08x %6d  \n",
		"ADJ: ",
		1,
		2, 3,
		4, 5,
		10
	);
	myprintf ("%s%08X_%08x\n", "test", 0xfeedface, 0xfeedface);

	/*
	 * Numeric conversions
	 */
	for (i=0; *numeric_formats [i]; i++) {
	    printf ("%d: %s", i, numeric_formats [i]);
	    for (j=0; j<sizeof (numbers) / sizeof (int); j++) {
		myprintf (numeric_formats [i], numbers [j]);
		  printf (numeric_formats [i], numbers [j]);
	    }
	    myprintf ("\n");
	}

	/*
	 * String conversions. (Also check out sprintf here).
	 */
	for (i=0; *string_formats [i]; i++) {
	    printf ("%d: %s", i, string_formats [i]);
	    for (j=0; *string [j]; j++) {
		myprintf (string_formats [i], string [j]);
		mysprintf (buf, string_formats [i], string [j]);
		myprintf ("%s", buf);
		printf (string_formats [i], string [j]);
		printf ("\n");
	    }
	}

}
#endif

/*
 * HACK HACK HACK HACK HACK HACK HACK  ajbfix
 */
int translate (char *s) {

	if (s == msg_success) {
	    return "";
	} else if (s == msg_failure) {
	    return "";
	} else {
	    return s;
	}
}

