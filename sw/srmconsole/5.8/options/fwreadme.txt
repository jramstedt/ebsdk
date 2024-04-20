###########################################################################
#19 13 Oct 2000
Programs PIO values for M5229 according to disks present.
Treats RTC as per Tinosa/Nautilus. i.e. not square wave like DP264

###########################################################################
#20 17 Oct 2000
UART debug write. I.e. writes 0 to THR until error goes away.
IDE PIO setup for all modes up to 5, doesn't setup for DMA yet.
Timer interrupt as per Tinosa.

These are all by default OFF.

The variable api_debug turns them on.

For example

>>>set api_debug 3
API Debug Value 3
Clock Debug Enabled
PIO Setup Enabled
UART Fix Disabled
>>>

The values are:
#define APIDB_CLOCK	0x01
#define APIDB_IDEPIO	0x02
#define APIDB_UARTFIX	0x04


This also has a delay on the IDE write (which was added for debug) removed.

###########################################################################
#21 20 Oct 2000
Contains a small fix in the IDE driver to setup IDE address as per other platforms
unlikely to make any difference, but worth trying bearing in mind CS20 IDE problem.

###########################################################################
#22 23 Nov 2000
Contains IRQ rerouting, PCI renumbering and change of system type to 1400 (ES40)
for Shark P2.

###########################################################################
#51 23 Nov 2000
5.8 port of software.

###########################################################################
#54 Dec 2000
Contains palcode fix to try to mask out bad (grounded) interrupts on Shark.

###########################################################################
#55 14 Dec 2000
SHARK: IIC devices on the bad bus are renamed with a leading underscore to
enable testing of the device.
SF: IIC appears to work fully and this pointer. Reset now works OK on SF+

###########################################################################
#56 20 Dec 2000
All Platforms: Bit 0 of api_debug now enable simple com debug. Tinosa Always ON.

SHARK: IIC devices update with Soohoons info. Fru list functions correctly.
SF: All IIC devices work. Fru list functions correctly.

###########################################################################
#57 21 Dec 2000

ROM Layouts change to match Eric's latest spec.
Tinosa Does not Scan LDT Bus as per Kyuwon request.

NOT api_debug bit values.
All Platforms
#define APIDB_SIMCOMDBG	0x01			/* Outputs simple debug code to COM1, off by default */

***SHARK
#define APIDB_CLOCK	0x02			/* Clock fix. Reads RTC on clock Int, off by default */
#define APIDB_IDEPIO	0x04			/* Sets up IDEPIO values, on by default */
#define APIDB_UARTFIX	0x08			/* Reads UART alot to clear errors, off by default */

***TINOSA
#define APIDB_ALLPCI	0x02			/* Scan all buses include LDT, off by default*/

***SWORDFISH
#define APIDB_ADPDIS	0x02			/* Adaptec disabled on UP2000, off by default */

***NAUTILUS
None

***GLXYTRAIN
None

###########################################################################
#59 01 Jan 2001

1/ If the V2 header address for a rom image does not match the address
   SRM is expecting it provides the option of using the new address. This
   should make future upgrades where we wish to move the SRM image
   around easier. This feature will probably never actually appear to the user
   as the address will always match.

2/ On a UP2000+ typing

   show power -cs20psu 

   will show the cs20psu attached to the PCF8584 I2C bus at address 0x5e. This
   feature is really for manufacturing.

###########################################################################
#61 13 Jan 2001

1/ EV68 support in show config

###########################################################################
#62 13 Jan 2001

1/ Removed ClockDebug from API_DEBUG no longer option of reading RTC on Shark
clock int.

2/ 

###########################################################################
#63 19 Jan 2001

1/ Removed PAL interrupt masking code from OSF palcode for CS20.

###########################################################################
#65 31 Jan 2001

Following Bugs:
57-0309-0A - Up2000 - Reports SF as SF+
57-0314-0A - Up2000+ - Upgrade from lfu causes all enviromental variables to be lost.
57-0315-0A - Up2000+ - Powering of system while kernel boot causes all enviromental variables to be lost.
57-0307-0A - Up2000 - Show FRU does not show processor's serial number.
57-0308-0A - Up2000 - Show power and init broken on SF.
57-0281-0A - CS20 - SRM 5.8-1.56 does not switch uart mux
57-0269-0A - CS20 - CS20 #3 shark does not have SRM support for adaptec U2W
57-0290-0A - CS20 - incorrect MHz value after software update
57-0291-0A - CS20 - Floppy controller errors with SRM A5.8-56 
57-0298-0A - CS20 - Init fails to work from SRM prompt
57-0055-0A - CS20 - SRM Doesn't display 8MB BCache size
57-0153-0A - CS20 - Psu adm9240 failures misinterpreted by srm
57-0229-0A - CS20 - SRM does not print EV68 identity.
57-0295-0A - CS20 - Lfu prints SRM versions as 5.8-0.
57-0297-0A - CS20 - Show power failed with SRM 
57-0172-0A - CS20 - PSU inhibit on motherboard i2c control does not work.

###########################################################################
#67 01 Feb 2001

CS20 removed write from ac_action variable to pca8550 on IIC bus.
CS20 corrected sense of system fail LED printout in show power.

###########################################################################
#69 22 Feb 2001

wol_enable environment variable added to enable the Wake on Lan to be turned on or off.
Valid values are ON and OFF.

