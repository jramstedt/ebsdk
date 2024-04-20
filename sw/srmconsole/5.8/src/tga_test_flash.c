/*
 *        Copyright (c) Digital Equipment Corporation, 1991
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */

/*
 *++
 *  FACILITY:
 *
 *      Color Frame Buffer Flash Utility 
 *
 *  ABSTRACT:
 *
 *      Flasher
 *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-11-91
 *
 *  MODIFICATION HISTORY:
 *      Initial Modification	
 *
 *
 *--
*/

/**
** INCLUDES
**/

#include   "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:tga_sfbp_include.h"
#include "cp$src:tga_sfbp_callbacks.h"
#include "cp$src:tga_sfbp_prom_defines.h"
#include "cp$src:tga_sfbp_def.h"
#include "cp$src:tga_sfbp_protos.h"
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"
#include "cp$src:tga_sfbp_test_flash.h"

int       first=1;

int flash$$deep (SCIA *scia_ptr,SFBP *data_ptr,int flag );
int       AMDDevice=0,DiagMode=0,PreCharge=1;
int       *EEprom;

void      pci_outmem    (unsigned __int64 address, unsigned int value, int length);
unsigned  int pci_inmem_byte (unsigned __int64 address, int length);
unsigned  int pci_inmem      (unsigned __int64 address, int length);

#define   PCI_byte_mask 0
#define   PCI_word_mask 1
#define   PCI_long_mask 3

extern    int TGA_mem_base[];


/*+
* ===========================================================================
* = test_flash - Flash Rom Utility =
* ===========================================================================
*
* OVERVIEW:
*       This is used to program the Flash memory.
* 
* FORM OF CALL:
*       test_flash (scia_ptr,data_ptr,argc,argv,vector)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*       argc     - argument count >=1
*       argv     - argv [0]="test", argv[1]=module number if argc >1.
*       slot     - the slot number
*       vector   - callback vector array 
*
* FUNCTIONAL DESCRIPTION:
*       This will program,erase and or read the FLASH ROM.
*
* ALGORITHM:
*       - REPEAT
*        - Print out the help menu
*        - Prompt for Input 
*        - CASE character input:
*          - "d"        - dump the flash rom header.
*          - "e"  THEN
*              - If device ID valid THEN PROMPT for ready message
*                - Program Device with all 0x0( for charge purposes)
*                - Erase Device with 0xff.
*          - "h" or "?"  - THEN print out the help menu                      
*          - "n"         - Input the slot of device to be programmed.
*          - "p" THEN
*              - If device ID valid THEN PROMPT for ready message
*                - Program Device with code at 0xA0430000 in uncached KSEG1 space
*          - "r"  THEN reset device
*          - "q"  THEN quit
*       - UNTIL "q"                
--*/
int test_flash (int argc,char **argv,CCV *ccv,
                struct scia *scia_ptr,struct sfbp_data *data_ptr,int param[])

{
register int      SaveSize,i,j,s=1,rval;
char     c[10],x[10],fwrev[10];
register unsigned char *newcode,*fwcode;
unsigned char     checksum;
char     *rom,*pptr;

TgaConsoleSlot = data_ptr->sfbp$l_slot_number;

/* For interactive mode                 */
/* t #/flash -v                         */

/* This is if I wanted to skip precharge*/
for (j=0,PreCharge=1,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'n' && *pptr++== 'p' )
                {PreCharge=0;break;}

for (j=0,DiagMode=0,pptr=argv[j];j<argc;j++,pptr=argv[j])    
        if (*pptr++=='-'&& *pptr++== 'd'  )
                {DiagMode=1;break;}

if (DiagMode)
        {
        /* This is just to verify programmability       */
        SaveSize = data_ptr->sfbp$l_rom_size;
        data_ptr->sfbp$l_rom_size = 16;
        }

rom    = (char *)(TGA_mem_base[data_ptr->sfbp$l_slot_number]+ sfbp$k_a_sfbp_reg_space + sfbp$k_eprom);
EEprom = (int *)rom;

if (data_ptr->sfbp$l_verbose||data_ptr->sfbp$l_interactive)
 {
 flash$$help(scia_ptr,data_ptr);
 do 
 {
 s=1;
 FLASH_PROMPT();
 gets(c);
 switch(c[0]) 
  {
  case 'd': flash$$readrom (scia_ptr,data_ptr);break;
  case 'e': flash$$resetrom(scia_ptr,data_ptr);
	    if (flash$$intelid(scia_ptr,data_ptr))
            {
 	    ERASE_PROMPT();
	    gets(x);
	    if (x[0] == 'y' || x[0]=='Y' ) 
                {
                CHARGING();
                if ((s=flash$$program(scia_ptr,data_ptr,ZEROS,&checksum))&1) 
                 {
                 ERASING();
                 s=flash$$erase(scia_ptr,data_ptr);
                 }
		} else NOT_ERASING();
	    } else BAD_JUMPER();
            break;
   case '?':
   case 'h':flash$$help(scia_ptr,data_ptr);break;
   case 'i':if (!flash$$intelid(scia_ptr,data_ptr))BAD_JUMPER();break;
   case 'p':newcode = (unsigned char *)ROM_CODE_MODULE_ADDRESS;
             fwcode = (unsigned char *)ROM_CODE_FW_ADDRESS;
             if (strncmp(newcode,"PMAGD",5)==NULL)
             {
              CLEAR(i,fwrev,sizeof(fwrev));
              strncpy (fwrev,fwcode,8);
              {printf  ("SFB+-I Firmware Revision %s\n",fwrev);}
              if(flash$$intelid(scia_ptr,data_ptr))
                {
		PROGRAM_PROMPT();
		gets(x);
		if (x[0] == 'y'|| x[0]=='Y') 
                        {
                        PROGRAMMING();
		        s= flash$$program(scia_ptr,data_ptr, REAL ,&checksum);
                        {printf ("\n");}
		        } else NOT_PROGRAMMING();
	        } else BAD_JUMPER();
              } else {ROM_HEADER();BOOT_HEADER();}
            break;
   case 'r':if(flash$$intelid(scia_ptr,data_ptr))
                flash$$resetrom(scia_ptr,data_ptr);
            else BAD_JUMPER();
            break;
   case 'q':break;
   default :if (c[0]!=0x13 && c[0]!=0xa&&c[0]!=0)HUH_PROMPT();
   }
  } while (c[0] != 'q');
 }else                                                  
   {
   /* Autoprogram the module by default */
   /* Boot and run in other words       */
   
   newcode = (unsigned char *)ROM_CODE_MODULE_ADDRESS;
   fwcode = (unsigned char *)ROM_CODE_FW_ADDRESS;
   if (strncmp(newcode,"PMAGD",5)==NULL)
    {
     CLEAR(i,fwrev,sizeof(fwrev));
     strncpy (fwrev,fwcode,8);
     {printf  ("SFB+-I Firmware Revision To Program %s\n",fwrev);}
     if((s=flash$$intelid(scia_ptr,data_ptr)&1))
     {
      RUS_PROMPT();
      AUTO_PROMPT();
      gets(x);
      if (x[0] == 'y'|| x[0]=='Y') 
       {
       CHARGING ();
       if ((s=flash$$program(scia_ptr,data_ptr,ZEROS,&checksum))&1 )
        {
        ERASING ();
        if ((s=flash$$erase(scia_ptr,data_ptr))&1 &&
            (s=flash$$program(scia_ptr,data_ptr,REAL,&checksum))&1 )
                DONE_HEADER();
        }
       }
      } else BAD_JUMPER();
    } else {ROM_HEADER();BOOT_HEADER();}
   }

/* Disable Flash Writes */
flash$$deep (scia_ptr,data_ptr,FALSE);


if (DiagMode)
        {
        data_ptr->sfbp$l_rom_size = SaveSize;
        }

return (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = flash$$help - Help mode =
* ===========================================================================
*
* OVERVIEW:
*       print help
* 
* FORM OF CALL:
*       flash$$help (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr        - shared driver pointer
*       data_ptr        - driver data pointer
*
* FUNCTIONAL DESCRIPTION:
*       Print the menu
*
--*/
int flash$$help(SCIA *scia_ptr,SFBP *data_ptr)

{

printf("    p : program\n");
printf("    e : erase \n");
printf("    i : read ID\n");
printf("    d : dump header\n");
printf("    r : reset device\n");
printf("    h : help \n");
printf("    q : quit\n");

return (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = flash$$resetrom - Reset the ROM Device =
* ===========================================================================
*
* OVERVIEW:
*       This is used to reset the rom device .
* 
* FORM OF CALL:
*       flash$$resetrom (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       This will reset the rom device. A reset is achieved by writing 0xFF to the 
*       command register twice. We must wait 1 millisecond between writes. We then 
*       put the device in read mode by putting a zero in the command register. 
*
--*/
int flash$$resetrom (SCIA *scia_ptr,SFBP *data_ptr)


{
volatile register int tmp;

/* Enable Flash Writes */
flash$$deep (scia_ptr,data_ptr,TRUE);

(*gfx_func_ptr->pv_sfbp_write_rom) (scia_ptr,0,CMD_RESET,LW);
msdelay (1);
(*gfx_func_ptr->pv_sfbp_write_rom) (scia_ptr,0,CMD_RESET,LW);
msdelay (1);
(*gfx_func_ptr->pv_sfbp_write_rom) (scia_ptr,0,CMD_READ,LW);
msdelay (10);

msdelay(20);

/* Disable Flash Writes */
flash$$deep (scia_ptr,data_ptr,FALSE);

return (SS$K_SUCCESS);
}




/*+
* ===========================================================================
* = flash$$intelid - Reset the Device Id =
* ===========================================================================
*
* OVERVIEW:
*       This is used to read the Device ID.
* 
* FORM OF CALL:
*       flash$$intelid (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       This will read the device ID. We write 0x90 to the command register on the first
*       bus cycle and then read the device ID on the second cycle. The device ID has
*       two parts, a manufacturing ID and a part number. The first bus read gives the
*       manufacturer and the second read gives the device code, such as 28F010. This 
*       routine will return an error if either is incorrect. 
*
--*/
int flash$$intelid (SCIA *scia_ptr,SFBP *data_ptr)

{
register int      s=1,manuf,device;

/* Enable Flash Writes */
flash$$deep (scia_ptr,data_ptr,TRUE);

(*gfx_func_ptr->pv_sfbp_write_rom) (scia_ptr,0,CMD_RDINTELID,LW);
msdelay (10);

manuf  = (*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,0)&0xff;
device = (*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,4)&0xff;

switch(manuf) 
    {
    case MFG_INTEL:
        {printf("Manufacturer = Intel (0x%02x)\n", manuf);}
        switch(device) 
         {
         case PROM_INTEL_256A  :printf("Device code  = 28F256A (0x%02x)\n", device);break;
         case PROM_INTEL_256B  :printf("Device code  = 28F256B (0x%02x)\n", device);break;
         case PROM_INTEL_256X  :printf("Device code  = 28F256  (0x%02x)\n", device);break;
         case PROM_INTEL_28F512:printf("Device code  = 28F512  (0x%02x)  64K x 8\n", device);break;
         case PROM_INTEL_28F010:printf("Device code  = 28F010  (0x%02x) 128K x 8\n", device);break;
         case PROM_INTEL_28F020:printf("Device code  = 28F020  (0x%02x) 256K x 8\n", device);break;
         default               :printf("Device code  = unknown (0x%02x)\n", device);s=0;break;
         }
        break;

   case MFG_AMD:
        AMDDevice = 1;
        printf("MANUFACTURER = AMD (0x%02x)\n", manuf);
        switch(device)
          {
          case PROM_AMD_28F020 : printf("DEVICE CODE  = 28F020 (0x%02x) 256K x 8\n", device);break;
          default               :printf("DEVICE CODE  = UNKNOWN (0x%02x)\n", device);s=0;break;
          }
        break;

    default       :printf("Flash Manufacturer Unknown (0x%02x)\n", manuf);s=0;break;
    }

/* Disable Flash Writes */
flash$$deep (scia_ptr,data_ptr,FALSE);

return(s);  
}




/*+
* ===========================================================================
* = flash$$program - Program the Device =
* ===========================================================================
*
* OVERVIEW:
*       Program the device.
* 
* FORM OF CALL:
*       flash$$program (scia_ptr,data_ptr,mode,checksum)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*       mode     - Mode of programming 
*       checksum - Checksum of program (basic XOR checksum)
*
* FUNCTIONAL DESCRIPTION:
*       This will program the device. This routine can be used to program the 
*       device with all 0's or the actual data previously booted into memory.
*       The device is set up for programming first by putting the device in
*       set up mode. The next write pulse will enable the transition to 
*       active programming. We then go into program verify mode and verify each
*       byte after read back.
*               One thing to keep in mind when programming the Flash Rom is the
*       rom stride. This program is always assuming the ROM stride is 4, meaning that
*       we program every longword from the CPU side. 
*
* ALGORITHM:
*       - IF program mode is all ZERO THEN
*         - DO for size of ROM
*             - CALL flash$erase for byte location
*             - Print a progress indicator.
*         _ END DO
*       - ELSE if programming real data THEN
*         - DO for the size of rom header
*            - Call flash$do_program for byte location
*         - END DO
*         - DO for the remaining size of ROM
*            - Call flash$do_program for byte location
*         - END DO
*       - Reset ROM device 
*
--*/
int flash$$program (SCIA *scia_ptr,SFBP *data_ptr,int mode,unsigned char *checksum)

{
char      x[20];
volatile  register  int       data;
volatile  register  int       i,k,s,dotcnt,size,stride;
volatile  register  unsigned  char *romaddr,*newcode;
volatile  register  unsigned  char      readcheck;

romaddr       = (unsigned char *)0;
newcode       = (unsigned char *)ROM_CODE_ADDRESS;
*checksum     = 0;
size          = data_ptr->sfbp$l_rom_size;
stride        = data_ptr->sfbp$l_flash_rom_stride;

/* Enable Flash Writes */
flash$$deep (scia_ptr,data_ptr,TRUE);

switch (mode) 
  {
  case ZEROS: 
        if (PreCharge)
          {
          for (i=k=dotcnt=0,s=1;s&1&&i<size;i++,romaddr+=stride)
                {
                *checksum = 0;
                s=flash$$do_program ((int)romaddr,(int)0);
                if (++dotcnt==DOT_CNT){printf (".");dotcnt=0;}
                }
          printf ("\n");
          }
        break;

  case REAL:
          romaddr += HEAD_OFFSET_BYTES;
	  for (i=0,s=1;s&1&&i<8;i++,romaddr+=stride)
             {
             s=flash$$do_program ((int)romaddr,(int)sfbprombits[i]);
             *checksum ^= (unsigned char )sfbprombits[i];
             }
          if (s&1)BODY_PROGRAM();
          newcode = (unsigned char *)ROM_CODE_ADDRESS;
          size    -= HEAD_SIZE_BYTES;        
          for (i=0;i<size;i++,newcode++)*checksum ^= *newcode;
          newcode = (unsigned char *)ROM_CODE_ADDRESS;
          for (i=k=dotcnt=0;s&1&&i<size;i++,newcode++,romaddr+=stride)
                {
                s=flash$$do_program ((int)romaddr,(int)*newcode);
                if (++dotcnt==DOT_CNT){printf (".");dotcnt=0;}
                }
         printf ("\n");
	 break;
   }

flash$$resetrom(scia_ptr,data_ptr);

if (!(s&1))printf ("\nsfbp-I Program Verify Error at offset 0x%x \n",romaddr-stride);

else if (mode == REAL)
 {
 readcheck =0;
 size          = data_ptr->sfbp$l_rom_size;
 romaddr       = 0;
 for (i=0;i<size;i++,romaddr+=stride)readcheck ^= ((*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,romaddr)&0xff);

 if (readcheck == *checksum)printf ("ROM Program Verification Successfull: ROM Checksum = 0x%x\n",readcheck);
 else printf ("\nProgram Verification Failed: ROM Checksum = 0x%x Expected 0x%x\n",readcheck,*checksum);
 }


return (s);
}




/*+
* ===========================================================================
* = flash$$do_program - Actual Program Device =
* ===========================================================================
*
* OVERVIEW:
*       Program the device.
* 
* FORM OF CALL:
*       flash$$do_program (address,data)
*
* ARGUMENTS:
*       address  - address to write to (usually a longword aligned address for rom stride of 4)
*       data     - Data to write 
*
* FUNCTIONAL DESCRIPTION:
*       Program the device at the address with the specified data.
*
* ALGORITHM:
*       - DO 
*         - Write set up command to device  
*         - Write data to byte address (long aligned)
*         - Wait 10 microseconds
*         - Write program verify command to device
*         - Wait 10 microseconds
*         - Read byte location
*         - If byte location incorrect then increment retry count
*       - UNTIL byte location valid or retry count exceeded
--*/
int flash$$do_program (int address,int data)

{
volatile register int rval;
volatile register int value,deep_value,s,retries,*rd;
register char  *rom;

retries  = 0;

rom      = (char *)(TGA_mem_base[data_ptr->sfbp$l_slot_number]+ address);
rd       = (int *)rom;
value    = ((address>>2)<<8);

do 
     {
      pci_outmem(EEprom,value | CMD_SUPROGRAM, PCI_long_mask);
      pci_outmem(EEprom,value | (data&0xff),PCI_long_mask);
      krn$_micro_delay (10);                                                          
      pci_outmem(EEprom,value | CMD_PROGVERF,PCI_long_mask);
      krn$_micro_delay (10);                                                          
      rval = pci_inmem(rd,PCI_long_mask)&0xff;
      } while ( (rval != data) && ++retries<PROGRAM_RETRY_CNT);                      

s = (retries == PROGRAM_RETRY_CNT) ? 0 : 1 ; 

if (!(s&1))
        printf ("Program Verify Error at 0x%x expect 0x%x actual 0x%x\n",address,data,rval);

return (s);
}


/*+
* ===========================================================================
* = flash$$erase - Erase the Device =
* ===========================================================================
*
* OVERVIEW:
*       Erase the device.
* 
* FORM OF CALL:
*       flash$$erase (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       This will erase the device. The Flash ROM spec states that in order to 
*       properly erase the device, first program each location with a 00 which
*       charges each cell. Then to erase the device, follow the erase set up 
*       and verify algorithm.
*
--*/
int flash$$erase (SCIA *scia_ptr,SFBP *data_ptr)

{
volatile register  int  i,s,k,rval,*addr,dotcnt,size;

addr          = (int *)0;
size          = data_ptr->sfbp$l_rom_size;

/* Enable Flash Writes */
flash$$deep (scia_ptr,data_ptr,TRUE);

for (i=k=dotcnt=0,s=1;s&1&&i<size;i++) 
  {                                                                     
  if ((s=flash$$do_erase ((int)addr)))addr++;
  if (++dotcnt==DOT_CNT){printf (".");dotcnt=0;}
  }
printf ("\n");

if (!(s&1))printf ("\nSFB+-E Erase Verify Error at 0x%x\n",addr);

/* Make sure flash disabled when done   */

flash$$resetrom(scia_ptr,data_ptr);

return (s);
}


/*+
* ===========================================================================
* = flash$$do_erase - Erase the Device =
* ===========================================================================
*
* OVERVIEW:
*       Erase the device.
* 
* FORM OF CALL:
*       flash$$do_erase (address)
*
* ARGUMENTS:
*       address  - location to erase
*
* FUNCTIONAL DESCRIPTION:
*       This will erase the device at the specified address. If a location does not
*       erase, we need to make sure that the erase set up command is issued again. 
*       So, actually what will happen is that the erase setup command will be issued
*       for each location. What may happen with the device characteristics is the
*       fact that some locations may require longer erase set up times.  
*               So, the first time this routine is called, the location will not
*       be erased and we will then issue the set up command. After the set up command
*       is written, each location after that should be erased with the erase 
*       verify command.
*
* ALGORITHM
*       - DO 
*         - Write erase verify command to device  
*         - Wait 10 microseconds
*         - Read byte location
*         - If byte location not 0xff THEN
*            - increment retry count
*            - Write erase set up command again (twice)
*            - Wait 10 milliseconds for each retry count at that location
*       - UNTIL byte location equals 0xff or retry count exceeded
--*/
int flash$$do_erase (int address)

{
volatile register int      rval;
volatile register int      value,i,s,retries,*rd;
register char  *rom;

retries  = 0;
rom      = (char *)(TGA_mem_base[data_ptr->sfbp$l_slot_number]+ address);
rd       = (int *)rom;
value    = ((address>>2)<<8);

do 
  {                                                                     
  pci_outmem(EEprom,value | CMD_ERASEVERF,PCI_long_mask);
  krn$_micro_delay (20);                                                          
  rval = pci_inmem(rd,PCI_long_mask)&0xff;
  if (rval != 0xff )
   {
   pci_outmem(EEprom,value | CMD_SUERASE,PCI_long_mask) ;
   pci_outmem(EEprom,value | CMD_SUERASE,PCI_long_mask) ;
   msdelay(40);
   /* At this point the entire charge array should be erased    */
   }
  } while ((rval!=0xff) && ++retries < ERASE_RETRY_CNT);

s = (retries==ERASE_RETRY_CNT) ? 0 : 1; 

if (!(s&1))
        printf ("Erase Verify Error at 0x%x expect 0xff actual 0x%x\n",address,rval);
        
return (s);
}



/*+
* ===========================================================================
* = flash$$readrom - Read the ROM Device =
* ===========================================================================
*
* OVERVIEW:
*       Read the device.
* 
* FORM OF CALL:
*       flash$$readrom (scia_ptr,data_ptr)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*
* FUNCTIONAL DESCRIPTION:
*       This will read the device. The ROM Header information is read. This is 160 Bytes in
*       length or 40 longwords. We read the 40 longwords and then set a structure pointer
*       to the local copy and print out the fields. 
*
--*/
int flash$$readrom (SCIA *scia_ptr,SFBP *data_ptr)

{
volatile register int      readcheck,data,i,stride,size;
volatile register int      *addr;
volatile register char     *romaddr,*start,*inc;
        
start         = (char *)0;
start         += HEAD_OFFSET_BYTES;
addr          = (int *)start;
inc           = (char *)start;
size          = data_ptr->sfbp$l_rom_size;
stride        = data_ptr->sfbp$l_flash_rom_stride;

/* Enable Flash Writes */
flash$$deep (scia_ptr,data_ptr,TRUE);

(*gfx_func_ptr->pv_sfbp_write_rom) (scia_ptr,addr,CMD_READ,LW);

/* DISABLE Flash Writes */
flash$$deep (scia_ptr,data_ptr,FALSE);

msdelay (10);

/* Print the first 8 locations  */

for (i=0;i<8;i++,inc+=stride)
 {
 addr = (int *)0;
 data = (*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,addr)&0xff;
 printf ("0x%x = 0x%x\n",addr,(data&0xff));
 }

/* Then print the ROM 3E0 Header*/
addr = (int *)start;
inc  = (char *)start;
for (i=0;i<8;i++,inc+=stride)
 {
 addr = (int *)inc;
 data = (*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,addr)&0xff;
 printf ("0x%x = 0x%x\n",addr,(data&0xff));
 }

readcheck =0;
romaddr = (char *)0;
for (i=0;i<size;i++,romaddr+=stride)
        readcheck ^= ((*gfx_func_ptr->pv_sfbp_read_rom) (scia_ptr,romaddr)&0xff);

printf ("ROM Check  = 0x%x\n",readcheck&0xff);

return (SS$K_SUCCESS);
}


/*+
* ===========================================================================
* = flash$$deep - Deep Register Setup =
* ===========================================================================
*
* OVERVIEW:
*       This is used to read the Device ID.
* 
* FORM OF CALL:
*       flash$$deep (scia_ptr,data_ptr,flag)
*
* ARGUMENTS:
*       scia_ptr - shared driver area pointer
*       data_ptr - driver data area pointer
*       flag     - true of false
*
* FUNCTIONAL DESCRIPTION:
*       Enable or disable flash program. The deep register affects 
*       the video ram depth and can be deadly, so make sure you
*       always do a read-modify-write operation on the register. Also,
*       wait for chip idle before accessing chip again.
*
--*/
int flash$$deep (SCIA *scia_ptr,SFBP *data_ptr,int flag )

{
union deep_reg *deep;
int   deep_value;

/* Wait for chip idle before accessing deep     */
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

deep           = (union deep_reg *)&deep_value;
deep_value = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_deep);
deep->sfbp$r_deep_bits.sfbp$v_flashe = flag;
(*gfx_func_ptr->pv_sfbp_write_reg)(scia_ptr,sfbp$k_deep,deep->sfbp$l_whole,LW);

/* Wait for chip idle after  accessing deep     */
(*gfx_func_ptr->pv_sfbp_wait_csr_ready)(scia_ptr,data_ptr);

deep_value = (*gfx_func_ptr->pv_sfbp_read_reg)(scia_ptr,sfbp$k_deep);

return(1);  
}

