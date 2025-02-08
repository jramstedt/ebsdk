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
__attribute__((unused)) static const char *rcsid = "$Id: io_port.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: io_port.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.34  1998/08/03  17:53:03  gries
 * remove \ from first line
 *
 * Revision 1.33  1998/06/26  18:21:38  gries
 * changes for video init
 *
 * Revision 1.32  1997/12/15  20:52:50  pbell
 * Updated for dp264.
 *
 * Revision 1.31  1997/08/14  20:50:09  pbell
 * Added InitTty command to init any com port including the SROM.
 *
 * Revision 1.30  1997/07/28  20:20:15  pbell
 * Fixed StringToPortID().
 *
 * Revision 1.29  1997/07/25  22:16:57  pbell
 * Updated for Srom time out support on SromGetChar.
 *
 * Revision 1.28  1997/07/25  21:24:39  fdh
 * Added a compile conditional to disable the keyboard.
 * What if you are at home and you want the effect of
 * disconnecting the keyboard.
 *
 * Revision 1.27  1997/07/10  00:42:38  fdh
 * Added PortArray[], PortIDToString(), and StringToPortID().
 *
 * Revision 1.26  1997/05/22  20:10:28  fdh
 * Added the NEEDPCI conditional around the call to IOPCIClearNODEV();
 *
 * Revision 1.25  1997/05/19  17:19:28  fdh
 * Clear any pending PCI abort errors causes by the bios emulator.
 *
 * Revision 1.24  1997/04/30  21:12:17  pbell
 * Updated PortsGetChar to scan only FirstNormalCommPort.
 * Added UngetChar and updated GetChar and CharAv to support it.
 *
 * Revision 1.23  1997/04/23  17:48:57  pbell
 * Extended comm port routines to use com1-com4.
 * Changed the initialization routines to check
 * for the presence of com ports.
 *
 * Revision 1.22  1996/08/22  14:05:20  fdh
 * Modified tip() to work with all ports and moved it
 * along with other I/O port functions into this module.
 *
 * Revision 1.21  1996/08/20  02:50:57  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.20  1996/08/17  13:24:35  fdh
 * Corrected conditions which prevented failed
 * graphics init status from being reported.
 *
 * Revision 1.19  1996/08/16  17:16:42  fdh
 * Added LED codes for keyboard init, and graphics failed.
 * Added a beep code for graphics failed.
 *
 * Revision 1.18  1996/06/13  03:40:10  fdh
 * Modified PortsGetChar to support the BROADCAST
 * port type.
 *
 * Revision 1.17  1996/05/22  22:00:23  fdh
 * Added support for communicating with the Debug Monitor
 * through the SROM serial port.
 * Added resonable delays and retries when flushing buffers
 * for ports.
 *
 * Revision 1.16  1995/10/31  22:45:15  cruz
 * Added #ifdef NEEDTGA around the #include of tga.h
 *
 * Revision 1.15  1995/10/31  16:29:33  fdh
 * Added UNINITIALIZED Port type instead of assuming
 * that the uninitialized graphics port is set to COM1.
 * Use STATUS type for status veriable.
 *
 * Revision 1.14  1995/10/26  21:47:51  cruz
 * Made some local variables static.  Removed declarations of
 * external variables since they are now included in lib.h
 *
 * Revision 1.13  1995/10/26  18:32:32  cruz
 * Performed some cleanup.
 *
 * Revision 1.12  1995/10/23  15:20:07  fdh
 * Modified startup messages a little.
 *
 * Revision 1.11  1995/10/22  04:52:40  fdh
 * Added PrintPorts(), FlushPorts(), PortsGetChar() to handle
 * multiple console ports simultaneously.
 *
 * Revision 1.10  1995/09/29  20:19:54  fdh
 * Added RawGetChar()
 *
 * Revision 1.9  1995/09/12  21:24:40  fdh
 * Modified GetChar to return an int instead of a char.
 *
 * Revision 1.8  1995/09/05  18:37:12  fdh
 * Moved general console port support to this module.
 *
 * Revision 1.7  1995/09/02  06:43:54  fdh
 * Removed extern ui CPU_cycles_per_usecs;
 *
 * Revision 1.6  1995/09/02  06:30:01  fdh
 * Removed WaitUs(). Moved to time.h
 *
 * Revision 1.5  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.4  1994/07/13  14:19:39  fdh
 * Modified WaitUs to use wait_cycles to more accurately control wait loops.
 * This way the wait loops still work when compiled with optimizations.
 *
 * Revision 1.3  1994/06/03  18:53:48  fdh
 * Added a couple of global declarations and fixed up include declaration.
 *
 * Revision 1.2  1994/01/19  10:40:08  rusling
 * Ported to Alpha Windows NT.
 *
 * Revision 1.1  1993/06/08  19:56:22  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "mon.h"
#include "lib.h"
#include "console.h"
#include "callback.h"
#include "ledcodes.h"
#include "beepcode.h"

/*----------------------
 * Comm port status data
 *---------------------*/

typedef enum PORTSTATUS
{
    COMPORT_UNTESTED,
    COMPORT_HASAMOUSE,
    COMPORT_NORMAL,
    COMPORT_MISSING
} PORTSTATUS;

struct PORT_TYPES {
  Port_t PortType;
  char *String;
} PortArray[] = {
  { KBD, "GRAPHICS" },
  { COM1, "COM1" },
  { COM2, "COM2" },
  { COM3, "COM3" },
  { COM4, "COM4" },
  { SROM, "SROM" },
  { BROADCAST, "BROADCAST" },
  { LPT,  "PARALLELPORT" },
  { VGA,  "VGA" },
  { TGA,  "TGA" },
  { P8514, "P8514" },
  { UNDEFINED_PORT, "UNDEFINED" },
  { UNINITIALIZED, "UNINITIALIZED"},
  { 0, NULL}
};

PORTSTATUS CommPortStatus[4] = { COMPORT_UNTESTED, COMPORT_UNTESTED, COMPORT_UNTESTED, COMPORT_UNTESTED };
Port_t FirstNormalCommPort = COM1;

#ifndef NEEDPCI
#undef NEEDTGA
#endif

#ifdef NEEDTGA
#include "tga.h"
#endif

extern BOOLEAN SromPortInitialized;
extern BOOLEAN BootSROMPort;

Port_t outputport = COM1;
Port_t inputport = COM1;
Port_t GraphicsPort = UNINITIALIZED;
int PortSelected = FALSE;	/* Use all ports until the user selects one. */

#ifdef NEEDVGA
#ifdef NEEDVGA_BIOSEMULATION
static unsigned int bios_return_data[2];
extern int StartBiosEmulator(unsigned int *bios_return);
extern void SetupBiosGraphics(void);
#endif
#endif


void PutChar(char c)
{
  switch(outputport)
    {
    case COM1:
    case COM2:
    case COM3:
    case COM4:
      uart_putchar(outputport, c);
      return;

    case SROM:
      SromPutChar(c);
      return;

    case LPT:
      putcLpt((int)c);
      return;	

#ifdef NEEDVGA
    case VGA:
      vgaputc((int)c);
      return;
#endif

#ifdef NEED8514
    case P8514:
      c8514putc((int)c);
      return;
#endif

#ifdef NEEDTGA
    case TGA:
      tgaputc((int)c);
      return;
#endif

    default:
      return;   
    }
}

unsigned CharsSaved = 0;
char SavedChars[4];

void UngetChar( char c )
{
    if( CharsSaved < 4 )
	SavedChars[CharsSaved++] = c;
}

int GetChar(void)
{
    if( CharsSaved )
	return( SavedChars[--CharsSaved] );
    else
    {
	int c;

	while (!CharAv())
	    usleep(50);
	while( (c = RawGetChar()) & ~0x0ff );
	if (c == '\r') c = '\n';
	return (c);
    }
}

int CharAv(void)
{
  if( CharsSaved )
      return( TRUE );
  switch(inputport) {
  case COM1:
  case COM2:
  case COM3:
  case COM4:
    return(uart_charav(inputport));
  case SROM:
    return(SromCharav());
  case KBD:
    return(kbd_charav());
  default:
    return (TRUE);
  }
}

int RawGetChar(void)
{
  int c;

  switch(inputport) {
  case COM1:
  case COM2:
  case COM3:
  case COM4:
    c = uart_getchar(inputport);
    break;
  case SROM:
    c = SromGetChar();
    break;
  case KBD:
    c = kbd_getc();
    break;
  default:
    c = -1;
  }
  return (c);
}

/*
 * Retry over a 5 msec period
 * for a new character to flush
 */
#define RETRY_COUNT 100
void FlushChar(void)
{
  int count = RETRY_COUNT;
  CharsSaved = 0;
  while (count--) {
    if (CharAv()) {
      RawGetChar();
      count = RETRY_COUNT;
      continue;
    }
    usleep(50);
  }
}

static PORTSTATUS InitCommPort( int PortBase )
{
    PORTSTATUS Result;
    if( !UartInitLine( PortBase, 9600 ) )
	Result = COMPORT_MISSING;
    else if( IsMousePresent( PortBase ) )
	Result = COMPORT_HASAMOUSE;
    else
	Result = COMPORT_NORMAL;

    return( Result );
}

void InitCommPorts( void )
{
    int i;
    BOOLEAN fnp = FALSE;

    for( i=0; i<4; i++ )
    {
	CommPortStatus[i] = InitCommPort( UartPortBaseTable[i] );
	if( !fnp && CommPortStatus[i] == COMPORT_NORMAL )
	{
	    fnp = TRUE;
	    FirstNormalCommPort = inputport = outputport = CommPortFromNumber( i );
	}
    }
    outLed(led_k_uart_inited);  /* UARTs initialized */
}


DBM_STATUS console_video_init(void)
{
   int status;

#ifdef DISABLE_KEYBOARD
    status = FALSE;
#else
    status = kbd_init();
#endif

#ifndef FORCE_GRAPHICS_INIT
    if (status == TRUE) {
      printf("Keyboard initialized. Initializing Graphics.\n");
#else
    if (TRUE) {
      if (status == FALSE)
	printf("Keyboard unitialized. Initializing Graphics Anyway.\n");
#endif

    outLed(led_k_keyboard_inited);  /* Keyboard initialized */

#ifdef NEEDTGA
      if (DigitalTGADetected) {
	GRAPHICS_TYPE tgastatus = tgasetup();
	if (tgastatus != UNINITIALIZED_GRAPHICS) {
	  PRTRACE1("FwInitializeDigitalTGA returned\n"); 
	  inputport = KBD;
	  outputport = GraphicsPort = TGA;
	  return (STATUS_SUCCESS);
	}
	printf("\n\tError: TGA Grahics device initialization failed.\n");
      }
#endif

#ifdef NEED8514
      PRTRACE1("calling c8514init\n"); 
      if (c8514init()) {
        PRTRACE1("c8514init returned\n"); 
	inputport = KBD;
        outputport = GraphicsPort = P8514;
	return (STATUS_SUCCESS);
      }
      PRTRACE1("c8514init failed\n"); 
#endif /* NEED8514 */

#ifdef NEEDVGA
#ifdef NEEDVGA_BIOSEMULATION
      PRTRACE1("calling StartBiosEmulator\n"); 
      if ((status = StartBiosEmulator(bios_return_data))) {
	PRTRACE1("StartBiosEmulator Successful\n"); 
	SetupBiosGraphics();
      }

#ifdef NEEDPCI
      /* Clear any pending PCI abort errors causes by the bios emulator. */
      IOPCIClearNODEV();
#endif /* NEEDPCI */

      if (status) {
	vgasetup(); /* Init color palette, etc. */
	inputport = KBD;
	outputport = GraphicsPort = VGA;
	return(STATUS_SUCCESS);
      }
#else
      PRTRACE1("calling vgainit\n"); 
      vgainit();		/* FIX: vgainit should return status. */
      PRTRACE1("vgainit returned\n"); 
      inputport = KBD;
      outputport = GraphicsPort = VGA;
      return(STATUS_SUCCESS);
#endif
#endif /* NEEDVGA */

      /* If this point is reached no output port is found.
       * Revert back to the default input port.
       */
      outLed(led_k_graphics_failed);  /* Graphics failed */
      BeepCode(beep_k_graphics_failed); /* Beep to inform the user. */
      printf("\n\tGraphics initialization failed.");
      printf("\n\tReverting to Serial Communications Port %d\n",
	     CommPortToNumber( FirstNormalCommPort ) + 1 );

      inputport = FirstNormalCommPort;
      outputport = FirstNormalCommPort;
      GraphicsPort = UNINITIALIZED;
      return (STATUS_FAILURE);
    }
    return status;
}

DBM_STATUS console_port_init(void)
{ 
    DBM_STATUS status;

    /* Establish default ports. */
    inputport = COM1;
    outputport = COM1;
    GraphicsPort = UNINITIALIZED;
    SromPortInitialized = FALSE;
    PortSelected = FALSE;
    UserPutChar = PortsPutChar;
    UserGetChar = PortsGetChar;
    FlushFunction = PortsFlushChar;

#ifdef SROM_PORT_BOOT
    if (BootSROMPort) {
      /*
       *  When the SROM Serial Port is initialized it starts to
       *  poll the port wating for an upper-case U to be typed.
       *  The initialization code will use the U to determine the
       *  communication baud rate to the port. Before the port is
       *  initialized no data will be printed to the port.  Therefore,
       *  the user must know to type the upper-case U.
       *  This is basically provided to assist in hardware debugging
       *  when the I/O subsystem is not fully functional.  Also keep
       *  in mind that other I/O accesses might need to be disabled
       *  thoughout depending on the state of the I/O subsystem.
       */
      SromPortInit( FALSE );	/* Initialize the SROM Serial Port */
      ctty("SROM");		/* Select the SROM port. */
      return (STATUS_SUCCESS);	/* Ignore the other console devices. */
    }
#endif /* SROM_PORT_BOOT */

    InitCommPorts();

    PRTRACE1("in console_port_init... Uarts initialized!\n");

    printf("\n\n================  Starting Debug Monitor!!! ===================\n");

#ifdef NEEDPARALLELPORT
    /* lpt */
    outVti(lptCr,  0);
    for(i=0;i<lptDly;i++)
	;
    outVti(lptCr,   lptnInit|lptAFD);
    s = inVti(lptSr);
#endif

#ifdef DISABLE_KEYBOARD
    status = FALSE;
#else
    status = kbd_init();
#endif

#ifndef FORCE_GRAPHICS_INIT
    if (status == TRUE) {
      printf("Keyboard initialized. Initializing Graphics.\n");
#else
    if (TRUE) {
      if (status == FALSE)
	printf("Keyboard unitialized. Initializing Graphics Anyway.\n");
#endif

    outLed(led_k_keyboard_inited);  /* Keyboard initialized */

#ifdef NEEDTGA
      if (DigitalTGADetected) {
	GRAPHICS_TYPE tgastatus = tgasetup();
	if (tgastatus != UNINITIALIZED_GRAPHICS) {
	  PRTRACE1("FwInitializeDigitalTGA returned\n"); 
	  inputport = KBD;
	  outputport = GraphicsPort = TGA;
	  return (STATUS_SUCCESS);
	}
	printf("\n\tError: TGA Grahics device initialization failed.\n");
      }
#endif

#ifdef NEED8514
      PRTRACE1("calling c8514init\n"); 
      if (c8514init()) {
        PRTRACE1("c8514init returned\n"); 
	inputport = KBD;
        outputport = GraphicsPort = P8514;
	return (STATUS_SUCCESS);
      }
      PRTRACE1("c8514init failed\n"); 
#endif /* NEED8514 */

#ifdef NEEDVGA
#ifdef NEEDVGA_BIOSEMULATION
      PRTRACE1("calling StartBiosEmulator\n"); 
      if ((status = StartBiosEmulator(bios_return_data))) {
	PRTRACE1("StartBiosEmulator Successful\n"); 
	SetupBiosGraphics();
      }

#ifdef NEEDPCI
      /* Clear any pending PCI abort errors causes by the bios emulator. */
      IOPCIClearNODEV();
#endif /* NEEDPCI */

      if (status) {
	vgasetup(); /* Init color palette, etc. */
	inputport = KBD;
	outputport = GraphicsPort = VGA;
	return(STATUS_SUCCESS);
      }
#else
      PRTRACE1("calling vgainit\n"); 
      vgainit();		/* FIX: vgainit should return status. */
      PRTRACE1("vgainit returned\n"); 
      inputport = KBD;
      outputport = GraphicsPort = VGA;
      return(STATUS_SUCCESS);
#endif
#endif /* NEEDVGA */

      /* If this point is reached no output port is found.
       * Revert back to the default input port.
       */
      outLed(led_k_graphics_failed);  /* Graphics failed */
      BeepCode(beep_k_graphics_failed); /* Beep to inform the user. */
      printf("\n\tGraphics initialization failed.");
      printf("\n\tReverting to Serial Communications Port %d\n",
	     CommPortToNumber( FirstNormalCommPort ) + 1 );

      inputport = FirstNormalCommPort;
      outputport = FirstNormalCommPort;
      GraphicsPort = UNINITIALIZED;
      return (STATUS_FAILURE);
    }
    return (STATUS_SUCCESS);
}


void PortsPutChar(char c)
{
  uart_putchar(FirstNormalCommPort, c);

  if (GraphicsPort != UNINITIALIZED) {
    switch (GraphicsPort) {
#ifdef NEEDVGA
    case VGA:
      vgaputc((int)c);
      break;
#endif

#ifdef NEEDTGA
    case TGA:
      tgaputc((int)c);
      break;
#endif

#ifdef NEED8514
    case P8514:
      c8514putc((int)c);
      break;
#endif

    default:
      break;   
    }
  }
}


int PortsGetChar(void)
{
    int c;
    int i;
    int PortBase;

    /* check the FirstNormalCommPort for a terminal */

    i = CommPortToNumber( FirstNormalCommPort );
    PortBase = UartPortBaseTable[i];

    if( CommPortStatus[i] == COMPORT_NORMAL )
    {
	if( UartCharAv( PortBase ) )
	{
	    c = UartGetChar( PortBase );

	    if (!PortSelected)
	    {
		PortSelected = TRUE;
		UserGetChar = GetChar;
		UserPutChar = PutChar;
		FlushFunction = FlushChar;

		if (GraphicsPort != UNINITIALIZED)
		{
		    outputport = GraphicsPort;
		    printf( "setty COM%d\n", i+1 );
		}

		outputport = inputport = FirstNormalCommPort;
	    }

	    if (c == '\r') c = '\n';
	    return (c);
	}
    }
	
    if (GraphicsPort != UNINITIALIZED)
    {
	if (kbd_charav())
	{
	    c = kbd_getc();

	    if (!PortSelected)
	    {
		PortSelected = TRUE;
		UserGetChar = GetChar;
		UserPutChar = PutChar;
		FlushFunction = FlushChar;

		outputport = FirstNormalCommPort;
		printf("setty GRAPHICS\n");

		inputport = KBD;
		outputport = GraphicsPort;
	    }

	    if (c == '\r') c = '\n';
	    return (c);
	}
    }

    return (-1);
}


void PortsFlushChar(void)
{
  Port_t saveport = inputport;

  inputport = FirstNormalCommPort;
  FlushChar();		/* Flush any typeahead characters */

  if (GraphicsPort != UNINITIALIZED) {
    inputport = KBD;
    FlushChar();		/* Flush any typeahead characters */
  }

  inputport = saveport;
}

char *PortIDToString(Port_t port)
{
  struct PORT_TYPES *p;

  for( p = PortArray; p->String != NULL; p++ )
    if( p->PortType == port )
	return( p->String );

  return (NULL);
}

Port_t StringToPortID(char *arg)
{
  int i;
  struct PORT_TYPES *p;

  i = 0;
  while ((arg[i] = toupper((int)arg[i])) != '\0') ++i;

  for( p = PortArray; p->String != NULL; p++ )
    if( strcmp( arg, p->String ) == 0 )
	return( p->PortType );

  return( atoi(arg) );		/* Defaults to KBD */
}


extern void InitTty( char * PortName, int BaudRate )
{
    Port_t UserPort = StringToPortID( PortName );

    switch( UserPort )
    {
	case COM1:
	case COM2:
	case COM3:
	case COM4:
	    UartInitLine( CommPortBase( UserPort ), BaudRate );
	    break;

	case SROM:
	    SromPortInit( TRUE );
	    break;
	default:
	    break;
    }
}


void ctty(char *arg)
{
  Port_t port = StringToPortID(arg);
  switch (port) {
  case KBD:
    if (GraphicsPort != UNINITIALIZED) {
      inputport = KBD;
      outputport = GraphicsPort;
      UserPutChar = PutChar;
      UserGetChar = GetChar;
      FlushFunction = FlushChar;
    }
    else
      printf("\tGraphics have not been initialized.\n");
    break;
  case COM1:
  case COM2:
  case COM3:
  case COM4:
    inputport = port;
    outputport = port;
    UserPutChar = PutChar;
    UserGetChar = GetChar;
    FlushFunction = FlushChar;
    break;
  case BROADCAST:
    UserPutChar = PortsPutChar;
    UserGetChar = PortsGetChar;
    FlushFunction = PortsFlushChar;
    break;
  case SROM:
    if (!SromPortInitialized)
      SromPortInit( TRUE );
    inputport = SROM;
    outputport = SROM;
    UserPutChar = PutChar;
    UserGetChar = GetChar;
    FlushFunction = NULL;
    break;
  default:
    printf("\tUndefined Console Device.\n");
    return;
  }
  return;
}

void tip(char *arg)
{
  Port_t portin;
  Port_t portout;
  Port_t saveinput;
  Port_t saveoutput;
  int c;
  int lastout;

  portin = portout = StringToPortID(arg);
  if (portin == inputport) return;
  if (portin == UNDEFINED_PORT) return;
  if (portin == KBD) {
    if (GraphicsPort == UNINITIALIZED) {
      printf("\tGraphics have not been initialized.\n");
      return;
    }
    else portout = GraphicsPort;
  }

  printf("<CR>~. to exit\n");

  lastout = 0;
  saveinput = inputport;
  saveoutput = outputport;

  while(TRUE) {
    inputport = portin;
    if(CharAv() && !((c = RawGetChar()) & ~0x0ff) ) {
      outputport = saveoutput;
      PutChar((char)c);
    }

    inputport = saveinput;
    if(CharAv() && !((c = RawGetChar()) & ~0x0ff) ) {
      outputport = portout;
      if (lastout=='\r' && c=='~') {
	while(!CharAv() || ((c = RawGetChar()) & ~0x0ff) );
        if (c == '.') break;
        if (c == '?') {
	  outputport = saveoutput;
	  printf("<CR>~. to exit\n");
        }
        else {
          if (c != '~') PutChar((char)'~');
          lastout = c;
          PutChar((char)c);
        }
      }
      else {
        lastout = c;
        PutChar((char)c);
      }
    }
  }

  inputport = saveinput;
  outputport = saveoutput;
  PutChar('\n');
}
