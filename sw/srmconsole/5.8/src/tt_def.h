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
 * Abstract:	TTY Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *	adm	28-March-1995	add "lineset" to TTPB structure
 *
 *	pel	20-Aug-1992	remove allow_login flag from port block
 *
 *	pel	18-Aug-1992	add allow_login flag to port block
 *
 *	pel	30-Jun-1992	reduce xmit/recv buffers from 128 to 16 chars
 *
 *	ajb	15-Nov-1990	Remove obsolete definitions
 *
 *	jad	10-Sep-1990	Initial entry.
 */
#ifndef TT_DEF
#define	TT_DEF	1

#define VU	volatile unsigned

volatile struct TTYQ {
	VU int			qmask;	/* pointer mask		*/
	VU int			in;	/* input index		*/
	VU int			out;	/* output index		*/
	VU char			*buf;	/* pointer to queue	*/
	struct SEMAPHORE	ready;	/* work to be done */
};


/*
 * The buffers for the tranmitter and receiver are kept small so that
 * flow control information is not delayed.  Most of the character buffering
 * occurs in the class driver.
 */
#define	RXQ_SIZE	128 	/* must be a power of 2 	*/
#define	TXQ_SIZE	16 	/* must be a power of 2 	*/
#define	TX_HEADROOM	8	/* leave this amount of room for*/
				/* breakthrough messages such as*/
				/* bell and flow control	*/
 	
/*
 * Port block for the a serial line.  This structure ties all the other
 * data structures together.
 */
volatile struct TTPB {
	struct QUEUE		ttpb;
	int			(*rxoff)();
	int			(*rxon)();
	int			(*rxready)();
	int			(*rxread)();
	int			(*txoff)();
	int			(*txon)();
	int			(*txready)();
	int			(*txsend)();
	int			(*lineset)();
	int			port;
	int			translate_nl;
	int			perm_mode;
	int			perm_poll;
	int			mode;
	int			poll;
	int			type;
	int			page;
	int			spage;
	int			linecnt;
	char			**rcb;
	struct INODE		*ip;
	struct FILE		*fp;
	struct QUEUE		ctrlch;		/* ^c notification queue*/
	volatile int		stop;		/* ^O state		*/
	volatile int		flow;	        /* flow control state,	*/
						/* 0, XON, XOFF		*/
	volatile int		login_ena;	/* login enabled	*/
	volatile int		ctrl_o_ena;	/* control o enabled	*/
	volatile int		ctrl_c_ena;	/* control c enabled	*/
	volatile int		ctrl_x_ena;	/* control x enabled	*/
	volatile int		ctrl_t_ena;	/* control t enabled	*/
	volatile int		ctrl_d_ena;	/* control d enabled	*/
	volatile int		ctrl_o_rec;	/* control o received	*/
	volatile int		ctrl_c_rec;	/* control c received	*/
	volatile int		ctrl_x_rec;	/* control x received	*/
	volatile int		ctrl_t_rec;	/* control t received	*/
	volatile int		ctrl_d_rec;	/* control d received	*/
	volatile struct POLLQ	pqp;
	volatile struct TTYQ	rxq;
	volatile struct TTYQ	txq;
};    

#undef VU
#endif
