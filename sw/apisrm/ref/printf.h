/*
 * Copyright (C) 1990 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Context for printf
 *
 * Author:	AJ Beaverson
 *
 * Modifications:
 *
 *	ajb	12-Dec-1991	Remove message context additions
 *
 *	ajb	17-May-1991	Add %z
 *
 *	sfs	04-Apr-1991	Pad out structure fields.
 *
 *	mrc	05-Feb-1991	Increase MAX_PRINTF_ARGS to 24.
 *
 *	ajb	21-jan-1991	Buffer output on regular printfs for
 *				performance reasons.
 *
 *	pel	20-Nov-1990	Add file descr & pcb pointers to PF_CONTEXT.
 *
 *	ajb	03-Oct-1990	add message structure to print context so that
 *				message numbers can also serve as format
 *				strings.
 *
 *	ajb	23-Aug-1990	Change from argbase to argix.
 *
 *	sfs	01-Aug-1990	Initial entry.
 */

struct PF_CONTEXT {
	unsigned def_precision	: 1;	/* has precision been explicitly defined*/
	unsigned def_width	: 1;	/* has field width been explicity set   */
	unsigned leading_zero	: 1;	/* show leading zeros			*/
	unsigned left_justify	: 1;	/* left justify within field		*/
	unsigned upper_case	: 1;	/* print in upper case only		*/
	unsigned suppress_null	: 1;	/* suppress terminating null		*/
	unsigned fill1		: 26;	/* pad to long				*/
	int	el_write;		/* el write flag			*/
	int 	width;			/* the width of the field		*/
	int	precision;		/* number of digits that converted thing*/
					/* will occupy				*/
	int	*argptr;		/* argument list pointer		*/
	char	*format;		/* address into format string		*/
	char	*obuf;			/* address of output buffer (sprintf)	*/
	int	(*out) ();		/* address of output routine		*/
	int (*addr_trans) ();		/* addr of virt to phys translate rtn   */
	int	count;			/* number of characters written         */
	struct  FILE *fp;		/* addr of file descriptor for fprintf  */
	struct  PCB *pcb;		/* addr of PCB				*/	
	int	ttbuf_index;
	char	ttbuf [256];		/* multiple of 4, enough for a typical	*/
					/* line					*/
};
