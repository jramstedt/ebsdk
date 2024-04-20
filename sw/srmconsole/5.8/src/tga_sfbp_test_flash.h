
#ifdef HX
int sfbprombits[8] = {
    0x01,	                        /* ROM width in bytes		*/
    0x04,	                        /* ROM stride in bytes		*/
    0x10,	                        /* ROM size in 8KB increments	*/
    0x01,	                        /* Minimum of 4 Mbytes          */
    0x55,	                        /* Test patterns		*/
    0x00,	                        /* Test patterns		*/
    0xAA,	                        /* Test patterns		*/
    0xFF };	                        /* Test patterns		*/
#else
int sfbprombits[8] = {
    0x01,	                        /* ROM width in bytes		*/
    0x04,	                        /* ROM stride in bytes		*/
    0x20,	                        /* ROM size 256K                */
    0x01,	                        /* Minimum of 4 Mbytes          */
    0x55,	                        /* Test patterns		*/
    0x00,	                        /* Test patterns		*/
    0xAA,	                        /* Test patterns		*/
    0xFF };	                        /* Test patterns		*/
#endif


#define RAM_CODE_ADDRESS        0xA0330100	
#define ROM_CODE_ADDRESS	0xA0430100	
#define ROM_CODE_MODULE_ADDRESS	0xA0430110	
#define ROM_CODE_FW_ADDRESS	0xA0430100	
#define PROM_OFFSET		0x00000000
#define HEAD_OFFSET_BYTES	0x000003E0
#define HEAD_SIZE_BYTES		0x00000400
#define HEAD_MODULE_BYTES	0x00000040
#define ROMSIZE	16

#define  HUH_PROMPT()      {printf("huh ?\n");}
#define  RUS_PROMPT()      {printf("SFB+-I ROM Contents Will Be Destroyed in Slot %d\n",TgaConsoleSlot);}
#define  AUTO_PROMPT()     {printf("SFB+-I Are You Ready to Program Slot %d (y/n) ? ",TgaConsoleSlot);}
#define  ROM_HEADER()      {printf("SFB+-I No ROM Image Loaded at 0xa0430000\n");}
#define  BOOT_HEADER()     {printf("SFB+-I Load ROM Image >> boot -n #/mop/path\n");}
#define  FLASH_PROMPT()    {printf("SFB+-I Flash--> ");}
#define  HEADER_DONE()     {printf("SFB+-I Flash Rom Header Programmed\n");}
#define  BODY_PROGRAM()    {printf("SFB+-I Programming ROM Image Now\n");}
#define  NEW_TgaConsoleSlot_PROMPT() {printf("SFB+-I New slot assigned %d\n",TgaConsoleSlot);}
#define  ERASE_PROMPT()    {printf("SFB+-I Ready to erase device in slot %d (y/n) ? ",TgaConsoleSlot);}
#define  FOUND_PROMPT()    {printf("SFB+-I Found ROM Image Ready ? (y/n) ");}
#define  PROGRAM_PROMPT()  {printf("SFB+-I Ready to program device in slot %d (y/n) ? ",TgaConsoleSlot);}
#define  TgaConsoleSlot_PROMPT()     {printf("SFB+-I Current Slot is %d Enter New Slot of Device  ? ",TgaConsoleSlot);}
#define  BAD_JUMPER()      {printf("SFB+-E Unknown Manuf/Device Slot %d Jumper On ?\n",TgaConsoleSlot);}
#define  BAD_TgaConsoleSlot()        {printf("SFB+-E Bad Slot: Use 0,1 or 2 \n");}
#define  CHARGING()        {printf("SFB+-I Precharging device in slot %d\n",TgaConsoleSlot);}
#define  ERASING()         {printf("SFB+-I Erasing device in slot %d\n",TgaConsoleSlot);}
#define  NOT_ERASING()     {printf("SFB+-I Not Erasing\n");}
#define  PROGRAMMING()     {printf("SFB+-I Programming device in slot %d\n",TgaConsoleSlot);}
#define  NOT_PROGRAMMING() {printf("SFB+-I Not Programming\n");}
#define  DONE_HEADER()     {printf("SFB+-I Program Completed \n");}
#define  ERASE_RETRY_CNT     100
#define  PROGRAM_RETRY_CNT    25
#define  DOT_CNT           4096

#define  MFG_INTEL	   0x89
#define  PROM_INTEL_256A   0xB1
#define  PROM_INTEL_256B   0xB2
#define  PROM_INTEL_256X   0xB9
#define  PROM_INTEL_28F512 0xB8
#define  PROM_INTEL_28F010 0xB4
#define  PROM_INTEL_28F020 0xBD

#define  MFG_AMD           0x01
#define  PROM_AMD_28F020   0x2A

#define  CMD_READ	   0x00
#define  CMD_RDINTELID	   0x90
#define  CMD_SUERASE	   0x20
#define  CMD_ERASEVERF	   0xA0
#define  CMD_SUPROGRAM	   0x40
#define  CMD_PROGVERF	   0xC0
#define  CMD_RESET	   0xFF
#define  ZEROS             0
#define  TEST	           1
#define  REAL	           2
