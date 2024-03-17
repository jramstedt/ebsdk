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
static char *rcsid = "$Id: bptable.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * $Log: bptable.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.7  1995/12/05  18:33:51  cruz
 * Added routine to allow modification of saved instruction.
 *
 * Revision 1.6  1995/10/23  20:47:11  cruz
 * Performed some cleanup:  1) variables not used outside this
 * module were made static.  2) Comments that once were in the
 * header file were added to the code.
 *
 * Revision 1.5  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/04/07  15:02:31  fdh
 * Added RCS Identifiers.
 *
 * Revision 1.3  1994/03/09 12:48:33 berent
 * Modified to match version in OSF/1 remote debug server. Also
 * made NT compilable
 *
 * Revision 1.2  1993/10/03 00:07:24 berent
 * Merge in development by Anthony Berent
 *
 * Revision 1.1  1993/06/08 19:56:33 fdh
 * Initial revision
 */

#include "ladebug.h"
#include "bptable.h"

static int   bptinuse[BPTMAX];     /* TRUE if this entry is in use FALSE otherwise */
static address_value  bptaddress[BPTMAX];   /* address at which breakpoint is placed */
static instruction_value bptsavedinst[BPTMAX]; /* instruction replaced by the BREAK instruction */
static int bptcount;             /* number of breakpoints set */

/* bptinitialize - Initialize the breakpoint table.
 */
void bptinitialize(void)
{
  int i;

  for (i = 0; i < BPTMAX; i++)
    bptinuse[i] = FALSE;

  bptcount = 0;
}

/* bptmodify - Changes the saved instruction at breakpoint address
 *
 * Arguments:
 *    addr      - address of breakpoint
 *    savedinst - the instruction at that address
 * 
 * Return value:
 *    SUCCESS if successful, otherwise a negative error code.
 */
int bptmodify(address_value addr, instruction_value savedinst)
{
  int i;

  for (i = 0; i < BPTMAX; i++) {
    if (bptinuse[i] && (addr == bptaddress[i])) {
        bptsavedinst[i] = savedinst;
        return SUCCESS;
    }
  }
  return BPTNOBRK;
}

/* bptinsert - add a breakpoint to the breakpoint table
 *
 * Arguments:
 *    addr      - address of breakpoint
 *    savedinst - the instruction at that address
 * 
 * Return value:
 *    SUCCESS if successful, otherwise a negative error code.
 */
int bptinsert(address_value addr, instruction_value savedinst)
{
  int i;

  /* make sure we don't already have a breakpoint at this address */
  for (i = 0; i < BPTMAX; i++) {
    if (bptinuse[i]) {
      if (addr == bptaddress[i])
        return BPTDUP;
    }
  }

  /* look for an unused slot */
  for (i = 0; i < BPTMAX; i++) {
    if (!bptinuse[i]) {
        ++bptcount;
        bptinuse[i] = TRUE;
        bptaddress[i] = addr;
        bptsavedinst[i] = savedinst;
        return SUCCESS;
    }
  }

  return BPTFULL;
}

/* bptremote - remove a breakpoint from the breakpoint table
 *
 * Arguments:
 *     addr      - address of breakpoint
 *     savedinst - (returned) saved instruction
 * Return value:
 *    SUCCESS if successful, otherwise a negative error code.
 */
int bptremove (address_value addr, instruction_value *savedinst)
{
  int i;
  int found = FALSE;

  *savedinst = 0;

  for (i = 0; i < BPTMAX; i++) {
    if (bptinuse[i])
      if (addr == bptaddress[i]) {
        found = TRUE;
        bptinuse[i] = FALSE;
        *savedinst = bptsavedinst[i];
        --bptcount;
      }
  }
  if (found)
    return SUCCESS;
  else
    return BPTNOBRK;
}

/* bptgetn - get a breakpoint by number
 *
 * Arguments:
 *     n         - breakpoint number
 *     addr      - (returned) address of breakpoint
 *     savedinst - (returned) saved instruction
 * Return value:
 *    SUCCESS if successful, otherwise a negative error code.
 */
int bptgetn(int n, address_value *addr, instruction_value *savedinst)
{
  if ((n < 0) || (n > BPTMAX))
    return BPTILL;
  else {
    if (!bptinuse[n])
      return BPTINV;
    else {
      *addr = bptaddress[n];
      *savedinst = bptsavedinst[n];
      return SUCCESS;
    }
  }
}

/* bptgeta - get a breakpoint by address
 *
 * Arguments:
 *     addr      - address of breakpoint
 *     savedinst - (returned) saved instruction
 * Return value:
 *    SUCCESS if successful, otherwise a negative error code.
 */ 
int bptgeta(address_value addr, instruction_value *savedinst)
{
  int i;

  for (i = 0; i < BPTMAX; i++) {
    if (bptinuse[i])
      if (addr == bptaddress[i]) {
        *savedinst = bptsavedinst[i];
        return TRUE;
      }
  }
  return FALSE;
}

/* bptisbreakat - checks whether there is a breakpoint at the given address
 *
 * Argument:
 *     addr      - address to be checked
 * Return value:
 *     TRUE if there is a breakpoint at that address; FALSE if not.
 */ 
int bptisbreakat(address_value addr)
{
  int i;

  for (i = 0; i < BPTMAX; i++) {
    if (bptinuse[i]) {
      if (addr == bptaddress[i])
        return TRUE;
    }
  }
  return FALSE;
}

/* bptfull - checks whether the breakpoint table is full
 *
 * Return value:
 *      TRUE if full; FALSE if not.
 */ 
int bptfull(void)
{
  return (bptcount == BPTMAX);
}

