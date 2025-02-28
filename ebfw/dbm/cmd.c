/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: cmd.c,v 1.3 1999/04/15 21:30:39 paradis Exp $";
#endif

/*
 * $Log: cmd.c,v $
 * Revision 1.3  1999/04/15 21:30:39  paradis
 * Scrubbed out RX164 references
 *
 * Revision 1.2  1999/01/19 19:42:06  bissen
 * Added commands unique to the Rx164
 *
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.182  1998/08/26  18:39:44  thulin
 * Add support for CLUBS
 * co -l dbmentry.c
 *
 * Revision 1.181  1998/08/04  15:44:18  thulin
 * Added DISABLE_BOOTOPTION around call to set_romboot
 *
 * Revision 1.180  1998/08/03  17:31:15  gries
 * added bootoption and debug stuff from pc264
 *
 * Revision 1.179  1998/07/30  21:19:53  thulin
 * Conditionalize call to set_romboot with DISABLE_BOOTOPTION
 *
 * Revision 1.178  1998/06/26  18:19:38  gries
 * added cominit vinit to init video and comport commands
 *
 * Revision 1.177  1998/04/06  18:14:45  gries
 * changes for large memory systems
 *
 * Revision 1.176  1997/12/15  20:52:47  pbell
 * Updated for dp264
 *
 * Revision 1.175  1997/08/14  20:50:09  pbell
 * Added SROM port reset capacity to the setbaud command
 *
 * Revision 1.174  1997/08/01  19:21:29  pbell
 * Fixed copy and memtest for NT builds so that they operate on addresses above
 * 4gb.
 *
 * Revision 1.173  1997/07/31  19:47:21  fdh
 * Added DP264 conditionals.
 *
 * Revision 1.172  1997/07/10  00:43:49  fdh
 * Modified the setty command to return the current
 * tty name if no argument is provided.
 *
 * Revision 1.171  1997/06/30  14:10:59  pbell
 * Moved the location of objects in the toy nvram.
 *
 * Revision 1.170  1997/06/18  13:22:34  pbell
 * Fixed a parameter bug in flread and flwrite.  Both were using parameter 4
 * in place of 5 for the drive number.
 *
 * Revision 1.169  1997/06/16  21:04:49  pbell
 * Added Error to mces command help text.
 *
 * Revision 1.168  1997/06/16  19:43:40  pbell
 * Updated command help text inconsistancies.
 *
 * Revision 1.167  1997/06/11  18:46:31  fdh
 * Modified the memtest command to have the memtest function
 * handle the default values for the memory range and increment.
 *
 * Revision 1.166  1997/06/09  19:20:57  fdh
 * Corrected load and boot command help information.
 *
 * Revision 1.165  1997/06/09  18:54:54  fdh
 * Modified flwrite and flsave help descriptions.
 *
 * Revision 1.164  1997/06/09  15:41:09  fdh
 * Modified arguments for the flsave command.
 *
 * Revision 1.163  1997/06/02  17:41:42  fdh
 * Added a couple of include files for a couple of missing declarations.
 *
 * Revision 1.162  1997/05/31  04:14:02  fdh
 * Enabled the read and write iccsr commands for the DC21164PC.
 *
 * Revision 1.161  1997/05/23  13:17:56  fdh
 * Removed an obsoleted definition.
 *
 * Revision 1.160  1997/05/19  18:40:07  fdh
 * Modified the fwupdate command to simply call the
 * Fail-Safe Booter.
 *
 * Revision 1.159  1997/05/16  02:10:10  fdh
 * Added the function argument to the pci config space commands.
 *
 * Revision 1.158  1997/05/06  14:50:40  pbell
 * Added include files fat.h and nttypes.h for prototypes.
 *
 * Revision 1.157  1997/05/02  18:25:19  fdh
 * Changed the parameters used for the flread and flwrite commands.
 *
 * Revision 1.156  1997/04/29  21:46:03  pbell
 * Enhanced load and boot commands to use the XMODEM protocal and
 * added an optional destination address parameter
 *
 * Revision 1.155  1997/04/23  17:48:29  pbell
 * Updated the help text of the setty command.
 *
 * Revision 1.154  1997/03/27  19:41:30  fdh
 * Start the memtest at the first 1MB boundary beyond __start
 * since the _end label is not always defined.
 *
 * Revision 1.153  1997/02/26  21:48:26  fdh
 * Pass NULL as the printReg command as the default pointer to
 * signal that it should use its internal default pointer.
 *
 * Revision 1.152  1997/02/25  16:28:31  fdh
 * Pass ieee emulation signature in second argument to wrfen
 * for cmd_wrfen command.
 *
 * Revision 1.151  1997/02/25  02:22:39  fdh
 * Removed the conditional around the wrfen command.
 *
 * Revision 1.150  1997/02/21  01:01:29  fdh
 * Added contition for enabling a wrfen, "Write Floating Point Enable"
 * command.
 *
 * Revision 1.149  1996/08/20  17:50:10  fdh
 * Added a missing cast.
 *
 * Revision 1.148  1996/08/07  18:31:46  fdh
 * Modified pcishow to dump all of config space for a
 * specified device.
 *
 * Revision 1.147  1996/06/17  21:53:24  fdh
 * Corrected the printf modifier used in the cmd_examine function.
 * Corrected the argument type used for the mcheck arg in the mcheck
 * command.
 *
 * Revision 1.146  1996/06/13  03:35:21  fdh
 * Merged functions to save space where possible.
 * Corrected arg type passed to the tip command.
 *
 * Revision 1.145  1996/06/06  20:01:54  fdh
 * Abort on illegal registers specified with the cxreg command.
 *
 * Revision 1.144  1996/05/22  22:05:26  fdh
 * Modified memtest arguments.  Added mcheck command.
 * Print the value read for prx commands even when a PCI
 * device is not present.  Added swpipl command.
 * Added mces command.  Added mcheck and stop_drivers args
 * to the memtest command.  Print brief help when syntax
 * errors are encountered. Tabulate help output.
 *
 * Revision 1.143  1996/02/14  18:57:55  cruz
 * Changed cmd_sum to print the return checksum from ChecksumMem().
 *
 * Revision 1.142  1996/02/14  17:31:10  cruz
 * Added new command romverify.
 *
 * Revision 1.141  1996/02/06  14:06:23  cruz
 * Added command "flasherase".
 *
 * Revision 1.140  1996/01/26  00:06:07  cruz
 * Added the "next" command, which is similar to "step" but
 * allows you to skip over instructions so you don't have
 * to follow subroutine calls.
 * Enabled riccsr and wiccsr for DC21164.
 *
 * Revision 1.139  1995/12/15  22:35:44  cruz
 * Disabled cpreg and ppreg.
 *
 * Revision 1.138  1995/12/12  20:40:37  cruz
 * Modified help string for cmd setty to include port names.
 *
 * Revision 1.137  1995/12/08  19:40:50  cruz
 * Don't print data read from pci bus if it's detected that
 * no pci is present in the slot requested.
 *
 * Revision 1.136  1995/12/07  19:26:04  cruz
 * Added code for commands that use a range to check that
 * the ending address is greater than the starting address.
 *
 * Revision 1.135  1995/12/07  18:54:09  cruz
 * Fixed copy command so it includes the byte at end_address in
 * the copy operation.  Also put in a check to make sure that
 * the end address is greater than the start address, otherwise,
 * it does not perform the copy operation.
 *
 * Revision 1.134  1995/12/07  18:38:31  cruz
 * Renamed check cmd to sum.
 *
 * Revision 1.133  1995/11/30  22:50:00  cruz
 * Changed invocation of changeReg command to pass in PalImpureBase
 * instead of the save_register_array.  This was required since
 * changeReg uses offsets from PalImpureBase and not saved_register_array.
 *
 * Revision 1.132  1995/11/29  20:09:36  cruz
 * Changed description of eml command.
 *
 * Revision 1.131  1995/11/28  20:58:54  cruz
 * Added command eml and emq.  Removed em command.
 * Changed save command to flsave.
 *
 * Revision 1.130  1995/11/21  23:17:51  cruz
 * Updated description of first argument of flash command so it
 * more accurately describes its current function.
 *
 * Revision 1.129  1995/11/16  23:57:32  cruz
 * Updated flash command arguments to be ImageAddress, DestinationOffset, UserSpecifiedSize.
 *
 * Revision 1.128  1995/11/15  21:02:51  fdh
 * Changed some command names.
 *
 * Revision 1.127  1995/11/13  14:46:08  cruz
 * Fixed mispelling.
 *
 * Revision 1.126  1995/11/09  21:56:28  cruz
 * Added casting operators.
 *
 * Revision 1.125  1995/11/06  23:11:06  fdh
 * Use printf modifiers for printing unsigned long values.
 *
 * Revision 1.124  1995/11/02  23:09:53  cruz
 * Fixed the preg command which was broken for EV5 when
 * saved_register_array was changed to point to the base
 * of the GPR data area.
 *
 * Revision 1.123  1995/11/01  16:48:20  cruz
 * Disabled command fwupdate if NEEDFLOPPY is not defined.
 *
 * Revision 1.122  1995/10/30  16:50:05  cruz
 * Moved the declaration of scsi_initialized.
 *
 * Revision 1.121  1995/10/27  15:13:08  cruz
 * Moved declaration of etherprom and etherdump to netman.c
 *
 * Revision 1.120  1995/10/26  22:16:06  cruz
 * Removed declaration of CPU_cycles_per_usecs and regmode since
 * they are now included in lib.h
 *
 * Revision 1.119  1995/10/24  18:31:18  fdh
 * Added void to function declarations which pass no arguments.
 *
 * Revision 1.118  1995/10/23  19:56:27  cruz
 * Moved declaration of stopped and child_state to kernel.c
 *
 * Revision 1.117  1995/10/20  18:54:46  cruz
 * Performed some clean up.  Updated copyright headers.
 *
 * Revision 1.116  1995/10/20  13:58:51  fdh
 * Added sysshow command to dump out system (SROM) parameters.
 *
 * Revision 1.115  1995/10/18  18:03:30  fdh
 * Added estop command.
 *
 * Revision 1.114  1995/10/18  14:21:25  cruz
 * Moved the setvar routines to prtrace.c
 *
 * Revision 1.113  1995/10/13  20:18:34  fdh
 * Renamed copy command to memcopy.  Copy command is
 * now a floppy command.
 *
 * Revision 1.112  1995/10/13  20:07:27  cruz
 * Added commands for processing the new debugging command
 * "set"
 *
 * Revision 1.111  1995/10/11  21:44:43  fdh
 * Modified datatypes passed to memtest().
 *
 * Revision 1.110  1995/10/10  21:34:25  fdh
 * Added the apropos command and modified Help() to
 * support it.
 *
 * Revision 1.109  1995/10/10  14:22:41  fdh
 * Changed the ladbx command to ladebug.
 * Renamed ladbx.h to ladebug.h.
 *
 * Revision 1.108  1995/10/06  20:08:19  cruz
 * Rearranged the order of the arguments to flread and flwrite.
 *
 * Revision 1.107  1995/10/03  03:13:34  fdh
 * Include console.h.  Removed some obsolete functions.
 *
 * Revision 1.106  1995/09/28  20:26:15  cruz
 * Added new version of flread and flwrite commands.
 *
 * Revision 1.105  1995/09/27  17:41:12  cruz
 * Removed aliases of floppy commands and renamed a couple.
 *
 * Revision 1.104  1995/09/22  13:10:59  fdh
 * cmd_copy calls memcpy() instead of memmove().
 *
 * Revision 1.103  1995/09/21  20:52:53  cruz
 * Added the flcd command, as well as aliases for fldir and flcd.
 *
 * Revision 1.102  1995/09/20  15:08:22  fdh
 * Added fwupdate command that is used to run the "Standalone
 * Firmware Update Utility" from a floppy.
 *
 * Revision 1.101  1995/09/20  14:50:46  cruz
 * Added the ability to specify a path for the fldir command.
 *
 * Revision 1.100  1995/09/15  19:30:12  cruz
 * Updated floppy commands.
 *
 * Revision 1.99  1995/09/05  18:32:12  fdh
 * Removed Debug Monitor startup and command line processing.
 * This module now only contains the command array and associated
 * data.
 *
 * Revision 1.98  1995/09/02  03:36:33  fdh
 * Implemented command line history.
 * Modified startup messages.
 * Added init command which determines the current
 * PALbase and jtopal to it.
 *
 * Revision 1.97  1995/08/31  21:58:06  fdh
 * Implemented capability to pass command line arguments to user
 * programs with the go command.
 * Implemented get_cmdline() and parse_arg_strings() to clean up
 * the command parsing code a little bit.
 * Fixed bug in algorithm for handling "quoted" command line arguments.
 *
 * Revision 1.96  1995/08/30  23:14:53  fdh
 * Replaced call to bptremoveall() with call to kremoveallbreak()
 * to remove all breakpoints with the delete command.
 *
 * Revision 1.95  1995/08/29  12:58:18  fdh
 * Initialize calls to malloc.
 *
 * Revision 1.94  1995/08/25  20:13:23  fdh
 * Modified delete command to accept a * to mean delete
 * all existing breakpoints.
 * Tip command accepts arguments like com1, COM2 as well as 1 or 2.
 * Setty command accepts arguments like graphics, COM1, com2
 * as well as 0, 1, or 2.
 * Added function argument to references to PCI Config space
 * access routines.
 *
 * Revision 1.93  1995/06/30  15:53:08  cruz
 * Fixed bug which caused R25 to get overwritten when using
 * LadeBug with EB164.
 *
 * Revision 1.92  1995/05/19  20:52:31  fdh
 * Changed the startup messages a little bit.
 *
 * Revision 1.91  1995/05/19  20:35:27  fdh
 * Print PALcode Halt Code iduring Debug Monitor Startup.
 *
 * Revision 1.90  1995/04/21  19:47:55  cruz
 * Temporarily disable bcon and bcoff for EB164.
 *
 * Revision 1.89  1995/04/20  16:49:00  cruz
 * Include 21164 commands.
 *
 * Revision 1.88  1995/03/08  21:47:31  fdh
 * RE-Fixed bug in pwb command that was
 * re-broken in version 1.85.
 *
 * Revision 1.87  1995/03/03  17:43:41  fdh
 * Added a kludge to work around the linker defined labels
 * that don't get defined in VC++.
 *
 * Revision 1.86  1995/02/28  03:00:27  fdh
 * Casts to proper data types.
 *
 * Revision 1.85  1995/02/23  21:45:33  fdh
 * Include romhead.h.
 * Add some casts for warning free builds.
 *
 * Revision 1.84  1995/02/21  15:34:03  cruz
 * Fixed bug in cmd_em which manifested itself when the silent
 * switch was used.
 *
 * Revision 1.83  1995/02/21  14:40:37  cruz
 * Fixed bug in pwb command.
 *
 * Revision 1.82  1995/02/20  03:54:06  fdh
 * Fixup to romboot.
 *
 * Revision 1.81  1995/02/19  17:50:30  fdh
 * Modified the romload and romboot commands to use
 * an ostype argument as well as the image position
 * number.
 *
 * Revision 1.80  1995/02/13  21:41:42  fdh
 * Added bus to systax string for the prw command.
 *
 * Revision 1.79  1995/02/10  02:53:17  fdh
 * Added provisions for disabling bootoption command.
 *
 * Revision 1.78  1995/02/10  02:25:58  fdh
 * Added optional bus argument to PCI config space access commands.
 *
 * Revision 1.77  1995/01/19  19:46:11  cruz
 * Eliminated address masks from rb, rw, mrbb and mrw commands.
 *
 * Revision 1.76  1994/11/28  19:40:11  fdh
 * Modified to use registered LED codes.
 *
 * Revision 1.75  1994/11/23  20:14:25  cruz
 * Modified arguments for memtest.
 * Modified to support eb164.
 *
 * Revision 1.74  1994/11/19  03:26:51  fdh
 * Added romlist command.
 *
 * Revision 1.73  1994/11/18  19:47:56  fdh
 * Added Segment Count Override argument to
 * flash command.
 *
 * Revision 1.72  1994/11/18  19:06:09  fdh
 * Moved the calls to disable interrupts closer
 * to the place where they are needed.
 * Define a different prompt for the debug monitor
 * when being debugged by the debugger.
 *
 * Revision 1.71  1994/11/16  22:03:23  fdh
 * Disable interrupts while detecting memory.
 *
 * Revision 1.70  1994/11/15  20:06:47  fdh
 * Enable the RTC interrupts so that
 * messages can reach the debug server
 * when debugging the debug monitor.
 *
 * Revision 1.69  1994/11/10  19:37:59  fdh
 * Always disable interrupts before calling
 * bus init routines.
 *
 * Revision 1.68  1994/11/09  19:12:38  fdh
 * Added compare command.
 *
 * Revision 1.67  1994/11/08  21:20:54  fdh
 * Added flash command.
 *
 * Revision 1.66  1994/11/01  11:32:53  rusling
 * Changed following PCI-PCI bridge support.
 *
 * Revision 1.65  1994/10/17  18:20:34  berent
 * Disable interrupts earlier during startup.
 * This avoids taking an interrupt when the
 * PCI bus is probed to detect any existing devices.
 *
 * Revision 1.64  1994/08/09  08:02:28  fdh
 * Properly cast data type in cmd_outsysctl().
 *
 * Revision 1.63  1994/08/08  00:35:57  fdh
 * Added prtrace debug monitor command when TRACE_ENABLE is defined.
 *
 * Revision 1.62  1994/08/05  20:18:01  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.61  1994/08/03  19:43:36  fdh
 * Fixups around the linker defined symbols _edata and _end.
 *
 * Revision 1.60  1994/07/21  17:55:42  fdh
 * Added bcon and bcoff commands that are used to enable and
 * disable the Backup Cache.
 * Also print out memory size as passed from the SROM instead
 * of the value calculated by the Debug Monitor.
 *
 * Revision 1.59  1994/07/13  14:28:30  fdh
 * Modified to use sysdata structure.
 * Modified Beep command.
 *
 * Revision 1.58  1994/07/11  15:26:25  rusling
 * Fixed infinite loops when testing scratch.
 *
 * Revision 1.57  1994/07/11  14:23:26  fdh
 * Display CPU Speed at Startup.
 *
 * Revision 1.56  1994/06/23  13:43:51  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.55  1994/06/20  18:01:45  fdh
 * Modified to use new ANSI compliant PALcode definitions.
 *
 * Revision 1.54  1994/06/19  15:42:30  fdh
 * Disable include file inside of include file so that makedepend
 * can properly evaluate the includes.
 *
 * Revision 1.53  1994/06/17  19:36:54  fdh
 * Clean-up...
 *
 * Revision 1.52  1994/06/03  20:26:41  fdh
 * Cleanup
 *
 * Revision 1.51  1994/04/08  04:47:26  fdh
 * Fixup some help strings.
 *
 * Revision 1.50  1994/04/07  20:55:45  fdh
 * Small adjustment to a help string
 *
 * Revision 1.49  1994/04/06  18:59:30  fdh
 * Added syntax line to help description.
 * Cleaned up help descriptions.
 *
 * Revision 1.48  1994/04/06  04:21:30  fdh
 * Modified for updated cserve.h file.
 *
 * Revision 1.47  1994/04/04  21:33:41  fdh
 * Output some data to the LED port as soon as
 * the I/O bus is initialized.
 *
 * Revision 1.46  1994/04/03  00:25:13  fdh
 * Changed repeat argument to number of iterations for
 * memory and I/O read and write commands.
 *
 * Revision 1.45  1994/04/02  01:26:10  fdh
 * Report Debug Monitor entry point at startup.
 *
 * Revision 1.44  1994/04/01  22:48:42  fdh
 * Modified data type for DisStartAddress.
 * Removed obsoleted global variable.
 * Update bootadr and DisStartAddress properly
 * for all load and boot commands.
 * Added destination address argument to floppy commands.
 *
 * Revision 1.43  1994/03/25  05:01:55  fdh
 * Dress up help command.
 *
 * Revision 1.42  1994/03/24  21:36:05  fdh
 * Removed NEEDISA conditionals from "basic" I/O commands.
 * Also removed redundant mpil and ppil commands which
 * resemble the wl and rl commands.
 *
 * Revision 1.41  1994/03/16  11:25:04  fdh
 * Declare all command array functions to return void.
 *
 * Revision 1.40  1994/03/16  00:02:52  fdh
 * Recovered lost edits from revisions 1.37 to 1.39.
 * Added eaddr command and reorganised BBRAM access code. - rusling
 * Simplified cmd_memtest()... Moved most of it to memtest.c.
 * Simplified cmd_eaddr()... Moved most of it to eaddr.c.
 * Moved BBRAM defines to bbram.h.
 *
 * Revision 1.39  1994/03/14  20:49:10  fdh
 * Modify saved_register_array pointer initialization.
 *
 * Revision 1.38  1994/03/13  16:08:23  fdh
 * Make memtest skip over Stack.
 * Make memtest arguments optional and use resonable defaults.
 * Print approx (not the top or the bottom) location of stack
 * at startup.
 *
 * Revision 1.37  1994/03/11  22:10:58  fdh
 * Remove redundant argument in call
 * to printReg().
 *
 * Revision 1.36  1994/03/11  01:41:58  fdh
 * Modified to use PALcode Impure pointer as received from
 * PALcode cserve function.
 * Added bootadr argument to netboot and netload commands.
 * This behaves as though the bootadr command was used before
 * using these two commands.
 * Added pointer argument to the pxreg commands to allow
 * users to view another save_state area. (Useful for debugging
 * another copy of PALcode)
 *
 * Revision 1.35  1994/01/20  15:44:08  rusling
 * Added new command, arpshow to show the current
 * Arp  tables.
 *
 * Revision 1.34  1994/01/19  10:50:12  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.33  1993/11/29  11:49:03  rusling
 * Added mpil and ppil commands to modify and examine
 * PCI I/O addresses.
 *
 * Revision 1.32  1993/11/24  15:05:48  rusling
 * Check for PCI NODEV error when reading/writing PCI
 * address spaces.
 *
 * Revision 1.31  1993/11/22  16:01:50  rusling
 * Moved the IO Bus init call out of uart_init() and
 * explicitly into main().
 *
 * Revision 1.30  1993/11/22  15:47:57  rusling
 * Now call PCIInit() from main() if we NEEDPCI.
 *
 * Revision 1.29  1993/11/22  15:32:12  rusling
 * Fixed bug where eshow and einit were always working with
 * device 0.
 *
 * Revision 1.28  1993/11/22  13:16:41  rusling
 * Merged with my PCI/21040 changes.
 * Includes new commands eshow and pcishow.
 *
 * Revision 1.27  1993/11/19  16:55:46  fdh
 * Include system header file...
 * Fix cmd_em() so that compiler does not disable reads through optimizations.
 *
 * Revision 1.26  1993/11/02  21:08:08  fdh
 * Add back in romload and romboot commands that were
 * mysteriously dropped during the version 1.23 merge.
 *
 * Revision 1.25  1993/10/25  15:50:26  fdh
 * Added repeat and silent arguments to the mrx commands.
 *
 * Revision 1.24  1993/10/15  23:07:29  fdh
 * Allow noDebugger symbol to disable DECladebug server at
 * run time when debugging this program with DECladebug.
 * This was inadvertenly disabled by revision 1.23
 *
 * Revision 1.23  1993/10/03  01:05:18  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.18  1993/10/01  11:02:23  berent
 *>> Changing the ethernet buffer address reinitialises networking and
 *>> interrupts now disabled during startup
 *>>
 *>> Revision 1.17  1993/08/10  10:46:42  berent
 *>> Move network initialisation out of this module; add
 *>> netdevice command.
 *>>
 *>> Revision 1.16  1993/08/10  09:11:58  berent
 *>> Added some tracing, plus modified to use prtrace header file.
 *>>
 *>> Revision 1.15  1993/08/09  11:55:46  berent
 *>> Merge of 1.14 with changes for the new networking code (1.6.1.1)
 *
 * Revision 1.22  1993/09/27  18:54:49  fdh
 * Added memory test
 *
 * Revision 1.21  1993/08/26  06:22:05  fdh
 * Cleaned up command recall.  No longer need carriage return
 * with command recall. Backspace over command recall character.
 *
 * Revision 1.20  1993/08/25  23:14:33  fdh
 * Disable debugger commands when noDebugger is TRUE.
 * Modified to use prtrace header file.
 *
 * Revision 1.19  1993/08/25  21:37:52  fdh
 * Removed the thrid argument on the romload and romboot
 * commands.  This argument is implied when the first argument
 * is zero.
 *
 * Revision 1.18  1993/08/25  04:03:07  fdh
 * Created romload and romboot commands.
 *
 * Created noDebugger symbol to be used to disable the
 * debugger features of the Debug Monitor. This allows
 * one to debug a second copy of the Debug Monitor with
 * the Debug Monitor.  This is possible both with and
 * without DECladebug.  Simply assign a TRUE value to
 * the noDebugger symbol before stepping into main().
 *
 * Revision 1.17  1993/08/21  02:12:38  fdh
 * Corrected io routine masks and eliminated unnecessary ones.
 *
 * Revision 1.16  1993/08/19  06:17:18  fdh
 * Pass in PROMPT and BANNER at compile time.
 *
 * Revision 1.15  1993/08/09  12:59:49  fdh
 * Establish RTC base address in one place.
 *
 * Revision 1.14  1993/07/30  19:54:12  fdh
 * replaced dm command with dml and dmq.
 *
 * Revision 1.13  1993/07/29  12:57:09  fdh
 * Memory barrier after deposit memory (dm) command.
 *
 * Revision 1.12  1993/07/28  16:55:06  fdh
 * Change configuration register command names.
 *
 * Revision 1.11  1993/07/27  22:13:18  fdh
 * Re-enable CheckDate().
 *
 * Revision 1.10  1993/07/27  22:01:01  fdh
 * Correct printf mask for configuration register access functions.
 *
 * Revision 1.9  1993/07/27  21:35:46  fdh
 * Fixes to configuration register access commands.
 *
 * Revision 1.8  1993/07/26  20:11:50  fdh
 * disable CheckDate() upon startup... Remove this
 * version after RTC is know to work.
 *
 * Revision 1.7  1993/07/26  14:59:03  fdh
 * Added PCI commands and removed System Control Register commands.
 *
 * Revision 1.6  1993/06/18  17:32:36  fdh
 * Point disassembler to start of image after loading...
 *
 * Revision 1.5  1993/06/18  16:48:46  fdh
 * Init unitialized data during C runtime startup...
 *
 * Revision 1.4  1993/06/17  19:14:11  fdh
 * Added swppal command to take advantage of the swppal
 * facility in PALcode.
 * Added bpstat command to print current breakpoint status.
 * Fixed command recall bug in which only the first command
 * line argument was recalled after doing two consecutive
 * command recalls.
 *
 * Revision 1.3  1993/06/11  23:54:00  fdh
 * Minor cleanup... Mostly aesthetic...
 *
 * Revision 1.2  1993/06/09  20:21:12  fdh
 * Added ident command to Identify files by RCS ID Keywords
 * used to build images that are downloaded to memory.
 *
 * Revision 1.1  1993/06/08  19:56:39  fdh
 * Initial revision
 *
 * Debug Monitor Command Parser   L.S.   9/13/91
 */
#include "system.h"
#include "lib.h"
#include "mon.h"
#include "ether.h"
#include "ladebug.h"
#include "bbram.h"
#include "cmd.h"
#include "pci.h"
#include "romhead.h"
#include "rom.h"
#include "console.h"
#include "paldata.h"
#include "palcsrv.h"
#include "nttypes.h"
#include "fat.h"
#include "mcheck.h"
#include "fsboot.h"
#include "interlock.h"
#ifdef __osf__
#include <c_asm.h>
#endif

extern ul *saved_register_array;
extern ul default_address;
extern ul bootadr;

extern int cpu_count;
extern void (*secondary_cpu_function)(void);
extern char __start;
extern ul DisStartAddress;
ul scratch;
static  volatile int lock=0;

static ui fixarg;
static char address_error[] ="End address must be greater than start address.\n";

void InitCommPorts();

#ifdef NEEDPCI
char *NoDevString = "No PCI device exists in that slot\n";
#endif

#define Byte 0
#define Word 1
#define Long 2
#define Quad 3

#ifdef NEEDDEBUGGER
char *NoDebuggerString = "Error; Debugger disabled.\n";
char *NoProcessString = "Error; Debug process terminated.\n";
static void cmd_ladebug(void)
{
  if (!noDebugger) {
#if 0
    if (argc > 1) {
      bootadr = ((argc < 3) ? bootadr : hexarg[2]);
      netboot(argc, argv[1]);
      kbreak( (address_value)bootadr);
      printf("Breakpoint entered at 0x%x\n", bootadr);
      ExecuteProgram((Function_t)bootadr, (argc>3 ? argc-3 : 0), &argv[3]);
    }
#endif

    kenableserver();
  }
  else printf(NoDebuggerString);
}
static void cmd_bpstat(void)
{
  if (!noDebugger)
    kprint_breakpoints();
  else printf(NoDebuggerString);
}
static void cmd_continue(void)
{
  if (!noDebugger) {
    if (child_state != PROCESS_STATE_PROCESS_TERMINATED)
      kgo();
    else printf(NoProcessString);
  }
  else printf(NoDebuggerString);
}
static void cmd_step(void)
{
  if (!noDebugger) {
      if (child_state != PROCESS_STATE_PROCESS_TERMINATED) {
        if (fixarg == 0) {
            kstep();
        } else {
            knext();
        }
      }
      else printf(NoProcessString);
  }
  else printf(NoDebuggerString);
}
static void cmd_setbpt(void)
{
  if (!noDebugger)
    kbreak( (address_value)( hexarg[1]));
  else printf(NoDebuggerString);
}
static void cmd_clearbpt(void)
{
  if (!noDebugger) {
    if (*argv[1] == '*') kremoveallbreak();
    else kremovebreak( (address_value)( hexarg[1]));
  }
  else printf(NoDebuggerString);
}
#endif /* NEEDDEBUGGER */


#ifdef TRACE_ENABLE
static void cmd_setvar(void) {setvar(argc, argv);}

static void cmd_prtrace(void)
{
  if (argc == 1)
    printf("Tracing by debug prints is %s.\n",
           prtrace_enable ? "ENABLED" : "DISABLED" );
  else prtrace_enable = (int)hexarg[1];
}
#endif /* TRACE_ENABLE */

static void cmd_init(void)
{
  ul destaddr;
  destaddr = *(ul *)(PalImpureBase + CNS_Q_PAL_BASE);
  jToPal(destaddr);
}
static void cmd_beep(void) {Beep((int) decarg[1], (int) decarg[2]);}

static void cmd_memtest(void)
{
  memtest( (argc < 3) ? 0 : hexarg[2],
           (argc < 4) ? 0 : hexarg[3],
           (argc < 5) ? 0 : (int) hexarg[4],
           ((argc < 2) ? 1 : (int) decarg[1]),
           ((argc < 6) ? (State_t) StateOn : ConfigStateID(argv[5])), /* Machine checks on by default */
           ((argc < 7) ? 0 : (int) hexarg[6]) /* Stop driver by default */
          );
}

static void cmd_mcheck(void) { SetMcheck(ConfigStateID(argv[1]));}

#ifdef NEEDFLOPPY
static void cmd_flcopy(void) {
    FileCopy (argv[1], argv[2]);
}

static void cmd_flload(void)
{
  bootadr = ((argc < 3) ? bootadr : hexarg[2]);
  LoadAFile(argv[1], (char *) bootadr);
  DisStartAddress = bootadr;
}
static void cmd_flsave(void)
{
  FileWriteRange(argv[1],hexarg[2], (size_t)hexarg[3]);
}

static void cmd_flboot(void)
{
  bootadr = ((argc < 3) ? bootadr : hexarg[2]);
  if (LoadAFile(argv[1], (char *) bootadr) > 0){
    jToPal(bootadr);
  }
  DisStartAddress = bootadr;
}
static void cmd_flraw(void)
{
  ui first, bytes, iter, drive;

  first = (argc < 2) ? 0 : (ui)decarg[1]; /* Defaults to sector 0 */
  bytes= (argc < 3) ? 0 : (ui)decarg[2]; /* Do whole disk if size is zero */
  bootadr = ((argc < 4) ? bootadr : hexarg[3]);	/* Default to bootadr */
  iter  = (argc < 5) ? 1 : (ui)decarg[4]; /* Default to 1 iteration */
  drive= (argc < 6) ? 0 : (ui)decarg[5]; /* Default to drive 0 */

  if (ReadWriteSectors (fixarg, first, bytes, (char *)bootadr, iter, drive)) {
      DisStartAddress = bootadr;
  }
}
static void cmd_fldir(void)
{
   FileDirectory(argc<2 ? "" : argv[1]);}

static void cmd_flcd(void)
{
 char *path, *drive;

   path = fcd(argc<2 ? "" : argv[1]);
   drive = fdr("");
   if (path && drive) {
      printf("%s:%s\n\n", drive, path);
   }
   else {
      printf("Invalid path and/or drive spec.\n\n");
  }
}
#endif /* NEEDFLOPPY */

static void cmd_version(void) { PrintVersion(); }
static void cmd_ident(void)
{
  ul first, last;
  first = (argc < 2) ? default_address : hexarg[1];
  last  = (argc < 3) ? first+0x70 : hexarg[2];
  if (last >= first)
    ident(first, last);
  else
    printf(address_error);
}

static void cmd_go(void) {
  Function_t destaddr;

#ifdef NEEDDEBUGGER
  kinstall_breakpoints();
#endif

  destaddr = (Function_t) ((argc < 2) ? bootadr : hexarg[1]);
  ExecuteProgram(destaddr, (argc>2 ? argc-2 : 0), &argv[2]);
}
static void cmd_jtopal(void) {
  ul destaddr;
  destaddr = (argc < 2) ? bootadr : hexarg[1];
  jToPal(destaddr);
}

static void cmd_load(void)
{
    bootadr = ((argc < 2) ? bootadr : hexarg[1]);
    DisStartAddress = bootadr;
    XReceive( bootadr );
}

static void cmd_boot(void)
{
    bootadr = ((argc < 2) ? bootadr : hexarg[1]);
    DisStartAddress = bootadr;
    if( XReceive( bootadr ) )
        jToPal(bootadr);
}

static void cmd_deposit(void) {
  scratch = (ul)decarg[3];
  do {
    switch(fixarg) {
    case Quad:
      WriteQ(hexarg[1],(ul)hexarg[2]);
      break;

    case Long:
      WriteL(hexarg[1],(ui)hexarg[2]);
      break;

    case Word:
      WriteW(hexarg[1],(uw)hexarg[2]);
      break;

    case Byte:
      WriteB(hexarg[1],(ub)hexarg[2]);
      break;

    default:
      break;
    }

    mb(); /* Force it out */
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

#ifdef __GNUC__
static void deposit_quad (unsigned long address, unsigned long value) {
    // CALL_PAL INSQHIL Insert into longword queue at head, interlocked
    register sl __r0 __asm__("$0");
    register ul __r16 __asm__("$16") = address;
    register ul __r17 __asm__("$17") = value;
    __asm__ __volatile__ (
      "call_pal 0x87;"
      : "+r"(__r16), "+r"(__r17), "=r"(__r0)
    );
}
#endif

static void cmd_dirty_deposit(void) {
  scratch = (ul)decarg[3];
  do {
    switch(fixarg) {
    case Quad:
#ifdef _WIN32
      deposit_quad(hexarg[1],(ul)hexarg[2]);
#elif __GNUC__
      deposit_quad(hexarg[1], hexarg[2]);
#else
      asm("call_pal 0x87",hexarg[1],(ul)hexarg[2]);
#endif
      break;

    case Long:
      WriteL(hexarg[1],(ui)hexarg[2]);
      break;

    case Word:
      WriteW(hexarg[1],(uw)hexarg[2]);
      break;

    case Byte:
      WriteB(hexarg[1],(ub)hexarg[2]);
      break;

    default:
      break;
    }

    mb(); /* Force it out */
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

static void cmd_examine(void)
{
  ul keepme = 0ul;

  scratch = (ul)decarg[2];
  do {
    switch(fixarg) {
    case Quad:
      keepme = ReadQ(hexarg[1]);
      break;

    case Long:
      keepme = ReadL(hexarg[1]);
      break;

    case Word:
      keepme = ReadW(hexarg[1]);
      break;

    case Byte:
      keepme = ReadB(hexarg[1]);
      break;

    default:
      break;
    }

    if (!hexarg[3])
      printf( "%lX\n", keepme);
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

static void cmd_print(void)
{
  ul first, last;
  first = (argc < 2) ? default_address : hexarg[1];
  last  = (argc < 3) ? first+0x70 : hexarg[2];
  if (last >= first)
    PrintMem(fixarg, first, last, decarg[3], (int)hexarg[4]);
  else
    printf(address_error);
}
static void cmd_sum(void) 
{ 
    int ck;
    ck = ChecksumMem(hexarg[1],hexarg[2]); 
    printf("Checksum = 0x%04X (%d)\n", ck, ck);
}
static void cmd_change(void)
{
  ul base;
  base = (argc < 2) ? default_address : hexarg[1];
  ChangeMem(fixarg, base);
}
static void cmd_fill(void)
{
  if (hexarg[2] >= hexarg[1])
    FillMem((ul) hexarg[1], (ul) hexarg[2], (argc < 4) ? 0 : (ui)hexarg[3]);
  else
    printf(address_error);
}
static void cmd_search(void)
{
  if (hexarg[2] >= hexarg[1])
    search(hexarg[1], hexarg[2], (int)fixarg, (char *)argv[3], (int)hexarg[4]);
  else
    printf(address_error);
}
static void cmd_copy(void)
{
  if (hexarg[2] >= hexarg[1])
  {
    ul s = hexarg[3];
    ul ct = hexarg[1];
    ul size = hexarg[2]-hexarg[1]+1;
    while (size--) WriteB( s++, ReadB( ct++ ) );
  }
  else  
    printf(address_error);
}

static void cmd_comp(void)
{
  if (hexarg[2] >= hexarg[1])
    CompareMem(hexarg[1], hexarg[2], hexarg[3]);
  else
    printf(address_error);
}

static void cmd_iack(void) { printf("%02x\n", inIack() & 0xff); }
#ifdef NEEDSCSI
extern ui scsi_initialized;
static void cmd_scsiquery(void) { scsiquery(); }
static void cmd_scsiboot(void) { scsiboot(0, hexarg[1], argv[2]); }
static void cmd_scsiread(void) { scsiread(0, hexarg[1], hexarg[2], hexarg[3], hexarg[4]); }
#endif /* NEEDSCSI */
static void cmd_help(void) { Help(fixarg); }
static void cmd_tip(void) { tip(argv[1]); }
static void cmd_date(void)
{
  if (argc == 1) printDate();
  else setDate((ub *)argv[1]);
}

#ifdef NEEDFLASHMEMORY
static void cmd_flash(void) {
  if (fixarg)
        flash_main (~0ul,
                    ((argc < 2) ? 0ul : hexarg[1]),
                    ((argc < 3) ? 0ul : hexarg[2]));
  else  
    flash_main (((argc < 2) ? bootadr : hexarg[1]),
                ((argc < 3) ? ~0ul : hexarg[2]),
                ((argc < 4) ? 0ul : hexarg[3]));
}

#ifdef NEEDFLOPPY
static void cmd_fwupdate(void) {
  if (fsboot(&bootadr)) {
    jToPal(bootadr);
  }
}
#endif
#endif

#if !(defined(EB164) || defined(DP264) || defined(CLUBS))
static void cmd_bc(void)
{
  if (fixarg) EnableBCache();
  else DisableBCache();
}
#endif

static void cmd_netdevice(void)
{
  if (argc == 1)
    printf("Using network device %d\n", monitor_ether_device);
  else {
    if (!netman_set_monitor_device((int)decarg[1]))
      printf("error: invalid netdevice\n");
  }
}
static void cmd_netload(void) {
  bootadr = ((argc < 3) ? bootadr : hexarg[2]);
  netboot(argc, argv[1]);
  DisStartAddress = bootadr;
}
static void cmd_netboot(void)
{
  bootadr = ((argc < 3) ? bootadr : hexarg[2]);
  if (netboot(argc, argv[1]) >= 0){
    DisStartAddress = bootadr;
    jToPal(bootadr);
  }
  DisStartAddress = bootadr;
}

/*
 * Note: DisStartAddress and bootadr are set by read_rom()
 * based on the address specified in the ROM. This value
 * is overridden by hexarg[2].
 */
static void cmd_romload(void)
{
  read_rom(argc,
           (argc<2 ? "" : argv[1]),
           ((argc < 3) ? bootadr : hexarg[2]), fixarg);
}
static void cmd_romboot(void)
{
  int i= 1;

  if (atomic_inc(1,&lock)==0)
  {
    printf("read rom\n");
    if (read_rom(argc,
               (argc<2 ? "" : argv[1]),
               ((argc < 3) ? bootadr : hexarg[2]), fixarg)) {
    printf("reset lock\n");
    lock =0;
    }
  }
  else
  {
    while(lock) i++;
  }
    jToPal(bootadr);
}
static void cmd_romlist(void) {list_rom_headers();}

static void cmd_bootopt(void)
{
#ifndef DISABLE_BOOTOPTION
  if (argc < 2) {
    printf("\nPredefined bootoptions are...\n");
    ostype_dump("  ");
  }
  set_romboot(argc<2 ? "" : argv[1]);
#endif
}

static void cmd_arpshow(void) {arp_show();}

static void cmd_ethershow(void) {ether_device_show();}

static void cmd_videoinit(void) { console_video_init();}


static void cmd_cominit(void) { InitCommPorts();}

static void cmd_etherinit(void) { netman_start_monitor_device();}

static void cmd_etherstop(void) { netman_stop_monitor_device();}
#ifdef NEED_ETHERNET_ADDRESS
static void cmd_eaddr(void) { ethernet_address(argc, (unsigned char *) argv[1]); }
#endif
static void cmd_etherpreg(void) { ether_device_preg(monitor_ether_device); }

static void cmd_etherbuff(void)
{
  if (argc == 1) printf("%x\n", etherbuffers);
  else {
    etherbuffers = (char *) hexarg[1];

    /*
     * When the buffers move all the networking buffers
     * have to be reinitialised since they all hold onto buffers
     */
    netman_setup();
  }
}
static void cmd_etherdump(void)
{
  if (argc == 1)
    printf("packet dumps are %s.\n", etherdump ? "ON" : "OFF" );
  else etherdump = (ui)hexarg[1];
}
static void cmd_etherprom(void)
{
  if (argc == 1)
    printf("Promiscuous Mode is %s.\n", etherprom ? "ENABLED" : "DISABLED" );
  else etherprom = (ui)hexarg[1];
}
static void cmd_preg(void)
{
  printReg(
           ((argc < 2) ? (ul *)NULL : (ul *)(hexarg[1])),
           fixarg);
}
static void cmd_creg(void)
{
  if ((unsigned int)decarg[1] > (unsigned int) 32) {
    printf("Nonexistant Register\n");
    return;
  }
  changeReg((ul *)PalImpureBase, (int)fixarg, (int)decarg[1], hexarg[2]);
}
static void cmd_etherstat(void) {ether_device_print_stats(monitor_ether_device);}
static void cmd_dis(void) { dis( argc, hexarg[1], hexarg[2]); }
static void cmd_ctty(void)
{
  char *PortString;
  if (argc == 1) {
    if ((PortString = PortIDToString(outputport)) != NULL)
      puts(PortString);
  }
  else
    ctty(argv[1]);
}

static void cmd_setbaud(void) { InitTty( argv[1], (int)decarg[2] ); }

static void cmd_bootadr(void)
{
  int x;

  if (argc == 1) printf("%08x\n", bootadr);
  else {
    bootadr = hexarg[1];
    /* Save default boot address in non-volatile ram */
    for (x=0;x<4;x++) {
      BBRAM_WRITE(BBRAM_BOOTADR + x, (ub)(bootadr >> (x * 8)));
    }
    BBRAM_WRITE(BBRAM_BOOTADR + 4, BBRAM_BOOTADR_COOKIE(bootadr))
  }
}

#ifdef NEEDSCTL
static void cmd_insysctl(void) { printf("%08x\n",insctl()&0xFFFFFF00L); }
static void cmd_outsysctl(void) { outsctl((ui)hexarg[1]); }
#endif

static void cmd_ripr(void) { printf("%lx\n", cServe(0, 0, fixarg));}
static void cmd_wipr(void) { cServe(hexarg[1], 0, fixarg);}


#ifdef DC21164
static void cmd_wbcache(void) {wr_bcache(argc, hexarg[1], hexarg[2], fixarg);}
#endif /* DC21164 */

static void cmd_read_io(void)
{
  ui (*function)(ui p) = NULL;
  ul mask = 0ul;

  switch(fixarg) {
  case Long:
    function = inportl;
    mask = 0xffffffffUL;
    break;

  case Word:
    function = inportw;
    mask = 0xffffUL;
    break;

  case Byte:
    function = inportb;
    mask = 0xffUL;
    break;

  default:
    return;
  }

  scratch = (ul)decarg[2];
  do {
    if (!hexarg[3])
      printf("%x\n",(*function)((ui)hexarg[1])&mask);
    else
      (*function)( (ui)hexarg[1]);
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

static void cmd_write_io(void)
{
  void (*function)(ui p , ui d) = NULL;
  ul mask = 0ul;

  switch(fixarg) {
  case Long:
    function = outportl;
    mask = 0xffffffffUL;
    break;

  case Word:
    function = outportw;
    mask = 0xffffUL;
    break;

  case Byte:
    function = outportb;
    mask = 0xffUL;
    break;

  default:
    return;
  }

  scratch = (ul)decarg[3];
  do {
    (*function)( (ui)hexarg[1], (ui)(hexarg[2] & mask));
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

static void cmd_memory_read_io(void)
{
  ui (*function)(ui p) = NULL;
  ul mask = 0ul;

  switch(fixarg) {
  case Long:
    function = inmeml;
    mask = 0xffffffffUL;
    break;

  case Word:
    function = inmemw;
    mask = 0xffffUL;
    break;

  case Byte:
    function = inmemb;
    mask = 0xffUL;
    break;

  default:
    return;
  }

  scratch = (ul)decarg[2];
  do {
    if (!hexarg[3])
      printf("%x\n", (*function)( (ui)(hexarg[1])) & mask);
    else
      (*function)( (ui)hexarg[1]);
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

static void cmd_memory_write_io(void)
{
  void (*function)(ui p , ui d) = NULL;
  ul mask = 0ul;

  switch(fixarg) {
  case Long:
    function = outmeml;
    mask = 0xffffffffUL;
    break;

  case Word:
    function = outmemw;
    mask = 0xffffUL;
    break;

  case Byte:
    function = outmemb;
    mask = 0xffUL;
    break;

  default:
    return;
  }

  scratch = (ul)decarg[3];
  do {
    (*function)( (ui)hexarg[1], (ui)(hexarg[2] & mask));
    if (scratch != 0) scratch = scratch -1;
  } while (scratch > 0);
}

#ifdef NEEDPCI
static void cmd_pcishow(void)
{ 
  if (argc > 1) {
    IOPCIClearNODEV();
    PCIDumpDevice(((argc>2)?(ui)decarg[2]:0), (ui) decarg[1], (argc>3)?(ui)decarg[3]:0);
    if (IOPCIClearNODEV())
      printf(NoDevString);
  }
  else
    PCIShow();
}
static void cmd_cfgr(void)
{
  int value = 0;
  ul mask = 0ul;

  IOPCIClearNODEV();

  switch(fixarg) {
  case Long:
    value = incfgl((ui)((argc>3)?decarg[3]:0), (ui)decarg[2], (ui)((argc>4)?decarg[4]:0), (ui)hexarg[1]);
    mask = 0xffffffffUL;
    break;

  case Word:
    value = incfgw((ui)((argc>3)?decarg[3]:0), (ui)decarg[2], (ui)((argc>4)?decarg[4]:0), (ui)hexarg[1]);
    mask = 0xffffUL;
    break;

  case Byte:
    value = incfgb((ui)((argc>3)?decarg[3]:0), (ui)decarg[2], (ui)((argc>4)?decarg[4]:0), (ui)hexarg[1]);
    mask = 0xffUL;
    break;

  default:
    return;
  }

  if (IOPCIClearNODEV())
    printf(NoDevString);

  printf("%x\n", value & mask);
}

static void cmd_cfgw(void)
{
  IOPCIClearNODEV();

  switch(fixarg) {
  case Long:
    outcfgl( (ui)((argc>4)?decarg[4]:0), (ui)decarg[2], (ui)((argc>5)?decarg[5]:0), (ui)hexarg[1], (ui)hexarg[3]);
    break;

  case Word:
    outcfgw( (ui)((argc>4)?decarg[4]:0), (ui)decarg[2], (ui)((argc>5)?decarg[5]:0), (ui)hexarg[1], (ui)hexarg[3]);
    break;

  case Byte:
    outcfgb( (ui)((argc>4)?decarg[4]:0), (ui)decarg[2], (ui)((argc>5)?decarg[5]:0), (ui)hexarg[1], (ui)hexarg[3]);
    break;

  default:
    return;
  }

  if (IOPCIClearNODEV())
    printf(NoDevString);
}
#endif /* NEEDPCI */

static void cmd_rmode(void)
{
  if (argc == 1) printf("rmode = %d\n", regmode);
  else {
    regmode = (ui)hexarg[1];
    /* Save register mode in non-volatile ram */
    BBRAM_WRITE(BBRAM_RMODE, (ub)regmode);
    BBRAM_WRITE(BBRAM_RMODE + 1, BBRAM_RMODE_COOKIE(regmode));
  }
}

static void cmd_sysshow(void)
{
  DumpSysData();
}

static void cmd_swpipl(void)
{
  int ipl;
  if (argc < 2) {
    ipl = swpipl(7);
    swpipl(ipl);
    printf("\nCurrent Interrupt Priority Level: %d\n", ipl);
    return;
  }

  if ((ul) decarg[1] > (ul) 7) {
    printf("error: Invalid Interrupt Priority Level\n");
    return;
  }

  ipl = (int) decarg[1];
  swpipl(ipl);
}

static void cmd_mces(void)
{
  if (argc > 1) wrmces(hexarg[1]);
  printf("\nMachine Check Error Summary: %02X\n\n", rdmces());
}

static void cmd_wrfen(void)
{
  /*
  ** 0xABC means really wrfen.  This is used to
  ** distinguish emulated wrfen versus real wrfen
  ** when running with floating point emulation
  */
  wrfen((ui)hexarg[1], (ul) 0x0ABC);
}

#ifndef _WIN32
static void cmd_mt(void)
{
  mt(argc, argv);
}
#endif

const struct KEY {
   char *keyw;
    char *syntax;
    void (*func)(void);
    int fixarg;
    int dual_cmd;
    char *helpstr;
    char *syntaxstr;
} keyarray[] = {

/*
 * Download and Execution Commands
 */
  { "load", "x", cmd_load, 0, 0, "XMODEM load a file through a serial port", "address" },
  { "boot", "x", cmd_boot, 0, 0, "XMODEM load a file through a serial port and begins execution", "address" },
  { "netload", "sx", cmd_netload, 0, 0, "Downloads file via the Ethernet port to address. Address defaults to bootadr",
      "file address" },
  { "netboot", "sx", cmd_netboot, 0, 1, "Downloads file through the Ethernet port and begins execution", "file address" },
#ifdef NEEDFLOPPY
  { "flcd", "s", cmd_flcd, 0, 0, "Sets the current directory or default drive.  If no path is specified, displays current directory", "path" },
  { "flcopy", "SS", cmd_flcopy, 0, 0, "Copies source file to destination file", "source destination" },
  { "fldir", "s", cmd_fldir, 0, 0, "Displays floppy directory", "path" },
  { "flboot", "Sx", cmd_flboot, 0, 0, "Downloads the floppy file and begins execution", "file address" },
  { "flload", "Sx", cmd_flload, 0, 0, "Downloads the floppy file at specified address. Address defaults to bootadr", "file address" },
  { "flread", "ddxdd", cmd_flraw, 0, 0, "Reads data from specified floppy sectors", "first_sector bytes dest_address iterations drive" },
  { "flwrite", "ddxdd", cmd_flraw, 1, 0, "Writes data to specified floppy sectors", "first_sector image_size source_address iterations drive" },
  { "flsave", "SXX", cmd_flsave, 0, 0, "Writes a memory range to a file", "file_name start_address file_size" },
#endif /* NEEDFLOPPY */
  { "romboot", "sx", cmd_romboot, 0, 1, "Loads specified image from ROM and begins execution", "type address" },
  { "romlist", "", cmd_romlist, 0, 0, "Lists ROM image headers contained in ROM", "" },
  { "romload", "sx", cmd_romload, 0, 0, "Loads specified image from ROM to specified address", "type address" },
  { "romverify", "sx", cmd_romload, 1, 0, "Compares image from ROM to specified address", "type address" },
  { "bootadr", "x", cmd_bootadr, 0, 0, "Sets the default address to download a file or begin execution", "address" },
  { "bootopt", "s", cmd_bootopt, 0, 0, "Selects firmware for an O/S on the next powerup", "type" },
#ifdef NEEDSCSI
  { "scsiquery", "", cmd_scsiquery, 0, 0, "query scsi bus devices" },
  { "scsiboot", "XS", cmd_scsiboot, 0, 0, "boot from <unit> <file>" },
  { "scsiread", "XXXX", cmd_scsiread, 0, 0, "<unit> <lbn> <bytes> to <adr>" },
#endif /* NEEDSCSI */
  { "go", "x", cmd_go, 0, 0, "Starts execution at the specified address. Defaults to bootadr", "start_address" },
  { "jtopal", "x", cmd_jtopal, 0, 1, "Starts execution at the specified address in PALmode. Defaults to bootadr", "start_address" },
  { "init", "", cmd_init, 0, 1, "Reinitializes Debug Monitor (jtopal PALbase)", ""},

/*
 * Examine and Modify Memory Commands
 */
  { "emb", "Xdx", cmd_examine, Byte, 0, "Displays a byte of data from specified address", "address iterations silent"},
  { "emw", "Xdx", cmd_examine, Word, 0, "Displays a word of data from specified address", "address iterations silent"},
  { "eml", "Xdx", cmd_examine, Long, 0, "Displays a longword of data from specified address", "address iterations silent"},
  { "emq", "Xdx", cmd_examine, Quad, 0, "Displays a quadword of data from specified address", "address iterations silent"},
  { "ddmq", "XXd", cmd_dirty_deposit, Quad, 0, "Deposits quadword of data at specified address", "address data iterations"},
  { "dmb", "XXd", cmd_deposit, Byte, 0, "Deposits byte of data at specified address", "address data iterations"},
  { "dmw", "XXd", cmd_deposit, Word, 0, "Deposits word of data at specified address", "address data iterations"},
  { "dml", "XXd", cmd_deposit, Long, 0, "Deposits longword of data at specified address", "address data iterations"},
  { "dmq", "XXd", cmd_deposit, Quad, 0, "Deposits quadword of data at specified address", "address data iterations"},
  { "pq", "xxdx", cmd_print, Quad, 0, "Prints memory as quadwords (64 bits)", "start_address end_address iterations silent" },
  { "pl",  "xxdx", cmd_print, Long, 0, "Prints memory as longwords (32 bits)", "start_address end_address iterations silent" },
  { "pw", "xxdx", cmd_print, Word, 0, "Prints memory as words (16 bits)", "start_address end_address iterations silent" },
  { "pb", "xxdx", cmd_print, Byte, 0, "Prints memory as bytes (8 bits)", "start_address end_address iterations silent" },
  { "cq", "x", cmd_change, Quad, 0, "Edits memory quadwords (64 bits)", "address" },
  { "cl",  "x", cmd_change, Long, 0, "Edits memory longwords (32 bits)", "address" },
  { "cw", "x", cmd_change, Word, 0, "Edits memory words (16 bits)", "address" },
  { "cb", "x", cmd_change, Byte, 0, "Edits memory bytes (8 bits)", "address" },
  { "fill", "XXx", cmd_fill, 0, 0, "Fills memory range with specified longword (32-bit) pattern",
      "start_address end_address fill_value" },
  { "copy", "XXX", cmd_copy, 0, 0, "Copies a memory range to a specified address", "start_address end_address destination" },
  { "compare", "XXX", cmd_comp, 0, 0, "Compares a memory range to a specified address", "start_address end_address compare_address" },
  { "dis",  "xx", cmd_dis, 0, 0, "Displays memory as CPU instructions", "start_address end_address" },
  { "sum", "XX", cmd_sum, 0, 0, "Computes the checksum of a memory range", "start_address end_address" },
  { "rl", "Xdx", cmd_read_io, Long, 0, "Reads longword from register port in I/O address space", "register iterations silent" },
  { "rw", "Xdx", cmd_read_io, Word, 0, "Reads word from register port in I/O address space", "register iterations silent" },
  { "rb", "Xdx", cmd_read_io, Byte, 0, "Reads byte from register port in I/O address space", "register iterations silent" },
  { "wl", "XXd", cmd_write_io, Long, 0, "Writes a longword to a register port in I/O address space", "register data iterations" },
  { "ww", "XXd", cmd_write_io, Word, 0, "Writes a word to a register port in I/O address space", "register data iterations" },
  { "wb", "XXd", cmd_write_io, Byte, 0, "Writes a byte to a register port at I/O address space", "register data iterations" },
  { "mrl", "Xdx", cmd_memory_read_io, Long, 0, "Reads a longword from memory in I/O address space", "address iterations silent" },
  { "mrw", "Xdx", cmd_memory_read_io, Word, 0, "Reads a word from memory in I/O address space", "address iterations silent" },
  { "mrb", "Xdx", cmd_memory_read_io, Byte, 0, "Reads a byte from memory in I/O address space", "address iterations silent" },
  { "mwl", "XXd", cmd_memory_write_io, Long, 0, "Writes a longword to memory in I/O address space", "address data iterations" },
  { "mww", "XXd", cmd_memory_write_io, Word, 0, "Writes a word to memory in I/O address space", "address data iterations" },
  { "mwb", "XXd", cmd_memory_write_io, Byte, 0, "Writes a byte to memory in I/O address space", "address data iterations" },
  { "sq", "XXSx", cmd_search, Quad, 0, "Searches for quadword pattern in specified memory range",
      "start_address end_address string inverse" },
  { "sl", "XXSx", cmd_search, Long, 0, "Searches for longword pattern in specified memory range",
      "start_address end_address string inverse" },
  { "sw", "XXSx", cmd_search, Word, 0, "Searches for word pattern in specified memory range",
      "start_address end_address string inverse" },
  { "sb", "XXSx", cmd_search, Byte, 0, "Searches for byte pattern in specified memory range",
      "start_address end_address string inverse" },

/*
 * PCI Commands
 */
#ifdef NEEDPCI
  { "pcishow", "ddd", cmd_pcishow, 0, 0, "Shows information about PCI", "id bus function" },
  { "prl", "XDdd", cmd_cfgr, Long, 0, "Reads a longword from specified address in PCI configuration space",
    "pci_address id bus function"},
  { "prw", "XDdd", cmd_cfgr, Word, 0, "Reads a word from specified address in PCI configuration space",
      "pci_address id bus function"},
  { "prb", "XDdd", cmd_cfgr, Byte, 0, "Reads a byte from specified address in PCI configuration space",
      "pci_address id bus function"},
  { "pwl", "XDXdd", cmd_cfgw, Long, 0, "Writes a longword to a specified address in PCI configuration space",
      "pci_address id data bus function"},
  { "pww", "XDXdd", cmd_cfgw, Word, 0, "Writes a word to a specified address in PCI configuration space",
      "pci_address id data bus function"},
  { "pwb", "XDXdd", cmd_cfgw, Byte, 0, "Writes a byte to a specified address in PCI configuration space",
      "pci_address id data bus function"},
#endif /* NEEDPCI */

/*
 * Utility Commands
 */
#ifdef NEEDFLASHMEMORY
  { "flash", "xxx", cmd_flash, 0, 0, "Programs image into flash memory", "source_address destination_offset bytes_to_write" },
  { "flasherase", "xx", cmd_flash, 1, 0, "Erases data from flash memory", "starting_offset bytes_to_erase" },
#ifdef NEEDFLOPPY
  { "fwupdate", "", cmd_fwupdate, 0, 0, "Runs \"Standalone Firmware Update Utility\" (FWUPDATE.EXE) from floppy drive", "" },
#endif
#endif
#if !(defined(EB164) || defined(DP264) || defined(CLUBS))
  { "bcon", "", cmd_bc, 1, 0, "Enables the Backup Cache except on 21164 and 21164PC boards", "" },
  { "bcoff", "", cmd_bc, 0, 0, "Disables the Backup Cache except on 21164 and 21164PC boards", "" },
#endif
  { "date", "s", cmd_date, 0, 0, "Modifies or displays date and time", "yymmddhhmmss" },
  { "apropos", "s", cmd_help, 1, 0, "Displays help descriptions for a specified keyword", "keyword" },
  { "help", "s", cmd_help, 0, 0, "Displays a brief help description for a specified command", "command_name" },
  { "h", "s", cmd_help, 0, 0, "Displays a brief help description for a specified command (abbreviation)", "command_name" },
  { "ident", "xx", cmd_ident, 0, 0, "Displays RCS ID strings found in the specified memory range", "start_address end_address" },
  { "version", "", cmd_version, 0, 0, "Prints Debug Monitor version information", "" },
  { "sysshow", "", cmd_sysshow, 0, 0, "Print SROM parameters", "" },
  { "swpipl", "d", cmd_swpipl, 0, 0, "Sets or Displays Interrupt Priority Level.", "ipl" },
  { "mces", "x", cmd_mces, 0, 0, "Sets or Displays the Machine Check Error Summary Register.", "msces_data" },
  { "wrfen", "X", cmd_wrfen, 0, 0, "Enables/Disables Floating Point.", "fen" },

/*
 * Debug Commands
 */
  { "preg", "x", cmd_preg, 0, 1, "Displays CPU general purpose registers at specified PALcode impure address", "address" },
  { "pfreg", "x", cmd_preg, 1, 1, "Displays CPU floating-point registers at specified PALcode impure address", "address" },
  { "creg", "DX", cmd_creg, 0, 1, "Modifies CPU general purpose registers", "register_number value" },
  { "cfreg", "DX", cmd_creg, 1, 1, "Modifies CPU floating-point registers", "register_number value" },
#ifdef NEEDDEBUGGER
  { "stop", "X", cmd_setbpt, 0, 0, "Sets a breakpoint at the specified address", "address"},
  { "bpstat", "", cmd_bpstat, 0, 0, "Displays the current breakpoint status", ""},
  { "next", "", cmd_step, 1, 0, "Similar to the step command but does not step into routines.", "" },
  { "n", "", cmd_step, 1, 0, "Similar to the step command but does not step into routines.", "" },
  { "step", "", cmd_step, 0, 0, "Execute the next instruction after a breakpoint", "" },
  { "s", "", cmd_step, 0, 0, "Execute the next instruction after a breakpoint (abbreviation)", "" },
  { "cont", "", cmd_continue, 0, 0, "Continue execution from a breakpoint", ""},
  { "delete","S", cmd_clearbpt, 0, 0, "Removes a breakpoint at the specified address", "address"},
  { "ladebug", "", cmd_ladebug, 0, 0, "Starts the DECladebug server for a remote debug session", ""},
#endif /* NEEDDEBUGGER */
#ifdef TRACE_ENABLE
  { "setvar", "ss", cmd_setvar, 0, 0, "Sets internal variable to specified value", "variable value" },
  { "prtrace", "x", cmd_prtrace, 0, 0, "Enables or disables tracing by debug prints", "status" },
#endif /* TRACE_ENABLE */
/*
 * Miscellaneous Commands
 */
#ifdef NEEDSCTL
  { "rsys","",cmd_insysctl,0, 0, "Reads the EB64 System Control Register", "" },
  { "wsys","X",cmd_outsysctl,0, 0, "Writes the EB64 System Control Register", "data" },
#endif /* NEEDSCTL */
#if defined(DC21064) || defined(DC21066)
  { "rabox","",cmd_ripr,CSERVE_K_RD_ABOX,0, "Reads the CPU ABOX_CTL register", "" },
  { "wabox","X",cmd_wipr,CSERVE_K_WR_ABOX,0, "Writes the CPU ABOX_CTL register", "data" },
#endif
#ifdef DC21064
  { "rbiu","",cmd_ripr,CSERVE_K_RD_BIU,0, "Reads the CPU BIU_CTL regiter", "" },
  { "wbiu","X",cmd_wipr,CSERVE_K_WR_BIU,0, "Writes to the CPU BIU_CTL register", "data" },
#endif /* DC21064 */

#if defined(DC21064) || defined(DC21066) || defined(DC21164) || defined(DC21164PC)
  { "riccsr","",cmd_ripr,CSERVE_K_RD_ICCSR,0, "Reads the CPU ICCSR register", "" },
  { "wiccsr","X",cmd_wipr,CSERVE_K_WR_ICCSR,0, "Writes to the CPU iccsr register", "data" },
#endif

#ifdef DC21164
  { "rbctl","",cmd_ripr,CSERVE_K_RD_BCCTL,0, "Reads the BCache Control regiter", "" },
  { "wbctl","Xx",cmd_wbcache, 0,0, "Writes to the BCache Control [and Configuration] register", "bctl_data [bcfg_data]" },
  { "rbcfg","",cmd_ripr,CSERVE_K_RD_BCCFG,0, "Reads the BCache Configuration register", "" },
  { "wbcfg","Xx",cmd_wbcache, 1,0, "Writes to the BCache Configuration [and Control] register", "bcfg_data [bctl_data]" },
#endif /* DC21164 */


  { "iack", "", cmd_iack, 0, 0, "Performs an Interrupt Acknowledge cycle", "" },
  { "rmode", "x", cmd_rmode, 0, 0, "Sets the dis command register display mode. (0=register name, 1=software name)", "mode" },
  { "setty", "s", cmd_ctty, 0, 0, "Specifies the port (COM1,... COM4 or GRAPHICS) used for Debug Monitor interaction", "port" },
  { "setbaud", "Sd", cmd_setbaud, 0, 0, "Sets communication port baud rate", "port baud_rate" },
  { "tip", "S", cmd_tip, 0, 0, "Connects to specified communication port", "port" },

  { "cominit", "", cmd_cominit, 0, 0, "Initializes Com ports", "" },
  { "vinit", "", cmd_videoinit, 0, 0, "Initializes Video controller and displays a", "" },
/*
 * Ethernet Commands
 */
  { "edevice","d", cmd_netdevice, 0, 0, "Selects a registered Ethernet device", "device_number"},
  { "eshow", "", cmd_ethershow, 0, 0, "Displays all registered Ethernet devices", "" },
  { "ereg", "", cmd_etherpreg, 0, 0, "Displays Ethernet controller registers", "" },
  { "estat", "", cmd_etherstat, 0, 0, "Displays Ethernet statistics", "" },
  { "einit", "", cmd_etherinit, 0, 0, "Initializes Ethernet controller and displays Ethernet hardware address", "" },
  { "estop", "", cmd_etherstop, 0, 0, "Stops Ethernet controller", "" },
  { "ebuff", "x", cmd_etherbuff, 0, 0, "Sets base address for Ethernet DMA buffers", "address" },
  { "edmp", "x", cmd_etherdump, 0, 0, "Sets or clears display of packets received or transmitted", "status" },
  { "eprom", "x", cmd_etherprom, 0, 0, "Sets or clears flag for receiving all packets (promiscuous mode)", "status" },
  { "arpshow", "", cmd_arpshow, 0, 0, "Displays all known ARP entries", ""},
#ifdef NEED_ETHERNET_ADDRESS
  { "eaddr", "s", cmd_eaddr, 0, 0, "Sets or Clears the default Ethernet MAC address", ""},
#endif

/*
 * Diagnostics Command
 */
  { "mcheck","S",cmd_mcheck,0,0,"Enables/disables processor and system machine checks.", "{ON | OFF | CPU | SYSTEM}"},
  { "beep","DD",cmd_beep,0,0,"Beeps speaker for specified duration (milliseconds) and frequency (Hertz)", "duration frequency"},
  { "memtest","dxxxsx",cmd_memtest,0,0,"Test memory range. (Makes longword (32-bit) accesses to memory)",
      "iterations start_address end_address increment mcheck stop_drivers pattern"},
#ifndef _WIN32
/* the memt.c lib module needs to be ported to work on NT due to the 64/32 long and pointer differences */
  { "mt","",cmd_mt,0,0,"Measure memory bandwidth.", ""},
#endif
  { NULL, NULL, NULL, 0, 0, NULL, NULL }
};

static void Help(int apropos)
{
  const struct KEY *p;
  int linelgth = 0;
  int pagelgth = 0;
  int keywlgth;
  p = &keyarray[0];
  if ((argc == 1) && !apropos)
  {

    printf("\nA brief help description is available for each of the\n");
    printf("following commands.\n\n");

    pagelgth = 4;

    while (p->keyw)
    {
      keywlgth = strlen(p->keyw);
      linelgth += _MAX(13, keywlgth);

      if (linelgth > 79) {
        if (++pagelgth > 24) {
          if (!kbdcontinue()) return;
          pagelgth = 1;
        }
        printf("\n");
        linelgth = _MAX(13, keywlgth);
      }
      printf("  %-11s", p->keyw);
      p += 1;
    }

    if (pagelgth > (24-9))	/* Can 9 more lines be displayed? */
      if (!kbdcontinue()) return;
    printf("\nexamples:\n\n");

#ifdef PROMPT
    printf(PROMPT);
#else
    printf("DECDBM> ");
#endif
    printf(" help command_keyword\n");
    printf("Displays help for command_keyword, where command_keyword\n");
    printf("is any command name that appears in the command list.\n\n");

#ifdef PROMPT
    printf(PROMPT);
#else
    printf("DECDBM> ");
#endif
    printf(" help *\n");
    printf("Displays help for all commands in the command list.\n\n");

  }
  else
    CommandHelp(argv[1], apropos);
}

static void CommandHelp(char *arg, int apropos)
{
  const struct KEY *p;
  int pagelgth = 0;
  char *s, synkey;
  p = &keyarray[0];
  while (p->keyw)
    {
      if ((strcmp(p->keyw, arg) == 0)
          || (strcmp("*", arg) == 0)
          || (apropos &&
              ((strstr(p->keyw, arg) != NULL)
               || (strstr(p->helpstr, arg) != NULL))))
        {
          if ((pagelgth += 5) > 24)
            {
              pagelgth = 5;
              if (!kbdcontinue()) break;
            }
          printf("%s:\n %s\n", p->keyw, p->helpstr);
          printf(" syntax: %s %s\n", p->keyw, p->syntaxstr);
          s = p->syntax;
          printf(" arguments: ");
          while(*s)
            {
              synkey = *s++;
              if (synkey == 'X') printf("<hex> ");
              if (synkey == 'x') printf("<opt hex> ");
              if (synkey == 'D') printf("<dec> ");
              if (synkey == 'd') printf("<opt dec> ");
              if (synkey == 'S') printf("<str> ");
              if (synkey == 's') printf("<opt str> ");
            }
          printf("\n\n");
        }
      p += 1;
    }
}


void (*Lookup(void))(void)
{
  const struct KEY *p;
  int matchok;
  ui wantargs, i;
  char synkey;
  p = &keyarray[0];
  while (p->keyw)
  {
    if (strcmp(argv[0], p->keyw) == 0)
    {
      matchok = TRUE;
      wantargs = strlen(p->syntax);
      for (i = 1; i <= wantargs; i += 1)
      {
        synkey = p->syntax[i-1];
        if ((synkey == 'X') && !hexargok[i]) matchok = FALSE;
        if (synkey=='x' && i<argc && !hexargok[i]) matchok = FALSE;
        if ((synkey == 'D') && !decargok[i]) matchok = FALSE;
        if (synkey=='d' && i<argc && !decargok[i]) matchok = FALSE;
        if (synkey=='S' && i>=argc) matchok = FALSE;
      }
      if (matchok) {
        fixarg = p->fixarg;
        if (p->dual_cmd)
        {
/*          printf("\nsecondary_cpu_function %lx %lx\n",&secondary_cpu_function,p->func); */
          secondary_cpu_function = p->func;
        }
        return(p->func);
      }
      printf("\nSyntax error!!!\n\n");
      CommandHelp(argv[0], 0);
      return(NULL);
    }
    p += 1;
  }
  printf("unknown command\n");
  return(NULL);
}
