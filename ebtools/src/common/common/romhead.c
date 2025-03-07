
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
__attribute__((unused)) static const char *rcsid = "$Id: romhead.c,v 1.2 1999/04/14 20:51:26 paradis Exp $";
#endif

/*
 * $Log: romhead.c,v $
 * Revision 1.2  1999/04/14 20:51:26  paradis
 * Added type 11 for server management console
 *
 * Revision 1.1.1.1  1998/12/29 21:36:20  paradis
 * Initial CVS checkin
 *
 * Revision 1.31  1997/05/30  03:45:06  fdh
 * Added SROM and VxWorks firmware IDs.
 *
 * Revision 1.30  1996/05/22  15:54:02  fdh
 * Added code to handle headers with experimental signature.
 *
 * Revision 1.29  1996/04/29  15:03:43  fdh
 * Added Fail-Safe Booter definitions.
 *
 * Revision 1.28  1996/02/14  17:33:03  cruz
 * Increase size of strbuf1 and strbuf2 to prevent overrun of
 * array boundaries.
 * Modified IsValidHeader() to check header version before
 * verifying header checksum.
 *
 * Revision 1.27  1996/02/07  18:55:52  cruz
 * Added IsHeaderValid().
 *
 * Revision 1.26  1995/12/12  15:16:56  cruz
 * Printed header size in hex.
 *
 * Revision 1.25  1995/11/22  15:19:07  cruz
 * Print image sizes in hex, as well as, decimal.
 *
 * Revision 1.24  1995/11/16  22:06:02  cruz
 * Added routine HasRomSignature().
 *
 * Revision 1.23  1995/11/15  23:51:13  fdh
 * Check aliases in match routines before giving up searches.
 *
 * Revision 1.22  1995/11/15  22:15:56  fdh
 * Modified dumpHeader() to only print the ROM offset
 * field when that field is valid.
 *
 * Revision 1.21  1995/11/13  18:39:04  cruz
 * Switched print order of ROM offset and ROM offset valid flag.
 *
 * Revision 1.20  1995/11/13  16:57:26  cruz
 * Added code to print the rom offset if header version 2 or greater.
 *
 * Revision 1.19  1995/10/30  16:56:39  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.18  1995/10/26  23:40:58  cruz
 * Added casting
 *
 * Revision 1.17  1995/10/04  01:57:36  fdh
 * Modified to use the ROM_HEADER_CHECKSUM() macro to
 * access the header checksum.
 *
 * Revision 1.16  1995/09/19  20:34:36  fdh
 * Added an element to the fw_id_t structure to hold a default
 * ROM OFFSET value for the registered firmware types.
 *
 * Revision 1.15  1995/08/25  19:47:20  fdh
 * Modified "Memory Image Size" message.
 *
 * Revision 1.14  1995/08/07  17:45:00  fdh
 * Added support for the Linux Miniloader.
 *
 * Revision 1.13  1995/02/27  19:22:33  fdh
 * Print header->romh.V0.destination as two longwords instead of
 * a single quadword.
 *
 * Revision 1.12  1995/02/27  15:35:26  fdh
 * Modified to not use 64 bit ints for portability.
 *
 * Revision 1.11  1995/02/25  05:18:13  fdh
 * Print 64bit fields as type long.
 *
 * Revision 1.10  1995/02/19  17:47:31  fdh
 * Modified an error message.
 *
 * Revision 1.9  1995/02/16  20:46:31  fdh
 * Print out decimal representation of image checksum too.
 *
 * Revision 1.8  1995/02/10  19:08:55  fdh
 * Minor fixup.
 *
 * Revision 1.7  1995/02/10  02:18:11  fdh
 * Moved compute_romh_chksum() to this file.
 *
 * Revision 1.5  1995/02/07  04:57:30  fdh
 * Modified fwid_match_i().
 * Added ostype_array[] and routines for accessing it.
 *
 * Revision 1.4  1995/02/07  01:04:05  fdh
 * Removed some unnecessary definitions.
 *
 * Revision 1.3  1995/02/06  02:44:42  fdh
 * Added fwid_match(), fwid_match_i(), and fwid_dump() routines.
 *
 * Revision 1.2  1995/02/05  01:59:05  fdh
 * Added include files and definitions.
 *
 * Revision 1.1  1995/02/03  16:35:37  fdh
 * Initial revision
 *
 */

#include "romhead.h"  /* includes lib.h */

const char *fw_dbm_id[] = {FW_DBM_STRINGS};
const char *fw_wnt_id[] = {FW_WNT_STRINGS};
const char *fw_srm_id[] = {FW_SRM_STRINGS};
const char *fw_fsb_id[] = {FW_FSB_STRINGS};
const char *fw_lnx_id[] = {FW_LNX_STRINGS};
const char *fw_vxw_id[] = {FW_VXW_STRINGS};
const char *fw__sr_id[] = {FW__SR_STRINGS};
const char *fw_smc_id[] = {FW_SMC_STRINGS};

const fw_id_t fwid_array[] = {
  { FW_DBM,  fw_dbm_id, FW_DBM_OFFSET},
  { FW_WNT,  fw_wnt_id, FW_WNT_OFFSET},
  { FW_SRM,  fw_srm_id, FW_SRM_OFFSET},
  { FW_FSB,  fw_fsb_id, FW_FSB_OFFSET},
  { FW_LNX,  fw_lnx_id, FW_LNX_OFFSET},
  { FW_VXW,  fw_vxw_id, FW_VXW_OFFSET},
  { FW__SR,  fw__sr_id, FW__SR_OFFSET},
  { FW_SMC,  fw_smc_id, FW_SMC_OFFSET},
  { 0, NULL, 0}
};

const char *ostype_dbm[] = {OS_DBM_STRINGS};
const char *ostype_wnt[] = {OS_WNT_STRINGS};
const char *ostype_vms[] = {OS_VMS_STRINGS};
const char *ostype_osf[] = {OS_OSF_STRINGS};
const char *ostype_fsb[] = {OS_FSB_STRINGS};
const char *ostype_lnx[] = {OS_LNX_STRINGS};
const char *ostype_vxw[] = {OS_VXW_STRINGS};
const char *ostype__sr[] = {OS__SR_STRINGS};
const char *ostype_smc[] = {OS_SMC_STRINGS};

const ostype_t ostype_array[] = {
  { FW_OSTYPE_DBM, FW_DBM,  ostype_dbm},
  { FW_OSTYPE_WNT, FW_WNT,  ostype_wnt},
  { FW_OSTYPE_VMS, FW_SRM,  ostype_vms},
  { FW_OSTYPE_OSF, FW_SRM,  ostype_osf},
  { FW_OSTYPE_FSB, FW_FSB,  ostype_fsb},
  { FW_OSTYPE_LNX, FW_LNX,  ostype_lnx},
  { FW_OSTYPE_VXW, FW_VXW,  ostype_vxw},
  { FW_OSTYPE__SR, FW__SR,  ostype__sr},
  { FW_OSTYPE_SMC, FW_SMC,  ostype_smc},
  { 0, 0, NULL}
};

#define MAXSTRING 80
char strbuf1[MAXSTRING+2];
char strbuf2[MAXSTRING+2];

const fw_id_t *fwid_match_i(int fwid)
{
  const fw_id_t *ptr;
  const ostype_t *ostype_ptr;

  ptr = fwid_array;
  while (ptr->id_string) {
    if (fwid == ptr->firmware_id)
      return (ptr);

    ++ptr;
  }

/*
 * Now check the aliases in ostype_array[]
 * before giving up.
 */
  if ((ostype_ptr = ostype_match_i(fwid)) != NULL)
    return(fwid_match_i(ostype_ptr->firmware_id));

  return ((fw_id_t *)NULL);
}

const fw_id_t *fwid_match(const char *const arg)
{
  const fw_id_t *ptr;
  int i, k;

  ptr = fwid_array;
  strncpy(strbuf1, arg, MAXSTRING);
  strbuf1[MAXSTRING+1] = '\0';
  k = 0;
  while ((strbuf1[k] = tolower((int)strbuf1[k])) != '\0') ++k;

  while (ptr->id_string) {
    i = 0;
    while (ptr->id_string[i]) {
      strncpy(strbuf2, ptr->id_string[i], MAXSTRING);
      strbuf2[MAXSTRING+1] = '\0';
      k = 0;
      while ((strbuf2[k] = tolower((int)strbuf2[k])) != '\0') ++k;

      if (strcmp(strbuf1, strbuf2) == 0)
	return (ptr);

      ++i;
    }
    ++ptr;
  }
  return ((fw_id_t *)NULL);
}

void fwid_dump(const char *const pre)
{
  const fw_id_t *ptr;
  int i;

  ptr = fwid_array;
  while (ptr->id_string) {
    i = 0;
    printf("%s\"%d\"", pre, ptr->firmware_id);
    while (ptr->id_string[i])
      printf(" \"%s\"", ptr->id_string[i++]);
    printf("\n");
    ++ptr;
  }
}

const ostype_t *ostype_match(const char *const arg)
{
  const ostype_t *ostype_ptr;
  const fw_id_t *fwid_ptr;
  int i, k;

  ostype_ptr = ostype_array;
  strncpy(strbuf1, arg, MAXSTRING);
  strbuf1[MAXSTRING+1] = '\0';
  k = 0;
  while ((strbuf1[k] = tolower((int)strbuf1[k])) != '\0') ++k;

  while (ostype_ptr->id_string) {
    for (i=0; i<2; ++i) {
      strncpy(strbuf2, ostype_ptr->id_string[i], MAXSTRING);
      strbuf2[MAXSTRING+1] = '\0';
      k = 0;
      while ((strbuf2[k] = tolower((int)strbuf2[k])) != '\0') ++k;

      if (strcmp(strbuf1, strbuf2) == 0)
	return (ostype_ptr);
    }
    ++ostype_ptr;
  }

/*
 * Now check the aliases in fwid_array[]
 * before giving up.
 */
  if ((fwid_ptr = fwid_match(arg)) != NULL)
    return(ostype_match_i(fwid_ptr->firmware_id));

  return ((const ostype_t *)NULL);
}

const ostype_t *ostype_match_i(int ostype)
{
  const ostype_t *ptr;

  ptr = ostype_array;
  while (ptr->id_string) {
    if (ostype == ptr->ostype)
      return (ptr);
    ++ptr;
  }
  return ((const ostype_t *)NULL);
}

void ostype_dump(const char *const pre)
{
  const ostype_t *ptr;
  int i;

  ptr = ostype_array;
  while (ptr->id_string) {
    printf("%s\"%d\"", pre, ptr->ostype);
    for (i=0; i<2; ++i)
      printf(" \"%s\"", ptr->id_string[i]);
    printf("\n");
    ++ptr;
  }
}

/*
 * Compute the header checksum.
 */
ui compute_romh_chksum(const romheader_t *const header)
{
  const char *ptr;
  ui chksum = 0;

  ptr = (char *) header;
  while (ptr < (char *) &ROM_HEADER_CHECKSUM(header)) {
    COMPUTE_CHECKSUM(*ptr,chksum);
    ++ptr;
  }
  return (chksum);
}

/* Prints out the ROM header pointed to by argument. 
 * Returns TRUE if header is valid; FALSE otherwise.
 */
int dumpHeader(const romheader_t *const header)
{

  int i, hver;
  ui hchksum;
  const fw_id_t *fwid_ptr;
   
  printf("  Header Size......... 0x%X (%d) bytes\n", header->romh.V0.hsize, 
         header->romh.V0.hsize);
  printf("  Image Checksum...... 0x%04x (%d)\n",
	 header->romh.V0.checksum, header->romh.V0.checksum);
  printf("  Memory Image Size... 0x%X (%d = %d KB)\n", header->romh.V0.size,
         header->romh.V0.size, header->romh.V0.size/1024);
  printf("  Compression Type.... %d\n", header->romh.V0.decomp);
  printf("  Image Destination... 0x%08x%08x\n",
	 header->romh.V0.destination.high,
	 header->romh.V0.destination.low);

  hver = ROMH_VERSION(header);
  if (hver > 0)			/* Version > 0 */
     {	
	printf("  Header Version...... %d%s\n", (ui) hver,
	       ((header->romh.V0.signature & ~ROM_H_BRANCH_OFFSET_MASK)
		== ROM_H_BRANCH_SIGNATURE) ? "A" : "");

	fwid_ptr = fwid_match_i((int)header->romh.V1.fw_id);
	printf("  Firmware ID......... %d - %s\n",
	       (ui) header->romh.V1.fw_id,
	       fwid_ptr == NULL ? "Unknown ID Type." : fwid_ptr->id_string[0]);

	printf("  ROM Image Size...... 0x%X (%d = %d KB)\n", header->romh.V1.rimage_size, 
               header->romh.V1.rimage_size, 
	       header->romh.V1.rimage_size/1024);

	printf("  Firmware ID (Opt.).. %08x%08x  ",
	       header->romh.V1.fwoptid.id_S.high,
	       header->romh.V1.fwoptid.id_S.low);
  
	for (i = 0; i < 8; i++)
  {
	   if (isprint((int)header->romh.V1.fwoptid.id[i]))
	     printf ("%c", header->romh.V1.fwoptid.id[i]);
	   else printf (".");
  }
	printf("\n");
        
        if (hver > 1) {         /* Version > 1 */
	  if (header->romh.V2.rom_offset != 0)
            printf("  ROM offset.......... 0x%08X\n", header->romh.V2.rom_offset & ~3);
        }
	printf("  Header Checksum..... 0x%04x", ROM_HEADER_CHECKSUM(header));
        hchksum = compute_romh_chksum(header);
       if (hchksum != ROM_HEADER_CHECKSUM(header)) {
	 printf("\nERROR: Bad ROM header checksum. 0x%04x\n", hchksum);
	 return (FALSE);
       }
       else
	  printf ("\n");	
  }      
  return (TRUE);  
}

/* 
 * Returns TRUE if image has a ROM signature.
 */
int HasRomSignature (const romheader_t *const RomImageHeader)
{
  if (RomImageHeader->romh.V0.csignature != (ui) ~ROM_H_SIGNATURE)
  return(FALSE);

  if (RomImageHeader->romh.V0.signature == ROM_H_SIGNATURE)
    return(TRUE);

  if ((RomImageHeader->romh.V0.signature & ~ROM_H_BRANCH_OFFSET_MASK)
      == ROM_H_BRANCH_SIGNATURE)
    return(TRUE);

  return(FALSE);
}

/* 
 * Returns TRUE if header is valid (has signature and checksum is correct).
 */
int IsHeaderValid (const romheader_t *const RomImageHeader)
{
    return ((HasRomSignature(RomImageHeader) && (ROMH_VERSION(RomImageHeader) == 0)) ||
            (HasRomSignature(RomImageHeader) && (compute_romh_chksum(RomImageHeader) == ROM_HEADER_CHECKSUM(RomImageHeader))));
}
