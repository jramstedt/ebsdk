#ifndef __LADBX_H_LOADED
#define __LADBX_H_LOADED
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
 *  $Id: ladebug.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: ladebug.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.21  1997/06/10  18:17:12  fdh
 * Removed obsoleted declarations.
 *
 * Revision 1.20  1997/02/26  21:49:57  fdh
 * Modified argument type to ktrap().
 *
 * Revision 1.19  1997/02/25  21:22:17  fdh
 * Added kinitdebugger.
 *
 * Revision 1.18  1997/02/21  04:18:34  fdh
 * Modified ktrap parameters.
 *
 * Revision 1.17  1996/05/22  21:30:07  fdh
 * Removed obsoleted prototypes.
 *
 * Revision 1.16  1996/01/26  00:04:14  cruz
 * Added new routine for supporting the "next" command.
 *
 * Revision 1.15  1995/12/05  18:40:19  cruz
 * Added prototype for bptmodify.
 *
 * Revision 1.14  1995/10/31  18:50:21  cruz
 * Commented out some unused routines.
 *
 * Revision 1.13  1995/10/30  16:57:51  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.12  1995/10/24  18:33:15  fdh
 * Removed a misplaced semicolon.
 *
 * Revision 1.11  1995/10/24  13:57:08  fdh
 * Added a global declaration and removed references
 * to the obsoleted interrupt save register array.
 *
 * Revision 1.10  1995/10/23  22:07:45  fdh
 * Added some general typedefs.
 *
 * Revision 1.9  1995/10/23  22:04:39  fdh
 * Removed obsolete header file.
 *
 * Revision 1.8  1995/10/23  20:59:12  cruz
 * Merged other header files into this one.
 *
 * Revision 1.7  1995/09/05  18:40:21  fdh
 * Added protocol.h
 *
 * Revision 1.6  1995/08/30  23:01:09  fdh
 * Added kremoveallbreak(void); and made restore_breakpoint_instrs(void);
 * and external routine.
 *
 * Revision 1.5  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/06/30  09:39:18  berent
 * Added   kwrmces to write the machine check error summary register
 *
 * Revision 1.3  1994/06/28  20:08:21  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.2  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.1  1994/06/20  12:54:55  fdh
 * Initial revision
 *
 */

#include "lib.h"
#include "protocol.h"

typedef ul address_value;
typedef ul register_value;
typedef ui instruction_value;

/*
 * Global variables.
 */
extern address_value saved_user_pc;
extern int child_state;          /* State of child process */
extern volatile int stopped;
extern volatile int dbug_remotely;
extern int noDebugger;

/*::::::::::::::
kutil.s
::::::::::::::*/
extern void kutilinitbreaks(void);

/*::::::::::::::
bptable.c
::::::::::::::*/
extern void bptinitialize(void);
extern int bptmodify(address_value addr, instruction_value savedinst);
extern int bptinsert(address_value addr, instruction_value savedinst);
extern int bptremove(address_value addr, instruction_value *savedinst);
extern int bptgetn(int n , address_value * addr , instruction_value * savedinst);
extern int bptgeta(address_value addr , instruction_value * savedinst);
extern int bptisbreakat(address_value addr);
extern int bptfull(void );


/*::::::::::::::
kernel.c
::::::::::::::*/
extern void kinitdebugger(void);
extern int kload_implemented(void );
extern int kload(char * name , char * argv[] , char * standardIn , char * standardOut , 
                 char * standardError , address_value loadAddress , address_value startAddress);
extern int kconnect_implemented(void );
extern int kconnect(ui pid);
extern int kkill_possible(void );
extern void kkill(void );
extern int kdetach_possible(void );
extern void kdetach(void );
extern ui kpid(void );
extern void kgo(void );
extern void kstop(void );
extern int kaddressok(address_value address);
extern ul kcexamine(address_value address);
extern int kcdeposit(address_value address , ul value);
extern void kstep(void );
extern void knext(void );
extern address_value kpc(void );
extern void ksetpc(address_value newpc);
extern register_value kregister(int reg);
extern void ksetreg(int reg , register_value value);
extern short kbreak(address_value address);
extern short kremovebreak(address_value address);
extern int kpoll(void );
extern void kinitpalentry(void );
extern void kstart(void );
extern void kinstall_breakpoints(void );
extern void kprint_breakpoints(void );
extern void kenableserver(void );
extern void restore_breakpoint_instrs(void );
extern void kremoveallbreak(void );
extern void ktrap(unsigned int type , ul arg0 , ul arg1 , ul arg2, register_value *arg3);
extern void kreenter(void );

#ifdef NOT_IN_USE
extern void knullipl(void );
#endif

/*::::::::::::::
pkthandl.c
::::::::::::::*/
extern void ProcessPacket(DebugPacket * packet_structP);

/*::::::::::::::
pktutil.c
::::::::::::::*/
extern uw ReadNetShort(ub * dataP);
extern ui ReadNetInt(ub * dataP);
extern void WriteNetShort(ub * dataP , uw s);
extern void WriteNetInt(ub * dataP , ui s);
extern ui ReadLocalInt(ub * dataP);
extern ul ReadLocalLong(ub * dataP);
extern void WriteLocalInt(ub * dataP , ui s);
extern void WriteLocalLong(ub * dataP , ul s);

#ifdef NOT_IN_USE
extern uw ReadLocalShort(ub * dataP);
extern ul ReadNetLong(ub * dataP);
extern void WriteNetLong(ub * dataP , ul s);
extern void WriteLocalShort(ub * dataP , uw s);
#endif


/*::::::::::::::
readloop.c
::::::::::::::*/
extern void enable_ladbx_msg(void );
extern void data_received(unsigned int arg0 , ui arg1 , ui * arg2);
extern void read_packets(void );
extern int this_client_is_connected(void );
extern void app_poll(address_value stop_pc);
extern int a_client_is_connected(void );
extern void set_connected(void );
extern void disconnect_client(void );
extern void ladbx_server_init_module(void );

#endif /* __LADBX_H_LOADED */
