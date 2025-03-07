#ifndef __MON_H_LOADED
#define __MON_H_LOADED
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
 *  $Id: mon.h,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $;
 */

/*
 * $Log: mon.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:07  paradis
 * Initial CVS checkin
 *
 * Revision 1.42  1997/12/15  20:48:26  pbell
 * Updated for dp264.
 *
 * Revision 1.41  1997/08/14  20:54:24  pbell
 * Added definition for ip_port function InitTty.
 *
 * Revision 1.40  1997/07/10  00:29:53  fdh
 * Added PortIDToString() and StringToPortID().
 *
 * Revision 1.39  1997/06/09  15:43:04  fdh
 * Modified the FileWriteRange declaration.
 *
 * Revision 1.38  1997/06/02  04:31:48  fdh
 * Added EstablishInitData().
 *
 * Revision 1.37  1997/05/02  18:20:12  fdh
 * Removed a prototype.
 *
 * Revision 1.36  1997/04/30  19:43:55  pbell
 * Add prototype for UngetChar.
 *
 * Revision 1.35  1997/04/29  21:53:51  pbell
 * Removed ffsrec definitions and added prototypes for xmodem.c
 *
 * Revision 1.34  1997/04/23  17:41:45  pbell
 * Extended enum PORTS to include COM3 and COM4.
 *
 * Revision 1.33  1997/03/17  19:11:34  fdh
 * Added init_storage_pool().
 *
 * Revision 1.32  1997/02/21  04:19:06  fdh
 * Added the CommandExec declaration.
 *
 * Revision 1.31  1996/08/20  02:48:43  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.30  1996/06/13  03:23:43  fdh
 * Added BROADCAST and UNDEFINED_PORT port types.
 *
 * Revision 1.29  1996/05/22  21:28:51  fdh
 * Added SROM port definitions.
 *
 * Revision 1.28  1996/02/14  18:55:34  cruz
 * Changed return type of ChecksumMem to int.
 *
 * Revision 1.27  1995/10/31  19:34:34  cruz
 * Commented out unused functions.
 *
 * Revision 1.26  1995/10/31  16:30:42  fdh
 * Added UNINITIALIZED port type.
 * console_port_init() returns type STATUS.
 *
 * Revision 1.25  1995/10/26  23:51:00  cruz
 * Removed some functions prototypes for functions that
 * were not external.  Added extern declaration for the
 * i/o ports
 *
 * Revision 1.24  1995/10/24  18:29:38  fdh
 * Added Function_t definition and modified ExecuteProgram to reference it.
 *
 * Revision 1.23  1995/10/22  05:08:41  fdh
 * Added prototypes for PrintPorts(), FlushPorts(), PortsGetChar()
 * and renamed user_main() to main_loop().
 *
 * Revision 1.22  1995/10/18  12:53:32  fdh
 * Changed argument names passed to ExecuteProgram().
 *
 * Revision 1.21  1995/10/04  23:45:50  fdh
 * Updated function prototypes.
 *
 * Revision 1.20  1995/10/03  03:28:38  fdh
 * Removed some obsolete prototypes.
 *
 * Revision 1.19  1995/09/12  21:31:27  fdh
 * Added edit_cmdline() and renamed RawMonGetChar() to MonGetChar().
 *
 * Revision 1.18  1995/09/05  18:44:04  fdh
 * Added Port_t definitions
 *
 * Revision 1.17  1995/09/02  03:51:12  fdh
 * now included definitions for fftty.c
 *
 * Revision 1.16  1995/08/25  19:37:47  fdh
 * Modified to support callbacks.
 *
 * Revision 1.15  1995/02/23  21:49:05  fdh
 * Added prototype for CompareMem().
 *
 * Revision 1.14  1994/11/08  21:43:55  fdh
 * Added include for lib.h
 *
 * Revision 1.13  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.12  1994/06/23  13:43:18  rusling
 * Fixed up WNT compiler warnings.
 *
 * Revision 1.11  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.10  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.9  1994/06/13  15:15:03  rusling
 * Added more function prototypes.
 *
 * Revision 1.8  1994/06/03  20:15:43  fdh
 * Removed lib.h include and declaration for memtest.
 *
 * Revision 1.7  1994/04/03  00:35:49  fdh
 * Modified PrintMem() prototype.
 *
 * Revision 1.6  1994/04/01  22:41:46  fdh
 * Removed obsoleted global variable.
 *
 * Revision 1.5  1994/03/16  00:12:01  fdh
 * Added some prototypes...
 *
 * Revision 1.4  1994/03/13  14:37:45  fdh
 * Modified memtest prototype.
 *
 * Revision 1.3  1994/01/19  10:22:28  rusling
 * Ported to ALpha Windows NT.
 *
 * Revision 1.2  1993/10/02  23:37:53  berent
 * Merge in development by Anthony Berent
 *
 *>> Revision 1.2  1993/08/06  10:45:27  berent
 *>> Added MONITOR_ETHER_DEVICE constant. This defines which ethernet device the
 *>> monitor uses.
 *
 * Revision 1.1  1993/06/08  19:56:15  fdh
 * Initial revision
 *
 */

/* definitions for basic things */

#include "system.h"
#include "lib.h"
#include "uart.h"

extern ul bootadr;

typedef enum PORTS {
  KBD,
  COM1,
  COM2,
  COM3,
  COM4,
  SROM,
  BROADCAST,
  LPT,
  VGA,
  TGA,
  P8514,
  UNDEFINED_PORT,
  UNINITIALIZED
} Port_t;

/*
 *  The following variables contain the address of the
 *  ports to be used for input and output.
 *  Their space is allocated in "io_port.c"
*/
extern Port_t outputport;
extern Port_t inputport;
extern Port_t GraphicsPort;

typedef int (* Function_t)(int, char *);

/* some constants */
#define RDFILE 0
#define VERIFY 1
#define MONITOR_ETHER_DEVICE 0  /* Ethernet port number to be used by monitor */

/*::::::::::::::
cmd.c
::::::::::::::*/
extern void (*Lookup(void ))(void);


/*::::::::::::::
rtc.c
::::::::::::::*/
extern void printDate(void );
extern void InitRTC(void );
extern void setDate(ub * date);
extern time_t time(time_t * out);


/*::::::::::::::
dis.c
::::::::::::::*/
extern void dis(int argc , ul first , ul last);


/*::::::::::::::
ffexec.c
::::::::::::::*/
extern void ExecuteProgram(Function_t place, int _argc , char * _argv[]);
extern void PrintMem(int size , ul first , ul last , ul iterations , int silent);
extern void CompareMem(ul first , ul last , ul copy);
extern int ChecksumMem(ul first , ul last);
extern void ChangeMem(int size , ul place);
extern void FillMem(ul first , ul last , ui value);
extern void PrintVersion(void );
extern void jToPal(ul destaddr);
extern uintptr_t get_bootadr(void );
extern char * halt_code_string(int halt_code);

#ifdef NOT_IN_USE
extern ul CurrentDefault(void );
extern void StackTrace(void );
#endif

/*::::::::::::::
file.c
::::::::::::::*/
extern DBM_STATUS LoadAFile(char * SourceFile , char * Destination);
extern DBM_STATUS FileWriteRange(char * Filename , ul StartAddress , size_t Size);
extern DBM_STATUS FileDirectory(char * Directory);
extern DBM_STATUS FileCopy(char * SourceFile , char * DestinationFile);


/*::::::::::::::
io_port.c
::::::::::::::*/
extern void PutChar(char c);
extern int  GetChar(void );
extern void UngetChar(char c);
extern int  CharAv(void );
extern int  RawGetChar(void );
extern void FlushChar(void);
extern DBM_STATUS console_port_init(void );
extern DBM_STATUS console_video_init(void);
extern void PortsFlushChar(void);
extern int  PortsGetChar(void);
extern void PortsPutChar(char c);
extern char *PortIDToString(Port_t port);
extern Port_t StringToPortID(char *arg);
extern void InitTty( char * PortName, int BaudRate );
extern void ctty(char *arg);
extern void tip(char *arg);

/*::::::::::::::
main.c
::::::::::::::*/
extern int main(int argc, char *argv[]);
extern void CommandExec(void );
extern void EstablishInitData (void);

/*::::::::::::::
malloc.c
::::::::::::::*/
extern void malloc_summary(char * string);
extern void * malloc(size_t size);
extern void free(void * pointer);
extern void * realloc(void * pointer , size_t size);
extern void init_storage_pool(void);


/*::::::::::::::
From mouse.c
::::::::::::::*/
extern BOOLEAN IsMousePresent( int PortBase );	/* returns TRUE if a mouse is found else FALSE */

/*::::::::::::::
pReg.c
::::::::::::::*/
extern void printReg(ul * register_array , int group);
extern void changeReg(ul * register_array , int group , int index , ul data);


/*::::::::::::::
sromport.c
::::::::::::::*/
extern int SromCharav(void);
extern int SromGetChar(void);
extern void SromPutChar(char c);
extern int SromPortInit( BOOLEAN PromptUser );

/*::::::::::::::
xmodem.c
::::::::::::::*/
extern BOOLEAN	XReceive( ul Address );

#endif /* __MON_H_LOADED */
