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
__attribute__((unused)) static const char *rcsid = "$Id: pkthandl.c,v 1.1.1.1 1998/12/29 21:36:14 paradis Exp $";
#endif

/*
 * HISTORY FROM EB64 SERVER DEVELOPMENT STREAM:
 *
 * $Log: pkthandl.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:14  paradis
 * Initial CVS checkin
 *
 * Revision 1.17  1997/12/15  20:45:53  pbell
 * Updated for dp264.
 *
 * Revision 1.16  1997/02/20  19:01:41  fdh
 * When a client request a disconnect send the reply
 * before actually disconnecting.  Also give a little
 * time for retry requests to be sent.
 *
 * Revision 1.15  1995/11/27  15:49:03  cruz
 * Removed static initialization of "first_packet" to TRUE by renaming
 * variable to "first_packet_seen" and rewriting a few lines.  The reason
 * for doing this is to allow for the debug monitor to be reinitialized and
 * have all the variables in the same state as if it had been started from
 * scratch.
 *
 * Revision 1.14  1995/10/23  20:50:12  cruz
 * Performed some clean up.
 * Moved code for dumping packets while debugging to this module
 * from pktutil.c
 *
 * Revision 1.13  1995/09/26  22:33:16  cruz
 * Moved some of the debugging messages around.
 * Removed the #define DUMP_PACKETS
 *
 * Revision 1.12  1995/06/29  14:14:31  cruz
 * Added more debugging PRTRACE and TRACE_REGISTERS statements.
 *
 * Revision 1.11  1994/11/07  12:44:21  rusling
 * Now include both system.h and lib.h.
 *
 * Revision 1.10  1994/08/05  20:17:45  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.9  1994/06/28  20:12:18  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.8  1994/06/23  10:42:33  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.7  1994/06/17  19:36:43  fdh
 * Clean-up...
 *
 * Revision 1.6  1994/03/16  15:49:09  berent
 * Corrected || to | in NT changes
 *
 * Revision 1.4  1994/03/09  12:48:33  berent
 * Moved utility functions to pktutil.c. Fixed for NT compilation.
 * This is now identical to the version used in the OSF/1 server
 *
 * Revision 1.4  1993/10/01  15:52:48  berent
 * corrected type conversions on reading a long from a packet
 *
 * Revision 1.3  1993/08/09  11:49:22  berent
 * Removed the assumption that certain packet data is alligned.
 *
 * Revision 1.2  1993/06/08  22:32:06  berent
 * Changed to improve ladbx server communications
 *
 * Revision 1.1  1993/06/08  19:56:37  fdh
 * Initial revision
 *
 */


/*
 * Implementation of DebugPacket handling for the example ladebug debug server.
 *
 * 13-January-1993 T.B.
 * 5-April-1993 A.B.
 * 24-November-1993 A.B. 
 *
 */

#include "ladebug.h"
#include "bptable.h"

extern int kdebug_save_ipl;

#ifdef DUMP_PACKETS
static char *CommandToString(unsigned short command);
static char *ReplyToString(unsigned int reply);
static void DumpPacket(ub * packetP);
static void DumpRawPacket(ub * packetP);
#endif

static ui pid;
static int first_packet_seen;  /* Depends on "static" setting variable to 0. */
static unsigned int last_sequence_number;
static DebugPacket saved_packet;


/*
 * ProcessPacket.
 *
 *
 * The DebugPacket pointed to by packetP is both an input and
 * an output parameter.
 *
 * On input it contains a command to execute on behalf of the client.
 *
 * On output is contains the response to that command.
 *
 */
void ProcessPacket(DebugPacket *packet_structP)
{
  ub * packetP = &(packet_structP -> packet[0]);
  ub * specific_dataP = packetP+PACKET_SPECIFIC_DATA_OFFSET;

  short int reply = REPLY_OK;
  short unsigned int command_code;
/*
  char * clientUserName;
  char * serverUserName;
*/
  char * processName;
  char * standardIn;
  char * standardOut;
  char * standardError;
  char * argP;
  char * argv[255];
  address_value loadAddress;
  address_value startAddress;
  int argc;
  int idx; 
  int disconnect_requested = FALSE;

  PRTRACE1("------------ Packet Received -----------------\n");
#ifdef DUMP_PACKETS
  DumpPacket(packetP);
#endif
  if(ReadNetInt(packetP+PACKET_SEQNUM_OFFSET) == last_sequence_number && first_packet_seen) {
      const char retransmit_number = packetP[PACKET_RETRANS_COUNT_OFFSET];
      *packet_structP = saved_packet;
      packetP[PACKET_RETRANS_COUNT_OFFSET] = retransmit_number;
      PRTRACE1("------------ Retransmit Reply -----------------\n");
#ifdef DUMP_PACKETS
      DumpPacket(packetP);
#endif
      return;
  }

  command_code = ReadNetShort(packetP+PACKET_COMMAND_OFFSET);
  /* Check state */
  if(!this_client_is_connected() && command_code > CONNECTED) {
      reply = REPLY_NOT_CONNECTED;
  } else if (kpoll() == PROCESS_STATE_PROCESS_RUNNING && command_code > STOPPED) {
      reply = REPLY_NOT_STOPPED;
  }
  /* Check the pid */
/*  if(command_code > CONNECTED && ReadLocalInt(packetP+PACKET_PROCESS_ID_OFFSET) != pid) {
      reply = REPLY_BAD_PID;
  }*/

  /* if all still ok handle the command */
  if(reply == REPLY_OK) {
      switch (command_code) {
      case LOADPROCESS:
          PRTRACE1("LADBX Command: Load Process.\n");
	  if(kload_implemented()) {
	      if(a_client_is_connected()) {
		  reply = REPLY_ALREADY_CONNECTED;
		  break;
	      }
	      /* get client and server user names */
	      argP = (char *) specific_dataP;
/*	      clientUserName = argP; */
	      argP += strlen(argP) + 1;
/*	      serverUserName = argP; */

	      argP += strlen(argP) + 1;
	      PRINTERR2("Starting %s\n",argP);
	      argv[0] = processName =  argP;
	      argP += strlen(argP) + 1;
	      argc = *argP;
	      argP++;
	      for(idx=1; idx < argc; idx++) {
		  argv[idx] = argP;
		  argP += strlen(argP) + 1;
	      }
	      /* Terminate arg list with a zero argument */
	      argv[argc] = 0;
		  
	      /* Extract standard input, output, and error */
	      standardIn = argP;
	      argP += strlen(argP)+1;
	      standardOut = argP;
	      argP += strlen(argP)+1;
	      standardError = argP;
	      argP += strlen(argP)+1;
	      
	      /* Extract load and start addresses */
	      loadAddress = ReadLocalLong((unsigned char *) argP);
	      argP += MEM_DATA_ADDRESS_SIZE;
	      startAddress = ReadLocalLong((unsigned char *) argP);
	      argP += MEM_DATA_ADDRESS_SIZE;
		  
	      if((! kload(processName, 
			  argv, 
			  standardIn, 
			  standardOut, 
			  standardError,
			  loadAddress,
			  startAddress)) ||
		  kpoll() != PROCESS_STATE_PROCESS_AT_BREAK) {

		  reply = REPLY_BAD_LOAD_NAME;
		  break;
	      }
	      pid = kpid();
	      
	      PRINTERR2("Debuggee started; pid=%i\n",pid);
	      WriteLocalInt(packetP+PACKET_PROCESS_ID_OFFSET, pid); 
	      specific_dataP[PROCESS_INFO_PROCESSOR_OFFSET] = PROCESSOR_ALPHA;
	      specific_dataP[PROCESS_INFO_TYPE_OFFSET] = PROCESS_TYPE_USER;
	      set_connected();
	  }
	  else {
	      reply = REPLY_NOT_IMPLEMENTED;
	  }
	  break;
      case CONNECTTOPROCESS:
          PRTRACE1("LADBX Command: Connect to Process.\n");
	  if(a_client_is_connected()) {
	      reply = REPLY_ALREADY_CONNECTED;
	      break;
	  }
	  /* Otherwise identical to connect to process insist */
		/* fallthrough */
      case CONNECTTOPROCESSINSIST:
          PRTRACE1("LADBX Command: Connect to Process Insist.\n");
	  specific_dataP[PROCESS_INFO_PROCESSOR_OFFSET] = PROCESSOR_ALPHA;
	  specific_dataP[PROCESS_INFO_TYPE_OFFSET] = PROCESS_TYPE_USER;
	  pid = ReadLocalInt(packetP+PACKET_PROCESS_ID_OFFSET);
	  if(kconnect_implemented()) {
	      if(kconnect(pid)) {
		  set_connected();
	      } else {
		  reply = REPLY_BAD_PID;
	      }
	  } else {
	      reply = REPLY_NOT_IMPLEMENTED;
	  }
	  break;
      case PROBEPROCESS:
          PRTRACE1("LADBX Command: Probe Process.\n");
	  specific_dataP[0] = kpoll();
	  break;
      case DISCONNECTFROMPROCESS:
          PRTRACE1("LADBX Command: Disconnect from Process.\n");
	  if(kdetach_possible()) {
	      kdetach();
	      disconnect_requested = TRUE;
	  }
	  else {
	      reply = REPLY_CANT_DISCONNECT;
	  }
	  break;
      case KILLPROCESS:
          PRTRACE1("LADBX Command: Kill Process.\n");
	  if(kkill_possible()) {
	      kkill();
	      disconnect_requested = TRUE;
	  } else {
	      reply = REPLY_CANT_KILL;
	  }
	  break;
      case CONTINUEPROCESS:
          PRTRACE1("LADBX Command: Continue Process.\n");
	  kgo();
	  break;
      case STOPPROCESS:
          PRTRACE1("LADBX Command: Stop Process.\n");
	  kstop();
	  break;
      case STEP:
          PRTRACE1("LADBX Command: Step.\n");
#ifdef TRACE_REGISTERS
          {
              int i;
              PRTRACE1("Register State before calling kstep():\n");
              for(i=0;i<64;i++) {
                  if (i % 2 == 0) PRTRACE1("\n");
                  if (i <= 31)
                    PRTRACE3("R%02d = %016X   ",i, kregister(i));
                  else
                    PRTRACE3("F%02d = %016X   ",i-32, kregister(i));
              }
              PRTRACE3("\nPC = %08X\n",i,kpc());
          }
#endif
	  kstep();
	  break;
      case SETBREAKPOINT:
          PRTRACE1("LADBX Command: Set BreakPoint.\n");
	  reply = kbreak( ReadLocalLong(specific_dataP+BPT_DATA_ADDRESS_OFFSET));
	  break;
      case CLEARBREAKPOINT:
          PRTRACE1("LADBX Command: Clear BreakPoint.\n");
	  reply = kremovebreak( ReadLocalLong(specific_dataP+BPT_DATA_ADDRESS_OFFSET));
	  break;
      case GETNEXTBREAKPOINT:
	  {
	      address_value request_addr = ReadLocalLong(specific_dataP+BPT_DATA_ADDRESS_OFFSET) ;
	      address_value returned_addr;
	      instruction_value savedinst;
	      int bpt_count = 0;
	      int bptres = SUCCESS;
              PRTRACE1("LADBX Command: Get Next BreakPoint.\n");
	      if(request_addr != 0) {
		  for(;;) {
		      bptres = bptgetn(bpt_count++,&returned_addr,&savedinst);
		      if((bptres == SUCCESS && returned_addr == request_addr) ||
			 bptres == BPTINV) 
			break;
		  }
	      }
	      if(bptres == SUCCESS) {
		  for(;;) {
		      bptres = bptgetn(bpt_count++,&returned_addr,&savedinst);
		      if(bptres == SUCCESS || bptres == BPTINV) 
			break;
		  }
	      }
	      if(bptres != SUCCESS)
		returned_addr = 0;
	      WriteLocalLong((specific_dataP+BPT_DATA_ADDRESS_OFFSET), returned_addr);
	  }
	  break;
      case GETREGISTERS:
	  {
	      int i;
              PRTRACE1("LADBX Command: Get Registers.\n");
	      for(i=0;i<64;i++) {
		  WriteLocalLong(specific_dataP+i * sizeof(ul),kregister(i));
#ifdef TRACE_REGISTERS
                  if (i % 2 == 0) PRTRACE1("\n");
                  if (i <= 31)
                    PRTRACE3("R%02d = %016X   ",i,ReadLocalLong(specific_dataP+i * sizeof(ul) ));
                  else
                    PRTRACE3("F%02d = %016X   ",i-32,ReadLocalLong(specific_dataP+i * sizeof(ul) ));
#endif
	      }
	      /* Fill in the PC as 65'th register */
	      WriteLocalLong(specific_dataP+64*sizeof(ul),(ul)kpc());
#ifdef TRACE_REGISTERS
              PRTRACE3("\nPC = %08X\n",i,ReadLocalLong(specific_dataP+64*sizeof(ul)));
#endif
	  }
	  break;
      case SETREGISTERS:
	  {
	      int i;
              PRTRACE1("LADBX Command: Set Registers.\n");
	      for(i=0;i<64;i++) {
		  ksetreg(i,ReadLocalLong(specific_dataP+i*sizeof(ul)));

#ifdef TRACE_REGISTERS
                  if (i % 2 == 0) PRTRACE1("\n");
                  if (i <= 31)
                    PRTRACE3("R%02d = %016X   ",i,ReadLocalLong(specific_dataP+i * sizeof(ul) ));
                  else
                    PRTRACE3("F%02d = %016X   ",i-32,ReadLocalLong(specific_dataP+i * sizeof(ul) ));
#endif
	      }
	      /* The PC will have been received as the 65'th register */
	      ksetpc((ul)ReadLocalLong(specific_dataP+64*sizeof(ul)));
#ifdef TRACE_REGISTERS
              PRTRACE3("\nPC = %08X\n",i,ReadLocalLong(specific_dataP+64*sizeof(ul)));
#endif
	  }
	  break;
      case READ:
	  {
	      const address_value start_addr = 
		  ReadLocalLong(specific_dataP + MEM_DATA_ADDRESS_OFFSET);
	      const address_value final_addr = 
		  start_addr + ReadNetInt(specific_dataP + MEM_DATA_COUNT_OFFSET);
	      unsigned char * current_position = specific_dataP+MEM_DATA_CONTENT_OFFSET; 
	      ul dataread;
	      unsigned int i;
	      /* allign the address */
	      address_value addr = ~((~start_addr) | 0x7);
              PRTRACE1("LADBX Command: Read.\n");
	      PRTRACE3("Reading data; start addr %X, final addr %X,", start_addr, final_addr);
	      PRTRACE2("Aligned start addr %X\n", addr);

	      while(addr < final_addr) {
		  PRTRACE2("Getting data for address %X\n",addr);
		  if(!kaddressok( addr)) {
		      reply = REPLY_BAD_ADDRESS;
		      PRINTERR2("Could not read data at %p\n",addr);
		      break;
		  }
		  dataread = kcexamine( addr);
		  for(i=0; i<sizeof(dataread); i++) {
		      if(addr >= start_addr  && addr < final_addr) {
			  *current_position++ = ((unsigned char *)&dataread)[i];
		      }
		      addr = addr + 1;
		  }
	      }
	  }
	  break;
      case WRITE:
	  {
	      const address_value start_addr = 
		  ReadLocalLong(specific_dataP + MEM_DATA_ADDRESS_OFFSET);
	      const address_value final_addr = 
		  start_addr + ReadNetInt(specific_dataP + MEM_DATA_COUNT_OFFSET);
	      unsigned char * current_position = specific_dataP+MEM_DATA_CONTENT_OFFSET; 
	      ul dataread;
	      unsigned int i;
	      /* align the address */
	      address_value addr = ~((~start_addr) | 0x7); 
              PRTRACE1("LADBX Command: Write.\n");
	      while(addr < final_addr) {
		  address_value read_addr = addr;
		  if(!kaddressok(read_addr)) {
		      reply = REPLY_BAD_ADDRESS;
		      PRINTERR2("Could not read data at %p\n",addr);
		      break;
		  }
		  dataread = kcexamine(read_addr);
		  for(i=0; i<sizeof(dataread); i++) {
		      if(addr >= start_addr  && addr < final_addr) {
			  ((unsigned char *)&dataread)[i] = *current_position++;
		      }
		      addr = addr + 1;
		  }
		  if(!kcdeposit(read_addr, dataread)) {
		      /* The address wasn't writable */
		      reply = REPLY_BAD_ADDRESS;
		      PRINTERR2("Could not write data at %p\n",addr);
		  }
	      }
	  }
	  break;
      default:
          PRTRACE1("LADBX Command: Unknown command.\n");
	  reply = REPLY_NOT_IMPLEMENTED;
	  break;
      }
      
  }
  /* Set result */
  WriteNetInt((packetP+PACKET_RETURN_VALUE_OFFSET),reply);
  WriteNetShort(packetP+PACKET_COMMAND_OFFSET, (uw)(command_code | REPLY_MASK));

  PRTRACE1("------------------- Reply Created ----------\n");
#ifdef DUMP_PACKETS
  DumpPacket(packetP);
#endif
  last_sequence_number = ReadNetInt(packetP+PACKET_SEQNUM_OFFSET);
  saved_packet = *packet_structP;
  first_packet_seen = TRUE;

  if (disconnect_requested == TRUE) {
    disconnect_client();
    swpipl(kdebug_save_ipl);		/* Restore interrupt state */
  }
}

#ifdef DUMP_PACKETS
static void DumpPacket(ub *packetP)
{
    unsigned short command;
    int argc;
    char * argP;
    int i;
    ub * specific_dataP = packetP+PACKET_SPECIFIC_DATA_OFFSET;

    printf("Protocol version       = %X\n", packetP[PACKET_PROTOCOL_VERSION_OFFSET]);
    printf("Retransmission count   = %X\n", packetP[PACKET_RETRANS_COUNT_OFFSET]);
    
    command = ReadNetShort(packetP+PACKET_COMMAND_OFFSET);
    
    printf("Command                = %4X (%s)\n", command, CommandToString((uw)(command & (~REPLY_MASK))));

    printf("Sequence number        = %X\n", ReadNetInt(packetP+PACKET_SEQNUM_OFFSET));
    printf("Process identification = %X\n", ReadLocalInt(packetP+PACKET_PROCESS_ID_OFFSET));
    printf("Return value           = %X (%s)\n",  ReadNetInt(packetP+PACKET_RETURN_VALUE_OFFSET),
           ReplyToString(ReadNetInt(packetP+PACKET_RETURN_VALUE_OFFSET)));
    switch(command & (~REPLY_MASK)) {
	
    case CONNECTTOPROCESS:
    case CONNECTTOPROCESSINSIST:
	if((command & REPLY_MASK) == 0 ) {
	    argP = (char *) specific_dataP;
	    printf("Client user name        = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Server user name        = %s\n", argP);
	} else {
	    printf("Processor type          = %d\n", specific_dataP[PROCESS_INFO_PROCESSOR_OFFSET]);
	    printf("Process type            = %d\n", specific_dataP[PROCESS_INFO_TYPE_OFFSET]);
	}
	break;

    case LOADPROCESS:
	if((command & REPLY_MASK) == 0 ) {
	    argP = (char *) specific_dataP;
	    printf("Client user name        = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Server user name        = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Process name            = %s\n", argP);
	    argP += strlen(argP) + 1;
	    argc = *argP;
	    printf("Argument count          = %d\n", argc);
	    argP ++;
	    for(i=1; i < argc; i++) {
		printf("Argument %d = %s \n", i, argP);
		argP += strlen(argP) + 1;
	    }
	    printf("Standard input          = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Standard output         = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Standard error          = %s\n", argP);
	    argP += strlen(argP) + 1;
	    printf("Load address            = %lX\n", ReadLocalLong((unsigned char *) argP));
	    argP += MEM_DATA_ADDRESS_SIZE;
	    printf("Start address            = %lX\n", ReadLocalLong((unsigned char *) argP));
	    argP += MEM_DATA_ADDRESS_SIZE;
	} else {
	    printf("Processor type          = %d\n", specific_dataP[PROCESS_INFO_PROCESSOR_OFFSET]);
	    printf("Process type          = %d\n", specific_dataP[PROCESS_INFO_TYPE_OFFSET]);
	}
	break;

    case SETBREAKPOINT:
    case CLEARBREAKPOINT:
    case GETNEXTBREAKPOINT:
	printf("Breakpoint address       = %lX\n", ReadLocalLong(specific_dataP+BPT_DATA_ADDRESS_OFFSET));
	printf("Complete packet:\n");
	printf("\n");
	break;
    case READ:
    case WRITE:
	printf("Address                  = %lX\n", ReadLocalLong(specific_dataP + MEM_DATA_ADDRESS_OFFSET));
	printf("Length                   = %X\n",ReadNetInt(specific_dataP + MEM_DATA_COUNT_OFFSET));
	break;
    case PROBEPROCESS:
	printf("State                    = %d\n",specific_dataP[0]);
	break;
  }
}

static void DumpRawPacket(ub *packetP)
{
  int i;
  for (i = 0; i < 100; i++ ) {
    printf("%02x", ((unsigned char *)packetP)[i]);
  }
  printf("\n");
}


static char *CommandToString(unsigned short command)
{
  switch (command) {
  case LOADPROCESS:
    return "LOADPROCESS";
  case CONNECTTOPROCESS:
    return "CONNECTTOPROCESS";
  case CONNECTTOPROCESSINSIST:
    return "CONNECTTOPROCESSINSIST";
  case PROBEPROCESS:
    return "PROBEPROCESS";
  case DISCONNECTFROMPROCESS:
    return "DISCONNECTFROMPROCESS";
  case KILLPROCESS:
    return "KILLPROCESS";
  case CONTINUEPROCESS:
    return "CONTINUEPROCESS";
  case STOPPROCESS:
    return "STOPPROCESS";
  case STEP:
    return "STEP";
  case SETBREAKPOINT:
    return "SETBREAKPOINT";
  case CLEARBREAKPOINT:
    return "CLEARBREAKPOINT";
  case GETNEXTBREAKPOINT:
    return "GETNEXTBREAKPOINT";
  case GETREGISTERS:
    return "GETREGISTERS";
  case SETREGISTERS:
    return "SETREGISTERS";
  case READ:
    return "READ";
  case WRITE:
    return "WRITE";
  default:
    return "*UNKNOWN*";
  }
}

static char *ReplyToString(unsigned int reply)
{
  switch (reply) {
  case REPLY_OK:
    return "REPLY_OK";
  case REPLY_BAD_PID:
    return "REPLY_BAD_PID";
  case REPLY_NO_RESOURCES:
    return "REPLY_NO_RESOURCES";
  case REPLY_NOT_CONNECTED:
    return "REPLY_NOT_CONNECTED";
  case REPLY_NOT_STOPPED:
    return "REPLY_NOT_STOPPED";
  case REPLY_BAD_ADDRESS:
    return "REPLY_BAD_ADDRESS";
  case REPLY_NOT_IMPLEMENTED:
    return "REPLY_NOT_IMPLEMENTED";
  case REPLY_BAD_LOAD_NAME:
    return "REPLY_BAD_LOAD_NAME";
  case REPLY_ALREADY_CONNECTED:
    return "REPLY_ALREADY_CONNECTED";
  case REPLY_CANT_DISCONNECT:
    return "REPLY_CANT_DISCONNECT";
  case REPLY_CANT_KILL:
    return "REPLY_CANT_KILL";
  default:
    return "*UNKNOWN*";
  }
}

#endif
