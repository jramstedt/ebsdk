#ifndef __EV6_H_LOADED
#define __EV6_H_LOADED
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

/*
 *  $Id: ev6.h,v 1.1.1.1 1998/12/29 21:36:22 paradis Exp $;
 *  Author: Franklin Hooker, Digital Equipment Corporation, July 1997
 */

/*
 * $Log: ev6.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:22  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1997/12/22  17:17:01  gries
 * Quinn's changes from pass 2-3
 *
 * Revision 1.1  1997/11/12  18:51:39  pbell
 * Initial revision
 *
 * Revision 1.3  1997/07/09  05:44:46  fdh
 * Added Win32 definitions.
 *
 * Revision 1.2  1997/07/07  14:56:17  fdh
 * Protytype for process_ev6_srom.
 *
 * Revision 1.1  1997/07/07  00:50:05  fdh
 * Initial revision
 *
 */

#ifndef DEFAULT_EV6_PASSNUM
#define DEFAULT_EV6_PASSNUM 3
#endif


#define MAX_CHAIN 400
#define DATUMSIZE	(sizeof(datum_t)*8) /* Block size in bits */

typedef unsigned int	bit_t;
typedef unsigned long	csr_datum_t;
typedef unsigned long	datum_t;

typedef struct reg_description {
  char *name;
  csr_datum_t value;
  char *alias;
  int  initialized;
} csr_t;

/* ev6.c */
extern void format_ev6_icache(void);
extern void convert_ev6_chain_to_cbox(void);

/* ev6_cbox.c */
extern csr_t * lookup_csr(char *name);

/* ev6_chain.c */
extern int setup_chain_data(bit_t *write_once_a, int reverse, int leader);
extern void dump_cbox(FILE *file);

/* ev6_srom.c */
extern int process_ev6_srom(datum_t* write_once_p);

#endif /* __EV6_H_LOADED */
