
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
 *  $Id: cmd.h,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $;
 */

/*
 * $Log: cmd.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.13  1997/12/15  20:52:48  pbell
 * Updated for dp264.
 *
 * Revision 1.12  1996/06/13  03:35:45  fdh
 * Updated static prototypes.
 *
 * Revision 1.11  1996/05/25  20:34:32  fdh
 * Added the DISABLE_BOOTOPTION conditional.
 *
 * Revision 1.10  1996/05/22  22:13:25  fdh
 * Updated a few prototypes.
 *
 * Revision 1.9  1995/12/07  18:39:09  cruz
 * Renamed check cmd to sum.
 *
 * Revision 1.8  1995/10/31  22:44:34  cruz
 * Added conditionals around prototypes that were dependent
 * on NEEDPCI and NEEDFLASHMEMORY.
 *
 * Revision 1.7  1995/10/26  21:53:16  cruz
 * Put #ifdef switches around the function prototypes for
 * functions that are compiled only when those switches are
 * defined.
 *
 * Revision 1.6  1995/10/10  21:34:53  fdh
 * Modified the prototype for Help().
 *
 * Revision 1.5  1995/10/04  23:33:59  fdh
 * Updated local function prototypes.
 *
 * Revision 1.4  1995/09/05  18:33:02  fdh
 * Removed get_cmdline() and parse_arg_strings()
 *
 * Revision 1.3  1995/08/31  21:51:33  fdh
 * Updated local prototypes.
 *
 * Revision 1.2  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.1  1994/06/17  19:40:17  fdh
 * Initial revision
 *
 */

static void Help(int apropos );
static void CommandHelp(char *arg, int apropos);

#ifdef NEEDDEBUGGER
static void cmd_ladebug(void );
static void cmd_bpstat(void );
static void cmd_continue(void );
static void cmd_step(void );
static void cmd_setbpt(void );
static void cmd_clearbpt(void );
#endif /* NEEDDEBUGGER */

static void cmd_beep(void );
static void cmd_memtest(void );
static void cmd_flload(void );
static void cmd_flboot(void );
static void cmd_flraw(void );
static void cmd_fldir(void );
static void cmd_version(void );
static void cmd_ident(void );
static void cmd_go(void );
static void cmd_jtopal(void );
static void cmd_load(void );
static void cmd_boot(void );
static void cmd_deposit(void );
static void cmd_examine(void );
static void cmd_print(void );
static void cmd_sum(void );
static void cmd_change(void );
static void cmd_fill(void );
static void cmd_search(void );
static void cmd_copy(void );
static void cmd_iack(void );
static void cmd_help(void );
static void cmd_tip(void );
static void cmd_date(void );
static void cmd_netdevice(void );
static void cmd_netload(void );
static void cmd_netboot(void );
static void cmd_romload(void );
static void cmd_romboot(void );
static void cmd_arpshow(void );
static void cmd_ethershow(void );
static void cmd_etherinit(void );
#ifdef NEED_ETHERNET_ADDRESS
static void cmd_eaddr(void );
#endif
static void cmd_etherpreg(void );
static void cmd_etherbuff(void );
static void cmd_etherdump(void );
static void cmd_etherprom(void );
static void cmd_preg(void );
static void cmd_creg(void );
static void cmd_etherstat(void );
static void cmd_dis(void );
static void cmd_ctty(void );
static void cmd_setbaud(void );
static void cmd_bootadr(void );
static void cmd_ripr(void );
#if defined(DC21064) || defined(DC21066)
static void cmd_wipr(void );
#endif
static void cmd_read_io(void );
static void cmd_write_io(void );
static void cmd_memory_read_io(void );
static void cmd_memory_write_io(void );

#ifdef NEEDPCI
static void cmd_pcishow(void );
static void cmd_cfgr(void );
static void cmd_cfgw(void );
#endif

static void cmd_rmode(void );
static void cmd_comp(void );
#ifndef EB164  /* Temporarily disabled these since they don't work. */
static void cmd_bc(void );
#endif
static void cmd_romlist(void );

#ifndef DISABLE_BOOTOPTION
static void cmd_bootopt(void );
#endif /* DISABLE_BOOTOPTION */

static void cmd_init(void);
static void cmd_flcopy(void);
static void cmd_flcd(void);

#ifdef NEEDFLASHMEMORY
static void cmd_flash(void);
static void cmd_fwupdate(void);
#endif
