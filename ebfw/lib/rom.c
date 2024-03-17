
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
static char *rcsid = "$Id: rom.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: rom.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.44  1997/05/01  20:56:03  pbell
 * Added support for switch DISABLE_FLASH_ADDRESS19.
 *
 * Revision 1.43  1996/08/20  17:53:07  fdh
 * Added compile time conditionals around unreferenced local variables.
 *
 * Revision 1.42  1996/05/25  20:33:58  fdh
 * Added the DISABLE_BOOTOPTION conditional.
 *
 * Revision 1.41  1996/05/22  20:55:06  fdh
 * Added code to handle headers with the experimental signature.
 *
 * Revision 1.40  1996/04/30  19:07:50  cruz
 * Minor changes to make lint happy.
 *
 * Revision 1.39  1996/04/06  03:33:09  fdh
 * Bracket sparse space accesses to the ROM with setting
 * the Host Address Extension Register (HAXR).  Also disable
 * interrupts while the HAXR is set.
 *
 * Revision 1.38  1996/02/27  16:09:05  fdh
 * Conditionalized code to erase bad images for
 * Flash based boards only.
 *
 * Revision 1.37  1996/02/15  11:33:48  cruz
 * Disabled checks of image checksums for V0 headers since they
 * do not contain valid checksums even though the field is defined.
 *
 * Revision 1.36  1996/02/14  17:32:21  cruz
 * Modified read_rom() to allow comparisons of images in memory
 * to images in ROM.
 *
 * Revision 1.35  1996/02/07  18:53:17  cruz
 * Added a pause in list of rom headers.
 * Changed list_rom_headers() to check the integrity of
 * images (using the image checksum from the header).  If
 * a corrupted image is found, it's noted and can be
 * erased if the user so desires.
 *
 * Revision 1.34  1995/12/07  21:14:03  cruz
 * Fixed print statement in load_rom() to show the correct
 * ending address for the load (had to subtract 1).
 *
 * Revision 1.33  1995/10/31  22:51:54  cruz
 * Added compile conditional NEEDFLASHMEMORY around variable
 * declarations that are not used when this compiled switch is
 * not enabled.
 *
 * Revision 1.32  1995/10/26  23:42:31  cruz
 * Made local variables static, added casting and fixed up prototypes
 *
 * Revision 1.31  1995/08/07  17:42:36  fdh
 * Added provisions to read_rom() to locate unregistered
 * firmware IDs.
 *
 * Revision 1.30  1995/03/07  07:35:07  fdh
 * Fix compile warning.
 *
 * Revision 1.29  1995/03/07  04:23:19  fdh
 * Correction in computing checksum for type 1
 * compressed images.
 *
 * Revision 1.28  1995/03/06  00:20:35  fdh
 * Properly cast data types.
 *
 * Revision 1.27  1995/03/05  04:19:57  fdh
 * Modified to use inrom() for the EB64.
 *
 * Revision 1.26  1995/03/05  01:05:17  fdh
 * Removed leftover test code.
 *
 * Revision 1.25  1995/03/05  00:48:28  fdh
 * Added provisions for padding for images
 * compressed with the SROM compression algorithm.
 *
 * Revision 1.24  1995/02/27  15:49:56  fdh
 * Modified to access 64bit image destination address as
 * a struct of two longwords.
 *
 * Revision 1.23  1995/02/23  21:50:05  fdh
 * Corrected a few data types.
 *
 * Revision 1.22  1995/02/21  21:24:44  cruz
 * Fixed type in output message.
 *
 * Revision 1.21  1995/02/20  03:53:33  fdh
 * Cleaned up the logic used in read_rom().
 *
 * Revision 1.20  1995/02/19  17:49:19  fdh
 * Modified read_rom() to use an ostype argument
 * as well as the image position number.
 *
 * Revision 1.19  1995/02/18  17:37:39  fdh
 * Modified image checksum messages.
 *
 * Revision 1.18  1995/02/18  17:22:06  fdh
 * Corrected upper limit condition when loading uncompressed
 * ROM image.
 *
 * Revision 1.17  1995/02/10  02:28:47  fdh
 * Removed dumpHeader(). Moved to romhead.c.
 * Modified findHeader().
 * Replaced compute_checksum() and romh_version() with Macros.
 * Created set_romboot().
 * Moved compute_romh_checksum() to romhead.c
 *
 * Revision 1.16  1995/02/06  20:16:46  cruz
 * Updated romlist and romload to use new style headers.
 *
 * Revision 1.15  1994/12/05  19:29:18  fdh
 * Modified to properly terminate decompression when
 * EOF is read (run length = 0).
 *
 * Revision 1.14  1994/11/28  21:42:45  cruz
 * Corrected typo in dumpHeader().
 *
 * Revision 1.13  1994/11/28  19:38:13  fdh
 * Corrected a bug where = should have been ==
 *
 * Revision 1.12  1994/11/28  18:24:00  fdh
 * Modified to have fewer Board specific conditionals.
 *
 * Revision 1.11  1994/11/19  03:29:42  fdh
 * Modified to use common rom header definitions.
 * Added support for romlist command.
 *
 * Revision 1.10  1994/11/10  19:39:45  fdh
 * Modified to read from flalsh memory that
 * has the address bit 19 register setting.
 *
 * Revision 1.9  1994/11/08  21:25:27  fdh
 * Moved general definitions to system.h
 *
 * Revision 1.8  1994/11/07  12:05:39  rusling
 * Now include system.h as well as lib.h
 *
 * Revision 1.7  1994/08/05  20:16:23  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.6  1994/06/21  14:21:55  rusling
 * fixed up WNT compile warnings.
 *
 * Revision 1.5  1994/06/17  19:35:37  fdh
 * Clean-up...
 *
 * Revision 1.4  1994/04/01  22:43:42  fdh
 * Modified data type for DisStartAddress.
 * Set new value for bootadr.
 *
 * Revision 1.3  1993/11/04  23:42:04  fdh
 * Fix access to Host Address Extension Register for EB64+
 *
 * Revision 1.2  1993/08/25  21:36:13  fdh
 * Removed ignore_header argument.  This is implied by the
 * image argument when equal to 0.
 *
 * Revision 1.1  1993/08/25  01:52:57  fdh
 * Initial revision
 *
 */

#include "system.h"
#include "lib.h"
#include "rom.h"
#include "bbram.h"
#include "romhead.h"
#include "console.h"
extern ul bootadr;
extern ul DisStartAddress;

static ul src;
static ul des;
static ui cnt;
static ui buffer;
static romheader_t head;

static char getByte(void );
static ul findHeader(void );
static void putByte(char c);
static void flushByte(void );
static ui load_rom(ul destination , ul rommax , int decomp);
static ui getLongWord(void );
static int CompareToRom (ul source, ul rommax);

static  int load_to_mem;

#ifdef NEEDFLASHMEMORY
static int high_region;
#endif

/*
 *  Scan ROM for Special ROM header...
 */

int read_rom(int argcount, char *arg, ul address, int compare)
{
  ostype_t *ostype_ptr;
  fw_id_t *fwid_ptr;
  int ostype, image;
  int loaded, Matches;
  ul destination;
  ul found_header;
  ui checksum;
  romheader_t *header;

  loaded = FALSE;
  found_header= FALSE;
  src = ROMBASE;
  buffer = 0;
  

#if defined(NEEDFLASHMEMORY) && !defined(DIASABLE_FLASH_ADDRESS19)
  /*
   * Init the Flash address bit 19
   * and flag.
   */
  high_region = FALSE;
  outportb(0x800, 0x00);
#endif


  ostype = 0x81;      /* Default case.  Just load first image */
  ostype_ptr = NULL;
  destination = address;
  header = (romheader_t *)destination;

  if ((*arg == '\0') && compare) {  /* Default case for compare:  Use type from source. */
      if (IsHeaderValid(header) && (ROMH_VERSION(header) > 0)) {
          ostype = header->romh.V1.fw_id;
          ostype_ptr = ostype_match_i(ostype);
      }
  }

  if (*arg != '\0') {
      if ((ostype_ptr = ostype_match(arg)) == NULL) {
          if (*arg == '#') 
            ostype = atoi(arg+1)+128;
          else if (isdigit((int) *arg))
            ostype = atoi(arg);
          ostype_ptr = ostype_match_i(ostype);
      }
  }

  if (ostype_ptr == NULL) {
    if (ostype > 0x80) {
      image = ostype - 0x80;

      printf("Searching for ROM image #%d\n", image);
      while(image--)
	found_header = findHeader();
    }
    else if (ostype == 0x80) {
      src = ROMBASE;
      loaded = TRUE;

      if (compare) {
          printf("Comparing entire ROM to image at 0x%X.\n", destination);
          Matches = CompareToRom(destination, ((ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC)));
      }
      else {
          printf("\nLoading entire ROM.\n");
          checksum = load_rom(destination, ((ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC)), 0);
      }
    }
    else {
      printf("\nUnknown O/S type specified: %d\n", ostype);
      printf("Searching for Firmware ID %d\n", ostype);
      while((found_header = findHeader()) != 0) {
	if (ROMH_VERSION((&head)) == 0) continue;
	if (head.romh.V1.fw_id == ostype)
	  break;
      }
    }
  }
  else {
    fwid_ptr = fwid_match_i(ostype_ptr->firmware_id);
    printf("Searching for the \"%s\".\n",
	   fwid_ptr->id_string[0]);

    while((found_header = findHeader()) != 0) {
      if (ROMH_VERSION((&head)) == 0) continue;
      if (head.romh.V1.fw_id == ostype_ptr->firmware_id)
	break;
    }
  }

  if (found_header && !compare)
    {
      dumpHeader(&head);
      if (argcount>2)
	destination = address;
      else {
	destination = (ul) (head.romh.V0.destination.high << 32)
	  | (ul) head.romh.V0.destination.low;
      }
      checksum = load_rom(destination, (src+head.romh.V0.size*ROMINC), head.romh.V0.decomp);

      if (ROMH_VERSION(&head) != 0) { /* Check checksums only if > V0 */
          if (checksum != head.romh.V0.checksum)
            printf("ERROR: Bad image checksum. Computed sum = 0x%04x\n", checksum);
          else
            printf("Image checksum verified. Computed sum = 0x%04x\n", checksum);
      }
  }

  if (found_header && compare) {
      printf("Comparing to image at 0x%X.\n", destination);
      src -= (ROMINC * head.romh.V0.hsize);  /* Point to start of header */
      Matches = CompareToRom(destination, (src + (header->romh.V0.size + header->romh.V0.hsize) * ROMINC));
  }

#if defined(NEEDFLASHMEMORY) && !defined(DIASABLE_FLASH_ADDRESS19)
  /*
   * Init the Flash address bit 19
   * and flag.
   */
  high_region = FALSE;
  outportb(0x800, 0x00);
#endif


  if (!compare && (found_header || loaded)) {
    return(TRUE);
  }
  if (compare && (found_header || loaded)) {
      if (Matches)
        printf("Images match.\n\n");
      else
        printf("Images do not match.\n\n");
      return Matches;
  }

  printf("The specified ROM image was not found.\n\n");
  return(FALSE);
}

void list_rom_headers(void)
{
  int HeaderCount;
  int GoodHeader, GoodImage;
  ui ImageCheckSum;
  ul HeaderOffset, EndOffset, SizeToErase;
  ul old_src;

#ifdef NEEDFLASHMEMORY
  int c;
  ul NewHeaderOffset;
#endif

  src = ROMBASE;
  buffer = 0;
  
#if defined(NEEDFLASHMEMORY) && !defined(DIASABLE_FLASH_ADDRESS19)
/*
 * Init the Flash address bit 19
 * and flag.
 */
  high_region = FALSE;
  outportb(0x800, 0x00);
#endif

  HeaderCount = 0;
  while (findHeader()) {

    if ((++HeaderCount % 3) == 0)
      if (!kbdcontinue()) break;

    HeaderOffset = src - (ul)ROMBASE - head.romh.V0.hsize*(ul)ROMINC;
    printf("\nROM image header found at offset: 0x%06x\n", HeaderOffset);
    GoodHeader = dumpHeader(&head);

    old_src = src;      /* Save where this image begins */
    ImageCheckSum = load_rom((ul)-1, (src+head.romh.V0.size*ROMINC), head.romh.V0.decomp);
    EndOffset = src - (ul)ROMBASE;   /* Compute ending offset */
    SizeToErase = EndOffset - HeaderOffset;  /* Size of this image */
    src = old_src;      /* Restore pointer so we can continue to search */

    GoodImage =  (ImageCheckSum == head.romh.V0.checksum)
      || (ROMH_VERSION(&head) == 0); /* Check checksums only if > V0 */;
    if (!GoodImage)
      printf("ERROR: Bad image checksum. 0x%04x\n", ImageCheckSum);

    if ((!GoodImage || !GoodHeader) && (SizeToErase != 0)) {
#ifdef NEEDFLASHMEMORY
        printf("This image is corrupt.  Would you like to remove it (Y/N)?");
        c = GetChar ();
        printf("%c\n", c);
        if ( (c == 'y') || (c == 'Y')) {
            /* 
             *  If another signature is found, then check to  see if it's part of a valid header. 
             *  If it is, then let's only erase up to this header.
             */
            while (findHeader() && IsHeaderValid(&head)) {
                NewHeaderOffset = src - (ul)ROMBASE - head.romh.V0.hsize*(ul)ROMINC;
                if (NewHeaderOffset <= EndOffset) {
                    SizeToErase =  NewHeaderOffset - HeaderOffset;
                    break;
                }
            }
            flash_main((ui)-1, (int)HeaderOffset, (ui)SizeToErase); 
        }
#endif
      }

    src = old_src;
  }

  if (HeaderCount==0)
    printf("No ROM Image headers were found\n\n");

#if defined(NEEDFLASHMEMORY) && !defined(DIASABLE_FLASH_ADDRESS19)
/*
 * Re-init the Flash address bit 19
 * and flag.
 */
  high_region = FALSE;
  outportb(0x800, 0x00);
#endif
}

static ul findHeader(void)
{
  ui i;

  while (src < ((ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC))) {

    /* Test for validation signature */
    head.romh.V0.signature = getLongWord();
    if ((head.romh.V0.signature == ROM_H_SIGNATURE)
	|| ((head.romh.V0.signature & ~ROM_H_BRANCH_OFFSET_MASK)
	    == ROM_H_BRANCH_SIGNATURE)) {

      /* Test for complimented validation signature */
      if ((head.romh.V0.csignature = getLongWord()) == (ui)~ROM_H_SIGNATURE) {

	/* This should be a valid header */
	head.romh.V0.hsize = getLongWord(); 	/* Get the header size now.	*/

	/* Load the rest of the header.	*/
	for (i = 3; i < (head.romh.V0.hsize/4); i++)
	  head.romh_array[i] = getLongWord();

	/* src now points to first location following header */
	return(src);
      } 
      else src -= (ROMINC*(4)); 		/* Peel back 1 getLongWord. 	*/
    }
  }
  return(0);
}

static ui getLongWord(void)
{
  ui data;
  data = 0;
  data |= ((getByte()&0xff)<< 0);
  data |= ((getByte()&0xff)<< 8);
  data |= ((getByte()&0xff)<<16);
  data |= ((getByte()&0xff)<<24);
  return(data);
}

static char getByte(void)
{
  ui addr;
  char c;
  int save_ipl;
  ui OldValue;

#if defined(NEEDFLASHMEMORY) && !defined(NEEDPRIVATEFLASHBUS)
  if (src < ((ul)ROMBASE+(ul)((ul)(ROMSIZE/2)*(ul)ROMINC))) {
    if (high_region) {
      high_region = FALSE;
      outportb(0x800, 0x00);
    }
    addr = (ui) src;
  }
  else {
    if (!high_region) {
      high_region = TRUE;
      outportb(0x800, 0x01);
    }
    addr = (ui) (src - ((ul)(ROMSIZE/2)*(ul)ROMINC));
  }
#else
  addr = (ui) src;
#endif

  /*
   * Make sure interrupts are disabled before setting the HAE.
   */
  save_ipl = swpipl(7);

  /* 
   ** The ROM usually lives at high I/O memory addresses.
   ** In order for the IOC to generate that address the HAE 
   ** (Host Address Extension) in the IOC control register 
   ** might need to be loaded.
   */
  OldValue = SetHAE(addr);

#if defined(NEEDEB64SPECIALROMACCESS) || defined(NEEDPRIVATEFLASHBUS)
  c = (char) inrom(addr);
#else
  c = (char) inmemb(addr);
#endif

  SetHAE(OldValue);		/* Restore Host Address Extension */

  swpipl(save_ipl);		/* Restore ipl. */

  src += ROMINC;
  return(c);
}

static void putByte(char c)
{
  buffer |= ((c&0xff)<<((cnt%4)*8));
  if(((cnt+1)%4)==0) {
    if (load_to_mem)
      *(ui *)des = buffer;
    buffer = 0;
    des += 4;
  }
  ++cnt;
}

void flushByte(void)
{
  if (load_to_mem)
    *(ui *)des = buffer;
}

/* Returns checksum of image loaded. */
static ui load_rom(ul destination, ul rommax, int decomp)
{
  char c;
  int run, i;
  ui checksum;
  ui data;
  int bytes;
  cnt = 0;

  load_to_mem = destination != (ul)-1;
  des = destination;
  checksum = 0;

  if (load_to_mem) {
      DisStartAddress = destination;
      bootadr = destination;
      printf("Loading ROM to address %08x\n", des);
  }

  switch(decomp) {

  case 1:
    if (load_to_mem)
      printf("Type %d decompression\n", head.romh.V0.decomp);
    bytes = 0;
    while(src < ((ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC)))
      {
	c = getByte(); ++bytes;
	COMPUTE_CHECKSUM(c, checksum);      
	if (c & 0x80)
	  {
	    run = -((int) c);
	    c = getByte(); ++bytes;
	    COMPUTE_CHECKSUM(c, checksum);
	    while (run--) putByte(c);
	  }
	else if (c & 0x7f)
	  {
	    run = (int) c & 0x7f;
	    while (run--)
	      {
		c = getByte(); ++bytes;
		COMPUTE_CHECKSUM(c, checksum);
		putByte(c);
	      }
	  }
	else {
	  if (ROMH_VERSION(&head) > 0) {
	    while((ui)bytes < head.romh.V1.rimage_size) {
	      COMPUTE_CHECKSUM('\0', checksum);
	      ++bytes;
	    }
	  }
	  break;
	}
      }
    flushByte();
    break;

  case 0:
    while((src < ((ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC))) && (src < rommax))
      {
	data = getLongWord();
        if (load_to_mem)
          *(ui *)des = data;
	for (i = 0; i < 4; i++) {
	  COMPUTE_CHECKSUM( (char) ((data >> 8*i) & 0xFF), checksum);
	}
	des += 4;
	cnt += 4;
      };
    break;
  }

  if (load_to_mem)
    printf("\nLoaded %d bytes starting at 0x%x to 0x%x\n", cnt, destination, des-1);
  return(checksum);
}

/*
 * Compares data from ROM to data in memory.
 */
static int CompareToRom (ul source, ul rommax) 
{
    char    data, *srcptr;
    int     Matches = TRUE;
    ul      RomSize;

    srcptr = (char *)source;
    RomSize = (ul)ROMBASE+(ul)((ul)ROMSIZE*(ul)ROMINC);
    while((src < RomSize) && (src < rommax))
      {
	data = (char) getByte();
        if (data != *srcptr) {
            Matches = FALSE;
            break;
        }
        srcptr++;
    }
    if (RomSize < rommax)
      return FALSE;
    return Matches;
}


/* Computes the size the image will occupy in the ROM.
 * It includes the size of the header.
 * A standard header is expected at location specified by rhead. 
 */
ui romimage_size(romheader_t * rhead)
{
   char c;
   int i ;
   char * img_src = (char * ) rhead;
   
   if (rhead->romh.V0.decomp == 0) 	/* If the image is not compressed, */
      return(rhead->romh.V0.size + rhead->romh.V0.hsize); /* then ROM size is image size. */
   if (ROMH_VERSION(rhead)) 	       /* If new style headers, then don't */
      return(rhead->romh.V1.rimage_size + rhead->romh.V0.hsize); /* compute; it's in the header */

   i = rhead->romh.V0.hsize;
   while (TRUE)
      {
	c = img_src[i++];
	if (c & 0x80)
	   i++;
        else if (c & 0x7f)
	   i += (int) c;
        else
	   break;
	}
   /* Assume that the image is padded to a longword boundary. */
   while(i%4)
     ++i;
   return(i);
}

#ifndef DISABLE_BOOTOPTION
void set_romboot(char *arg)
{
  ostype_t *ostype_ptr;
  fw_id_t *fwid_ptr;
  int ostype;
  int readonly;

  readonly = FALSE;
  fwid_ptr = NULL;

  if (*arg == '\0')
    readonly = TRUE;

  if (readonly) {
    BBRAM_READ(BBRAM_OSTYPE, ostype );
    ostype_ptr = ostype_match_i(ostype);
  }
  else {
    if ((ostype_ptr = ostype_match(arg)) == NULL) {
      if (*arg == '#') ostype = atoi(arg+1)+128;
      else ostype = atoi(arg);
      ostype_ptr = ostype_match_i(ostype);
    }
  }

  if (ostype_ptr == NULL) {
    if (!readonly) BBRAM_WRITE(BBRAM_OSTYPE, ostype );
    if (ostype > 0x80)
      printf("\nFirmware image %d selected.\n", (ostype-0x81)+1);
    else if (ostype == 0x80)
      printf("\nLoad and boot entire ROM at address zero.\n");
    else
      printf("\nO/S type selected: %d \"Unknown\"\n", ostype);
  }
  else {
    fwid_ptr = fwid_match_i(ostype_ptr->firmware_id);
    if (!readonly) BBRAM_WRITE(BBRAM_OSTYPE, (ub) ostype_ptr->ostype);
    printf("\nO/S type selected: \"%s\"\n",
	   ostype_ptr->id_string[0]);
  }
  printf("....Firmware type: \"%s\"\n\n", 
	 fwid_ptr == NULL ? "Unknown" : fwid_ptr->id_string[0]);
}
#endif /* DISABLE_BOOTOPTION */
