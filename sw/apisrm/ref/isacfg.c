/*
 * COPYRIGHT (c) 1987, 1988, 1994, 1995 BY
 * DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
 * ALL RIGHTS RESERVED.
 * 
 * THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
 * ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
 * INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
 * COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
 * OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
 * TRANSFERRED.
 * 
 * THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
 * AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
 * CORPORATION.
 * 
 * DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
 * SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
 */

/*	 
 *  Revision History:
 *
 *  11-Nov-1998 - tna   Conditionalized for YUKONA
 *
 *  24-Jun-1998 - ska   Add support for Yukon and Cortex
 *  
 *  2-Feb-1998	- dtr	Final monet support (miata removed)
 *
 *  07-Feb-1997	- dtr	Added ide support for miata
 *
 *  07-feb-1997 - smd   Corrected device numbering for the burns related 
 *			embedded devices.
 *
 *  17-Oct-1996 - jlw   Backed out changes of 9/5 -- not required
 *
 *  25-sep-1996 - smd   Added com2 to Burns as default configuration. Changed 
 *			all other burns specific stuff.
 *
 *  05-Sep-1996 - jlw   Added routine delete_table_end to reset expanded
 *                      end of isa table.
 *
 *  22-Aug-1996 - jlw	Allowed the FLOPPY device to be disabled so that
 *			systems in mfg without floppies would configure
 *			their exercisers properly. Mfg uses the
 *			"ls dva0" command in order to size properly.
 *
 *  21-mar-1996 - al    Added save_table for modifications to devices that
 *                      are only allowed to be enabled/disabled.
 *
 *  11-mar-1996 - al    Changed a good amount of stuff to support Burns.
 *                      Burns has embedded devices that are not in slot zero,
 *                      which messed up isacfg.  So I made three tables, one
 *                      to determine what can be modified, one for what can
 *                      be deleted, and one for what can be disabled.  This
 *                      actually simplifies things by making the logic of 
 *                      what can be changed independant of slot.  If you add
 *                      an embedded device, you should add it to the appro=
 *                      priate tables to determine it's characteristics.
 *
 *  27-Nov-1995 - al    Remove multiple get_table calls.  Now just do it once
 *                      in nvram driver init.  Also, put in return statuses.
 *
 *  10-OCT-1995 - rps   Add IRQ assignment for H8.
 *
 *  10-OCT-1995 - al    Change get_table and save_table to do all devices,
 *                      whether used or unused.  This will fix problem
 *                      with fields showing as zero.  Adding a zap_entry
 *                      during a make will fix the bug also, but this seems
 *                      to make more sense.  Since the values in NVR are
 *                      correct, we might as well use them.
 *
 *  31-AUG-1995 - al    DMA incorrectly set to zero in zap_entry because of
 *                      screw-up w/64 bit values.  Assign UNUSED directly.
 *
 *  30-AUG-1995 - rhd   Remove returns in make and assign status to msg
 *
 *  22-Aug-1995 - al    More mods to work with nvram_driver.
 *
 *  07-Aug-1995 - rhd   Modified the isacfg, get_table and save_table functions
 *			to use the NVRAM_DRIVER Funtionalilty.
 *
 *  07-Jul-1995 - rhd   Add check for ETYP data being suppled, if mode is make,
 *			in isacfg() and add remaining code of add_entry in
 *			make section of isacfg and remove add_entry function
 *
 *  6-jul-1995 -  al    Remove global init of table since it's not needed.
 *			Redo table_init to reduce size.  Change zap_entry
 *			to use sizeof handle instead of hardcoded 16.
 *
 *  5-jul-1995 -  al    Allow irq/dma conflicts if device disabled.
 *
 *  26-jun-1995 - al    Removed dmamode code.  It wasn't being used.  Rewrote
 *			qual_vs_table_conflict_warning to reduce size. Renamed
 *			qual_vs_table_conflict_warning to conflict_warning.
 *			Simplified get_table.  Disallowed negative
 *			IRQs and DMAs in conflict_warning.  Changed get_table
 *			to remove WHILE loop, so no possibility of hanging
 *			console due to bad NVRAM.
 *
 *  23-jun-1995 - al    Removed conflict_warning routine, which wasn't being
 *			used. Rewrote isacfg routine to make it under-
 *			standable.  Ditto dump_record routine.
 *
 *  08-Jun-1995 - rhd   Add test for hardcoded IRQ assignments 0,2,8,11,13 and
 *			if no arguments are given return(msg_success) rather
 *			then set status = msg_success
 *
 *  23-May-1995 - rhd   Fix Ptt M3X-00047 Can't mod dev outside of slot 1
 *
 *  19-May-1995	- er	Fixed AlphaPC64, EB66+ and EB164 conditionals so 
 *			itable is defined for these platforms.
 *
 *  17-May-1995 - rhd   Added reset to global var DevNotGiv=0 in show_isacfg()
 *			after it completed showing all the dev data when 
 *			device input was omitted from command line.
 *
 *  17-may-1995	- dtr	Correct table init.  Data is now local and not in the
 *			h file associated with it.  Also added mouse, kbd, and
 *			comm1 back in to table.
 *
 *  05-May-1995 - rhd   Added section in show_isacfg() so "isacfg -slot n"
 *			displayed all devices in the slot.
 *
 *  05-May-1995 - rhd   Added status = msg_success after show_isacfg call in
 *			isacfg to stop "msg_isa_notfound" after any isacfg
 *			-slot request.
 *
 *  13-Apr-1995 - rhd   Added reset enable field in init_table() for embedded
 *			devices 3-5.
 *			Added ifdef EXECUTE_CONFLICT_WARNING around
 *			conflict_warning()
 *			Added else if modifier not enadev for dev 3-5 to report
 *			error make no change isacfg().
 *
 *  30-Mar-1995 - noh   Modified code to initialize table at compile time.
 *			Changed code that copies structure members to 
 *			reduce size of module. Table now allocates space
 *			for 12 entries (MAX_ENTRIES) as this is what the
 *			code uses.
 *
 *  24-Feb-1995	- rbb	Conditionalize for the EB164
 *
 *  24-Feb-1995	- er	Conditionalize for AlphaPC-64 (Cabriolet) and EB66+
 *
 *  27-Dec-1994 - er	Conditionalize for EB66
 *
 *  05-dec-1994 - noh   Added support for Burns embbeded device H8 microcntrllr
 *
 *  22-Nov-1994	- al 	Change to DMA and IRQ conflict reporting
 *
 *  18-oct-1994 - noh   Changed DMA channels usage on AUDIO device (Burns Only)
 *
 *  13-oct-1994 - noh   Added support for Burns embbeded devices (AUDIO, VGA) 
 *			and removed COM2 device. 
 *
 *  11-Oct-1994	- er	Conditionalize for EB64+
 *
 *  22-jul-1994 - db    Add show config command support
 *
 *  13-may-1994	- dtr	Removed the reference to avantileds.  This was breaking
 *			all but avanti.
 */	 




/*
 * INCLUDE FILES
 */
#include "cp$src:msgnum.h"
#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:parser.h"
#include "cp$src:eisa.h"
#include "cp$src:pb_def.h"
#include "cp$src:nvram_def.h"
#include "cp$src:common.h"
#include "cp$src:isacfg.h"
#include "cp$inc:platform_io.h"
#include "cp$inc:prototypes.h"

/*
 * DEFINES
 */
#undef DEBUG

#if CORTEX || YUKONA
 union isa_config_table itable;
 union isa_config_table *isacfg_table = &itable;
 extern unsigned int floppy_att;
#endif

#if CABRIOLET || EB66P
extern union isa_config_table *isacfg_table;
#else
union isa_config_table *isacfg_table;
#endif


#define malloc(x) dyn$_malloc(x,DYN$K_SYNC|DYN$K_NOOWN, 0, 0, (struct ZONE *)0)
#define free(x) dyn$_free(x,DYN$K_SYNC|DYN$K_NOOWN)

/*
 * FUNCTION PROTOTYPES
 */
int           isacfg(int argc, char *argv[]) ;
int           get_table (void);
void          show_isacfg(struct QSTRUCT *qual,int wildcard);
void          init_table(void);
int           conflict_warning ( struct QSTRUCT *qual );
static void   modify_entry(struct QSTRUCT qual[],
                           struct TABLE_ENTRY *isacfg_table_entry);
static void   zap_entry(struct TABLE_ENTRY *isacfg_table_entry);
static void   delete_entry(int victim);
int           save_table(void);
unsigned long isacfg_get_device(char *handle,int instance,
				struct TABLE_ENTRY *ptr);
static void   dump_record(struct TABLE_ENTRY *isacfg_table_entry);
static void   dump_header(struct TABLE_ENTRY *isacfg_table_entry, 
                          unsigned int old_slot);
int can_be_disabled  ( char *handle ) ;
int cant_be_modified ( char *handle ) ;
int cant_be_deleted  ( char *handle ) ;
 

/*
 * GLOBAL VARIABLES
 */
char *TYPE_INIT = "type >>>init to use these changes\n" ;
char *INITIALIZING = "Initializing table to defaults\n" ;
char *isa_type[] = { "Unused","Singleport", "Embedded","Multiport"};
char *isa_enable[] = {"No", "Yes"};
char *devices_that_can_be_disabled [] = /*These devices can only be disabled*/
   {
    "PCXBJ", /* Sound card */
     "WD90",  /* Graphics on Burns */
     "LPT1",  
     "FLOPPY", /* Added for Mfg Needs */
     "COM2",
#if 0
     "ES1888",	/* sound */
#endif
     0        /* Terminator */
   } ;

char *devices_that_cant_be_deleted [] =
   { "MOUSE", 
     "KBD",
     "COM1",
     "COM2",
     "LPT1",    /* Parallel port */
#if   MONET
     "EIDE",	/* for Monet ide support */
#endif
     "FLOPPY",
     "PCXBJ",   /* Sound card */
     "WD90",    /* Graphics on Burns */
     "H8",      /* Kbd microcontroller */
     0          /* Terminator */
   } ;

char *devices_that_cant_be_modified [] =
   { "MOUSE", 
     "KBD",
     "COM1",
     "COM2",
     "LPT1",    /* Parallel port */
#if MONET
     "EIDE",	/* for monet ide support */
     "USB",
#endif
     "FLOPPY",
     "WD90",    /* Graphics on Burns */
     "H8",      /* Kbd microcontroller */
     0          /* Terminator */
   } ;


/*+
 * ============================================================================
 * = isacfg - sets or shows user inputted isa configuration data.	      =
 * ============================================================================
 *
 * OVERVIEW:
 *    
 *	This command allows the user to input data that will be saved in NVR
 *	for OSF and VMS so that the ISA configuration can be saved.  The OS's
 *	need this information since ISA is a black box to them.  
 *
 *   COMMAND FMT: isacfg 0 0 0 isacfg
 *
 *   COMMAND FORM:
 *  
 *	isacfg ([-slot <slot#>] 
 *		[-dev <device#>] 
 *		[-all|-rm|-mk|-mod|-init] [-<field> <value>] )
 *
 *   COMMAND TAG: isacfg 0 RXB isacfg
 *
 *   COMMAND OPTION(S):
 *
 * 	-slot <slot#> - allows the user to enter the slot # of the isa adapter. 
 *	  0 is the local multiport adaptor 1 through 3 are correspond to the 
 *	  physical slots on the mustang ISA riser from lowest to highest 
 *
 *	-dev <dev#> - optional, defaults to 0 if not entered. On a 
 *	  multifunction or multiport adaptor, to specify a device on the 
 *	  adaptor.
 *
 *	-all  Will show the whole config table.  overrides all other commands.
 *
 *	-mk   Add an entry into the table
 *
 *	-rm   Deletes an entry from the table
 *
 *	-mod  Modify an entry
 *
 *	-init Resets the table to factory defaults
 *
 *	-etyp <#>   - define an entry type for this entry. # can be:  
 *		    0 - unused
 *		    1 - single option
 *		    2 - Embedded Multiport device
 *		    3 - multiport option device
 *
 *	-handle <string>    - a 15 character name to bind to driver
 *
 *	-totdev  <#>   - placeholder for user to keep track of # devices at 
 *	  this slot.  must be modified by user for their own particular use.
 *
 *	-iobase<0-5> <#> - specify up to six iobase registers for a particular
 *				device entry.
 *
 *	-membase<0-2> <#> - specify up to three r/w isa memory regions
 *
 *	-memlen<0-2> <#> - specify length corresponding to membase<0-2>
 *
 *	-rombase <#> - specify an address for a isa bios ext. rom
 *
 *	-romlen	<#> - specify length of rom
 *
 *	-irq<0-3> <#> - allow the user to assign up to four IRQ channels to dev.
 *
 *	-dmachan<0-3> <#> - allow user to specify four dma channels for dev
 *
 *   COMMAND EXAMPLE(S):
 *~
 *
 *    >>>ISACFG -slot 1 -dev 0 -mk -IRQ0 12 -DMA0 1000 -handle WIDGET109
 *
 *    >>>ISACFG -slot 0 -dev 0
 *
 *    >>>ISACFG -all
 *
 *
 *~
 *   COMMAND REFERENCES:
 *
 * 	None
 *
 * FORM OF CALL:
 *
 *	isacfg( argc, argv);
 *
 * RETURN CODES:
 *
 *  msg_success - normal completion
 *    
 * ARGUMENTS:
 *
 *  int argc		- number of arguments
 *  char *argv []	- array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 * 	None
 *
-*/

int isacfg(int argc, char *argv[])
{
struct QSTRUCT qual[QMAX];
unsigned int entry, found=0 ;
int status ;
int qualifier;
int qcount;
int instance;


    /*	 
    **  If the count is zero then we need to load the isa table.  It is coming
    **	from the init code.
    */	 

    (void)get_table();

   /* No params is the init condition from build config.
    * Get table and leave.
    */
   if (argc == 0)
   {
      return(msg_success);
   }

   /* Get command line.
    *
    */
   status = qscan(&argc,argv,"-",QSTRING,qual);
   if (status != msg_success)
   {
      err_printf (status);
      return (status) ;
   }

   /* Show everything.
    *
    */
   if (qual[QALL].present)
   {
      show_isacfg(&qual[0],1);
      return ( msg_success ) ;
   }

   /* Initialize table to defaults.
    *
    */
   else if (qual[QINIT].present)
   {
      init_table();

      return ( save_table () ) ;
   }

   else if (!qual[QSLOT].present)
   {
      err_printf("ERROR: slot number required\n");
      return ( msg_success ) ;
   }

   /* If at least slot is present, but it's not a make, mod, or remove,
    * then it's a show 
    */
   else if ( (!qual[QMK].present) && (!qual[QRM].present) && 
             (!qual[QMOD].present) )
   {
      show_isacfg(&qual[0],0);
      return ( msg_success ) ;
   }

   else if (!qual[QDEV].present) /*To do anything else, we need a device*/
   {
      err_printf("ERROR: device number required\n");
      return ( msg_success ) ;
   }

   /* Find entry in ISA table
    *
    */
   for ( entry = 0; entry < MAX_ENTRIES ; entry++)
   {
      if (isacfg_table->table.device[entry].entry_type != ENTRY_NOT_USED)
      {
	 if ((isacfg_table->table.device[entry].isa_slot == 
		qual[QSLOT].value.integer) &&
	    (isacfg_table->table.device[entry].dev_num == 
		qual[QDEV].value.integer) )
	 {
	    ++found ;
	    break ;
	 }
      }  
      else break ; /*1st null entry is what we want*/
   }   


   /* MAKE SECTION START
    *  - adds an entry to the config table
    *
    */
   if ( qual[QMK].present )
   {

      /* Don't add entry to table if it already exists */
      if ( found )
      {
	 err_printf("This device already exists.\n");
	 return ( msg_isa_exists ) ;
      }

      /* Table is full */
      else if ( entry == MAX_ENTRIES )
      {
	 err_printf("ISA table is full.\n") ;
	 return ( msg_failure ) ;
      }

      /* ETYP qualifier not entered*/
      else if (!qual[QETYP].present)
      {
	 err_printf("ERROR: etyp field input required.\n");
	 return ( msg_isa_noetyp ) ;
      }

      /* HANDLE qualifier not entered */
      else if ( !qual[QHANDLE].present )
      {
	 err_printf("ERROR: Name field input required.\n");
	 return ( msg_isa_noname ) ; 
      }

      /* check IRQ and DMA Conflicts */
      else if ( conflict_warning ( qual ) != msg_success )
      {
	 return ( msg_found_conflict ) ;
      }

      /* OK to MAKE */
      else
      {
	 /* put the -etyp -slot and -dev at this place in the table
	 * if not total devices is set then set the total devices to 1
	 * even if the device is a multiport option....
	 *
	 */
	 isacfg_table->table.device[entry].entry_type = 
						qual[QETYP].value.integer;
	 if (!qual[QTOTDEV].present)
	 {
	    isacfg_table->table.device[entry].total_devices = 1;
	 }
	 isacfg_table->table.device[entry].isa_slot = qual[QSLOT].value.integer;
	 isacfg_table->table.device[entry].dev_num =  qual[QDEV].value.integer;

	 isacfg_table->table.header.num_entries++;
	 modify_entry(&qual[0],&isacfg_table->table.device[entry]);
	 return ( save_table () ) ;
      }
   }

   /* if device not found can not remove or modify */
   else if ( !found )
   {
      err_printf("ERROR:  Device not found.\n") ;
      return ( msg_isa_notfound ) ;
   }

   /* MAKE SECTION END
    *
    */


   /* DELETE SECTION START
    *
    */
   if ( qual[QRM].present ) 
   {
      if ( cant_be_deleted ( &isacfg_table->table.device[entry].handle[0] ) )
      {
	 err_printf("Not allowed to remove this device.\n");
	 return ( msg_permdev_nodel ) ;
      }
      else
      {
	 delete_entry(entry);
	 return ( save_table () ) ;
      }
   }   

   /* DELETE SECTION END
    *
    */


   /* MODIFY SECTION START
    *
    */

   if ( qual[QMOD].present ) 
   {     
      if( can_be_disabled(&isacfg_table->table.device[entry].handle[0] ) )
      {
         if ( qual[QENADEV].present )
         {
            isacfg_table->table.device[entry].device_enable = 
               qual[QENADEV].value.integer;
            return ( save_table () ) ;
         }
         else
            err_printf("Can only enable/disable this device.\n");
         return msg_success ;
      }

      if ( cant_be_modified ( &isacfg_table->table.device[entry].handle[0] ) )
      {
         err_printf("Not allowed to modify this device.\n");
         return ( msg_permdev_nomod ) ;
      }

      /* check IRQ and DMA Conflicts */
      if ( conflict_warning ( qual ) != msg_success )
      {
	 return ( msg_found_conflict ) ;
      }

      modify_entry(&qual[0],&isacfg_table->table.device[entry]);
      return ( save_table () ) ;
   }

   /* MODIFY SECTION END
    *
    */

   /* Catch-all
    *
    */
   err_printf("ERROR: Invalid ISACFG parameters\n") ;
   return (msg_isa_notfound) ;
}



/*
**++
** ========================================================
** =  cant_be_deleted - Check if this device can be deleted.
** ========================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**	Determines if an entry can be removed from the table.
**		
**  FORMAL PARAMETERS:
**
**      handle - pointer to device name in the table. 
**
**  RETURN VALUE:
**
**	0 - if device and be deleted
**
**	1 - if device cannnot be deleted
**
**  SIDE EFFECTS:
**
**      None
**
**--
*/ 

int cant_be_deleted ( char *handle ) 
{
   int i=0 ;

    while ( devices_that_cant_be_deleted[i] != 0 ) 
   {
      if (! strcmp_nocase ( handle, devices_that_cant_be_deleted[i] ) )
         return 1 ; /* Device found.  Can't delete it. */
      ++i ;
   }
   return 0 ; /* Device not in table, so it can be deleted. */
}



/*
**++
** ========================================================
** =  cant_be_modified - Check if this device can be modified.
** ========================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**	Determines if an entry can be modified in the table.
**		
**  FORMAL PARAMETERS:
**
**      handle - pointer to device name in the table. 
**
**  RETURN VALUE:
**
**	0 - if device and be modified
**
**	1 - if device cannnot be modified
**
**  SIDE EFFECTS:
**
**      None
**
**
**--
*/ 

int cant_be_modified ( char *handle ) 
{
   int i=0 ;

   while ( devices_that_cant_be_deleted[i] != 0 ) 
   {
      if (! strcmp_nocase ( handle, devices_that_cant_be_modified [i] ) )
         return 1 ; /* Device found.  Can't modify it. */
      ++i ;
   } 
   return 0 ; /* Device not in table, so it can be modified. */
}



/*
**++
** ========================================================
** =  can_be_disabled - Check if this device can be disabled.
** ========================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**	Determines if an entry can be disabled in the table.
**		
**  FORMAL PARAMETERS:
**
**      handle - pointer to device name in the table. 
**
**  RETURN VALUE:
**
**	0 - if device and be disabled
**
**	1 - if device cannnot be disabled
**
**  SIDE EFFECTS:
**
**      None
**
**
**--
*/ 

int can_be_disabled ( char *handle) 
{
   int i=0 ;

   while ( devices_that_can_be_disabled[i] != 0 )
   {
      if (! strcmp_nocase ( handle, devices_that_can_be_disabled[i] ) )
         return 1 ; /* Device found.  Can disable it. */
      ++i ;
   } 
   return 0 ; /* Device not in table, so it can't be disabled. */
}



/*
**++
** ========================================================
** =  conflict_warning 
** ========================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**	This routine warns users of potential ISA config conflicts.  It is more
**	thorough than the old conflict_warning(), as it tests the qualifier 
**	structure against itself, to make sure the user didn't make a mistake
**	entering the command, and against the ISA table, to make sure that the
**	command won't cause a conflict.  Right now, the old conflict_warning()
**	function is an "after the fact" problem finder.  That is, the user has 
**	already been allowed to create a conflict, and the old conflict_warning
**	function notifies the user after the ISA table has been updated.  This 
**	function will allow an abort of the ISACFG command to happen without
**	the ISA table being affected.
**
**  FORMAL PARAMETERS:
**
**	struct QSTRUCT *qual
**		structure of ISACFG command qualifiers
**       
**
**  RETURN VALUE:
**
**	msg_found_conflict or
**	msg_success
**
**  SIDE EFFECTS:
**
**      None
**
**  DESIGN:
**
**      
**
**
**
**--
*/ 

int conflict_warning ( struct QSTRUCT *qual )
{
   int entry;
   int num = isacfg_table->table.header.num_entries;
   struct TABLE_ENTRY *table;
   unsigned long unused = (unsigned long)0x80000000;
   int status = msg_success ;
   unsigned int temp1, temp2 ;
   struct QSTRUCT *qual_ptr ;

   qual_ptr = qual ; /* Keep ptr to original qual structure.*/

  /* WARN ABOUT ANY IRQ CONFLICTS*/
  /* Make sure QUAL stucture doesn't have any conflicts.  There are 6
   * distinct combinations given that you have 4 items taken 2 at a
   * time, order independant.   
   */

   for ( temp1 = QIRQ0; temp1 <= QIRQ3; ++temp1 ) 
   {
      if ( qual[temp1].present )
      {
	 switch(qual[temp1].value.integer)
	 {
	    /* check for reserved values */
	    case 0:
	    case 2:
	    case 8:
	    case 11:
	    case 13:
	     {
		err_printf("Reserved IRQ (0,2,8,11,13)\n");
		status = msg_found_conflict ;
	     }
	     break;

	    /* check if a negative value or a value greater that 15 */
	    default:
	     {
		if (( qual[temp1].value.integer < 0 ) ||
					   ( qual[temp1].value.integer > 15 ))
		{
		   err_printf("ERROR: IRQ must be between 0 and 15\n");
		   status = msg_found_conflict ;
	 	}
	     }
	 }

	 for ( temp2 = temp1+1 ; temp2 <= QIRQ3; ++temp2 ) 
	 {
	    if ( qual[temp2].present )
	    {
		if ( qual[temp1].value.integer == qual[temp2].value.integer ) 
		{
		  err_printf("Conflict between entered IRQs for this device\n");
		  status =  msg_found_conflict ;
		}
	    }
	 }
      }
   }

   if ( status != msg_success )
   {
      return (status);
   }


/*
 * Now compare QUAL structure to all devices in the ISACFG table.  This
 * is 16 items (4*4) times the number of devices in the table.
 */
   for ( entry = 0; entry < num ; entry++)
   {
      table = &isacfg_table->table.device[entry];
      qual_ptr = qual + QIRQ0;  /*SET TO 1ST IRQ ENTRY*/
      for ( temp1 = 0 ; temp1 < 4 ; ++temp1 ) 
      {
	 if ( table->device_enable ) /*Don't bother if device not enabled*/
	 {	 
	    if ( qual_ptr->present )
	    {
		if (table->irq0_assignment != unused)
		{
		   if ( qual_ptr->value.integer == table->irq0_assignment )
			status = msg_found_conflict ;
		}
		if (table->irq1_assignment != unused)
		{
		   if ( qual_ptr->value.integer == table->irq1_assignment )
			status = msg_found_conflict ;
		}
		if (table->irq2_assignment != unused)
		{
		   if ( qual_ptr->value.integer == table->irq2_assignment )
			status = msg_found_conflict ;
		}
		if (table->irq3_assignment != unused)
		{
		   if ( qual_ptr->value.integer == table->irq3_assignment )
			status = msg_found_conflict ;
		}
	    }  
	 }
	 if ( status == msg_found_conflict )
	 {
	    err_printf("ERROR: IRQ Conflict with %s\n", table->handle) ;
	    return(status);
	 }

	 ++qual_ptr ; /*POINT TO NEXT QUALIFIER*/
      }
   }

/*
 * NEXT WARN ABOUT ANY DMA CONFLICTS.
 */

/* Make sure QUAL stucture doesn't have any conflicts.  There are 6
 * distinct combinations given that you have 4 items taken 2 at a
 * time, order independant.
 */

   for ( temp1 = QDMACHAN0 ; temp1 <= QDMACHAN3 ; ++temp1 ) 
   {
      if ( qual[temp1].present )
      {
	 if ( ( qual[temp1].value.integer < 0 ) ||
	      ( qual[temp1].value.integer > 7 ) )
	 {
	    err_printf("DMA not between 0 to 7\n");
	    status = msg_found_conflict ;
	 }
	 else
	 {
	    for ( temp2 = temp1+1 ; temp2 <= QDMACHAN3 ; ++temp2 ) 
	    {
	       if ( qual[temp2].present )
	       {
		  if ( qual[temp1].value.integer == qual[temp2].value.integer ) 
		  {
		     err_printf("DMA Conflict\n") ;
		     status = msg_found_conflict ;
		  }
	       }
	    }
	 }
      }
   }

   if ( status != msg_success )
   {
      return (status);
   }

/*
 * Now compare QUAL structure to all devices in the ISACFG table.  This
 * is 16 items (4*4) times the number of devices in the table.
 */

   for ( entry = 0; entry < num ; entry++)
   {
      table = &isacfg_table->table.device[entry];
      qual_ptr = qual + QDMACHAN0 ;  /*SET TO 1ST DMA CHANNEL ENTRY*/
      for ( temp1 = 0 ; temp1 < 4 ; ++temp1 ) 
      {
	 if ( table->device_enable ) /*Don't bother if device not enabled*/
	 {
	    if ( qual_ptr->present )
	    {
	       if (table->dma0 != unused)
	       {
		  if ( qual_ptr->value.integer == table->dma0 )
		     status = msg_found_conflict ;
	       }
	       if (table->dma1 != unused)
	       {
		  if ( qual_ptr->value.integer == table->dma1 )
		     status = msg_found_conflict ;
	       }
	       if (table->dma2 != unused)
	       {
		  if ( qual_ptr->value.integer == table->dma2 )
		     status = msg_found_conflict ;
	       }
	       if (table->dma3 != unused)
	       {
		  if ( qual_ptr->value.integer == table->dma3 )
		     status = msg_found_conflict ;
	       }
	    }  
	 }  
	 if ( status == msg_found_conflict )
	 {
	    err_printf("ERROR: DMA Conflict with %s\n", table->handle) ;
	    return ( status ) ;
	 }
	 ++qual_ptr ;
      }
   }

return ( status ) ;
}



/*+
 * ===================================================
 * = checksum_header - Checksum the isa table header	  =
 * ===================================================
 *
/*
**++
** ========================================================
** =  conflict_warning 
** ========================================================
**
**  FUNCTIONAL DESCRIPTION:
**
**	Calculate the checksum of the specified data with a simple add
**	checksum scheme
**
**  FORMAL PARAMETERS:
**
**      cptr - pointer to data to checksum
**
**	length - number of bytes to checksum  
**
**  RETURN VALUE:
**
**	calculated checksum 
**
**  SIDE EFFECTS:
**
**      None
**
**
 *
--*/
unsigned long checksum_header(char *cptr, int length)
{
    int i;
    unsigned long sum = 0;

   for (i = 0; i < length-4; i++)
   {
       sum = (sum + cptr[i]) & 0xff;
   }
    return(sum);

}


/*+
 * ===================================================
 * = checksum_table - Checksum the entire isa table	  =
 * ===================================================
 *
**  FUNCTIONAL DESCRIPTION:
**
**	Calculate the checksum of the specified data with a simple add
**	checksum scheme (this provides the same functionality as the
**	checksum_header function)
**
**  FORMAL PARAMETERS:
**
**      cptr - pointer to data to checksum
**
**	offset - starting offset within the isa table
**
**  RETURN VALUE:
**
**	calculated checksum 
**
**  SIDE EFFECTS:
**
**      None
**
 *
--*/
unsigned long checksum_table(char *cptr, int offset)
{
    int i;
    unsigned long sum = 0;
    char *tptr = (char *)((int)cptr + offset);
    int length = isacfg_table->table.header.num_entries * 
				       sizeof(struct TABLE_ENTRY);

    for (i = 0; i < length; i++)
    {
       sum = (sum + tptr[i]) & 0xff;
    }
    return(sum);

}


/*+
 * ===================================================
 * = save_table - cause the existing table to be written out =
 * ===================================================
 *
**  FUNCTIONAL DESCRIPTION:
**
**	Save the existing isa table to nvram.
**
**  FORMAL PARAMETERS:
**
**	none
**	
**  RETURN VALUE:
**
**	success or failure
**
**  SIDE EFFECTS:
**
**      ISA Table written to non-volitile memory
 *
--*/
int save_table(void)
{
   unsigned char *cptr;
   int status, num_bytes, offset, page, num_pages ;
   struct FILE *local_fp ;

   status = msg_success ;

#if !CORTEX && !YUKONA
   cptr = &isacfg_table->string[0];
   isacfg_table->table.header.table_checksum = 
      checksum_table((char *)cptr,isacfg_table->table.header.first_entry_offset);
   isacfg_table->table.header.header_checksum = 
      checksum_header((char *)cptr,isacfg_table->table.header.first_entry_offset);

#if CABRIOLET || EB66P || EB164
   nvram_put( isacfg_table );
   return status ;
#endif
   
    if ((local_fp = fopen( "eerom", "r+" )))
    {
	fseek( local_fp, ISACFG_NVRAM_OFFSET, SEEK_SET );
	fwrite( isacfg_table, ISACFG_NVRAM_LENGTH, 1, local_fp );

	err_printf( TYPE_INIT ) ;
	fclose(local_fp);
    }
    else
    {	
	status = msg_failure;
    }
#endif
    return (status);
}



/*+
 * ===================================================
 * = get_table - read the existing isa table from nvram	  =
 * ===================================================
 *
**  FUNCTIONAL DESCRIPTION:
**
**	Read and validate the isa table in nvram.  If the table is invalid for
**	it valid and write it out.
**
**  FORMAL PARAMETERS:
**
**	none (void list)
**
**  RETURN VALUE:
**
**	success or failure
**
**  SIDE EFFECTS:
**
**      Isa table is read in to memory and may cause new nvram data
 *
--*/
int get_table ()
{
   unsigned int status, result;
   unsigned char *cptr;
   struct FILE *local_fp ;

   status = msg_success;

#if !CORTEX && !YUKONA
   isacfg_table = (union isa_config_table *)malloc(ISACFG_NVRAM_LENGTH);  

/* Check the header string, table and header checksums */
/* ...if any are not valid, recreate table and save to NVR */

    if ((local_fp = fopen( "eerom", "r" )))
    {
	fseek( local_fp, ISACFG_NVRAM_OFFSET, SEEK_SET );
	fread( isacfg_table, ISACFG_NVRAM_LENGTH, 1, local_fp );
	fclose(local_fp);
    }
    else
    {	
	status = msg_failure;
    }
#endif

   result = strcmp(isacfg_table->table.header.identifier,"CNFGTBL");
   if (result != 0) 
   {
      status = msg_failure ;      
   }

/* Do header checksum.*/ 
   if ( status == msg_success )
   {
      cptr = &isacfg_table->string[0];
      result = checksum_header((char *)cptr,sizeof(struct TABLE_HEADER));
      if (result != isacfg_table->table.header.header_checksum) 
      {
         status = msg_failure;
      }	
   }

/* Do table checksum.*/ 
   if ( status == msg_success )
   {
      cptr = &isacfg_table->string[0];
      result = checksum_table((char *)cptr,
			    isacfg_table->table.header.first_entry_offset);
      if (result != isacfg_table->table.header.table_checksum)
      {
        status = msg_failure;
      }	
   }
   
   if ( status == msg_failure )
   {
#if !CORTEX  && !YUKONA
      err_printf ("\nERROR: ISA table corrupt!\n" ) ;
#endif
      init_table();
      status = save_table();
   }

   return status ;
}




/*+
 * ===================================================
 * = modify_entry - modify and existing isa table entry	  =
 * ===================================================
 *
**  FUNCTIONAL DESCRIPTION:
**
**	Modify an exiating isa table entry based on the qualifiers and
**	parameters parse by qscan.
**
**  FORMAL PARAMETERS:
**
**	qual - pointer to array of qualifiers from qscan
**
**	isacfg_table_entry - address entry in table to modify
**
**  RETURN VALUE:
**
**	none
**
**  SIDE EFFECTS:
**
**      The selected entry will contain new data.
 *
--*/

static void modify_entry(struct QSTRUCT qual[],
	     struct TABLE_ENTRY *isacfg_table_entry) 
{
int qualifier;
int i;

/* walk the qstruct to see which parameters need to be modified.  We start at 2
 * because isacfg_table_entry already is pointing to slot/dev and end at 
 * QQUALMAX since the last two qualifiers deal with adding and removing entries
 */

   for (qualifier = 2; qualifier < QQUALMAX; qualifier++)
   {
	if (qual[qualifier].present)
	{
	    switch (qualifier)
	    {
/* Every SLOT has an overall type that is associated with it */
/* This is the type for the SLOT...not for the assoicated devices */
 
		case QETYP:     isacfg_table_entry->entry_type =
						 qual[QETYP].value.integer;
				break;
				
/*The total number of devices is kept around to mark how many devices are   */
/*at this slot */
		case QTOTDEV:   isacfg_table_entry->total_devices =
						 qual[QTOTDEV].value.integer;
				break;

/*Support here is added for up to 6 banks of io addresses.  The reason is   */
/*that the registers on some boards are not contiguous		      */


		case QIOBASE0:  isacfg_table_entry->io_base_addr0 =
						 qual[QIOBASE0].value.integer;
				break;

		case QIOBASE1:  isacfg_table_entry->io_base_addr1 =
						 qual[QIOBASE1].value.integer;
				break;

		case QIOBASE2:  isacfg_table_entry->io_base_addr2 =
						 qual[QIOBASE2].value.integer;
				break;

		case QIOBASE3:  isacfg_table_entry->io_base_addr3 =
						 qual[QIOBASE3].value.integer;
				break;

		case QIOBASE4:  isacfg_table_entry->io_base_addr4 =
						 qual[QIOBASE4].value.integer;
				break;
	
		case QIOBASE5:  isacfg_table_entry->io_base_addr5 =
						 qual[QIOBASE5].value.integer;
				break;

/* In the case of devices with non contiguous ISA memory we have provided   */
/* up to 3 memory base registers.				           */


		case QMEMBASE0:  isacfg_table_entry->mem0_base_addr =
						 qual[QMEMBASE0].value.integer;
				break;

		case QMEMLEN0:  isacfg_table_entry->mem0_length =
						 qual[QMEMLEN0].value.integer;
				break;

		case QMEMBASE1:  isacfg_table_entry->mem1_base_addr =
						 qual[QMEMBASE1].value.integer;
				break;

		case QMEMLEN1:  isacfg_table_entry->mem1_length =
						 qual[QMEMLEN1].value.integer;
				break;

		case QMEMBASE2:  isacfg_table_entry->mem2_base_addr =
						 qual[QMEMBASE2].value.integer;
				break;

		case QMEMLEN2:  isacfg_table_entry->mem2_length =
						 qual[QMEMLEN2].value.integer;
				break;

/* In some cases, the BIOS rom takes up space in memory space.              */
/* a user may want to mark off where the rom is to avoid that space         */


		case QROMBASE:  isacfg_table_entry->rom_base_addr =
						 qual[QROMBASE].value.integer;
				break;

		case QROMLEN:  isacfg_table_entry->rom_length =
						 qual[QROMLEN].value.integer;
				break;

		case QENADEV:  isacfg_table_entry->device_enable=
						 qual[QENADEV].value.integer;
				break;

/* in the case of the DMA modes and channels, the values are kept in the    */
/* same longword, so you have to OR in the mode or channel depending        */
/* on what is chosen						        */

		case QDMACHAN0:  
		   isacfg_table_entry->dma0 = 
		      (isacfg_table_entry->dma0 & 0x7F000000) |
		      qual[QDMACHAN0].value.integer;
		   break;

		case QDMACHAN1: 
		   isacfg_table_entry->dma1 =
		      (isacfg_table_entry->dma1 & 0x7F000000) |
		      qual[QDMACHAN1].value.integer;
		   break;

		case QDMACHAN2: 
		   isacfg_table_entry->dma2 =
		      (isacfg_table_entry->dma2 & 0x7F000000) |
		      qual[QDMACHAN2].value.integer;
		   break;

		case QDMACHAN3:
		   isacfg_table_entry->dma3 =
		      (isacfg_table_entry->dma3 & 0x7F000000) | 
		      qual[QDMACHAN3].value.integer;
		   break;

/* IRQ values are straight assignments*/

		case QIRQ0:  
			     isacfg_table_entry->irq0_assignment =
						 qual[QIRQ0].value.integer;
			     break;
		case QIRQ1:  
			     isacfg_table_entry->irq1_assignment =
						 qual[QIRQ1].value.integer;
			     break;
		case QIRQ2:  
			     isacfg_table_entry->irq2_assignment =
						 qual[QIRQ2].value.integer;
			     break;
		case QIRQ3:  
			     isacfg_table_entry->irq3_assignment =
						 qual[QIRQ3].value.integer;
			     break;


/* copy the null terminated string to the handle field */

		case QHANDLE: 
		   for (i = 0; (i<16) && (qual[QHANDLE].value.string[i]);  i++)
		   {
		      isacfg_table_entry->handle[i] =
			(qual[QHANDLE].value.string[i]); 
		   }
		   if (i<16)
		      isacfg_table_entry->handle[i] = 0;  /*null out the */
							  /*last char   */
		   break;

	    }
	}
     }

}



/*+
 * ===================================================
 * = delete_entry - deletes an entry from the config table   =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine deletes an entry at the slot/dev that is
 *     specified. and then compresses the table
 *  
**  FORMAL PARAMETERS:
**
**	victim - entry number to remove
**
**  RETURN VALUE:
**
**	none
**
**  SIDE EFFECTS:
**
**      The isa table is modified.
 *
--*/
static void delete_entry(int victim)
{
    int entry;
    int j;
    int last_entry;
    struct TABLE_ENTRY *isacfg_table_entry;
    struct TABLE_ENTRY *next_isacfg_table_entry;


    if (isacfg_table->table.header.num_entries == MAX_ENTRIES)
       last_entry = MAX_ENTRIES - 1;   /*Don't go past the edge, */
                                       /* we'll zap the last entry explicitly*/
    else 
       last_entry = isacfg_table->table.header.num_entries;   

    for ( entry = victim; entry < last_entry; entry++)
    {
	isacfg_table_entry = &isacfg_table->table.device[entry];
	next_isacfg_table_entry = &isacfg_table->table.device[entry+1];

	*isacfg_table_entry = *next_isacfg_table_entry;

	if (isacfg_table->table.device[entry+1].entry_type == ENTRY_NOT_USED)
	    break;
    }

    /* if the last entry is the end of the table, zap the entry explicitly*/
    if (last_entry == (MAX_ENTRIES - 1) )
       zap_entry(&isacfg_table->table.device[last_entry]);

   isacfg_table->table.header.num_entries--;
}


/*+
 * ===================================================
 * = zap_entry - reinits an entry from the config table   =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine deletes an entry at the slot/dev that is
 *     specified.
 *  
**  FORMAL PARAMETERS:
**
**	isacfg_table_entry - pointer to entry to reset
**
**  RETURN VALUE:
**
**	none
**
**  SIDE EFFECTS:
**
**      The isa table entry is modified.
 *
--*/

static void zap_entry(struct TABLE_ENTRY *isacfg_table_entry)
{
   uint64 i = 1;

   isacfg_table_entry->entry_type = ENTRY_NOT_USED ;
   isacfg_table_entry->isa_slot = 0;
   isacfg_table_entry->dev_num = 0;
   isacfg_table_entry->total_devices = 0;
   isacfg_table_entry->io_base_addr0 = i << 63;
   isacfg_table_entry->io_base_addr1 = i << 63;
   isacfg_table_entry->io_base_addr2 = i << 63;
   isacfg_table_entry->io_base_addr3 = i << 63;
   isacfg_table_entry->io_base_addr4 = i << 63;
   isacfg_table_entry->io_base_addr5 = i << 63;
   isacfg_table_entry->mem0_base_addr = i << 63;
   isacfg_table_entry->mem0_length = i << 63;
   isacfg_table_entry->mem1_base_addr = i << 63;
   isacfg_table_entry->mem1_length =  i << 63;
   isacfg_table_entry->mem2_base_addr = i << 63;
   isacfg_table_entry->mem2_length = i << 63;
   isacfg_table_entry->rom_base_addr = i << 63;
   isacfg_table_entry->rom_length = i << 63;
   isacfg_table_entry->device_enable = 0;
   isacfg_table_entry->dma0 = 1 << 31;
   isacfg_table_entry->dma1 = 1 << 31;
   isacfg_table_entry->dma2 = 1 << 31;
   isacfg_table_entry->dma3 = 1 << 31;
   isacfg_table_entry->irq0_assignment = 1 << 31;
   isacfg_table_entry->irq1_assignment = i << 31;
   isacfg_table_entry->irq2_assignment = i << 31;
   isacfg_table_entry->irq3_assignment = i << 31;

   for (i = 0; i < sizeof(isacfg_table_entry->handle); i++)
      isacfg_table_entry->handle[i] = '\0';
}



/*+
 * ===================================================
 * = init_table - no table so init the config table   =
 * ===================================================
 *
 * OVERVIEW:
 *  
 *     This routine inits the table if none exists.
 *  
**  FORMAL PARAMETERS:
**
**	None (void list)
**
**  RETURN VALUE:
**
**	none
**
**  SIDE EFFECTS:
**
**      The isa table is created.
 *
--*/

void init_table(void)
{
   struct TABLE_ENTRY *tp ;
   uint64 i = 1; /*fake out compiler for 64 bit quantities*/
   int j ;

   err_printf ( INITIALIZING ) ;

   /*init table header */

   isacfg_table->table.header.num_entries = NUM_EMBEDDED_ENTRIES;
   sprintf ( (char *)&isacfg_table->table.header.identifier,"%s","CNFGTBL") ;
   isacfg_table->table.header.major_version[0] = ISACFG_MAJOR_VERSION;
   isacfg_table->table.header.major_version[1] = 0;
   isacfg_table->table.header.minor_version[0] = ISACFG_MINOR_VERSION;
   isacfg_table->table.header.minor_version[1] = 0;
   isacfg_table->table.header.first_entry_offset = sizeof(struct TABLE_HEADER);
   isacfg_table->table.header.platform_specific_num_isa_slots = NUM_ISA_SLOTS;
      
   /*Mark everything as unused, then go fill in specifics*/

   for ( j=0 ; j < MAX_ENTRIES ; ++j )
   {
      zap_entry ( &isacfg_table->table.device[j] ) ;
   }

/*MOUSE*/
   tp = &isacfg_table->table.device[0] ;
   tp->entry_type = 2 ;
   tp->dev_num = 0 ;
   tp->io_base_addr0 = 0x60 ;
   tp->irq0_assignment = 12 ;
   sprintf ( tp->handle,"%s","MOUSE") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;

/*KEYBOARD*/
   tp = &isacfg_table->table.device[1] ;
   tp->entry_type = 2 ;
   tp->dev_num = 1 ;
   tp->io_base_addr0 = 0x60 ;
   tp->irq0_assignment = 1 ;
   sprintf ( tp->handle,"%s","KBD") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;

/*COM1*/
   tp = &isacfg_table->table.device[2];
   tp->entry_type = 2 ;
   tp->dev_num = 2 ;
   tp->io_base_addr0 = 0x3F8 ;
   tp->irq0_assignment = 4 ;
   sprintf ( tp->handle,"%s","COM1") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;


/*COM2*/
   tp = &isacfg_table->table.device[3];
   tp->entry_type = 2 ;
   tp->dev_num = 3 ;
   tp->io_base_addr0 = 0x2F8 ;
   tp->irq0_assignment = 3 ;
   sprintf ( tp->handle,"%s","COM2") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;


/*PARALLEL PORT */
   tp = &isacfg_table->table.device[4] ;
   tp->dev_num = 4 ;
   tp->entry_type = 2 ;
   tp->io_base_addr0 = 0x3BC ;
   tp->irq0_assignment = 7 ;
   sprintf ( tp->handle,"%s","LPT1") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;

/*FLOPPY */
#if !YUKONA
#if YUKON
if(floppy_att)
#endif
 {	
   tp = &isacfg_table->table.device[5] ;
   tp->dev_num = 5 ;
   tp->entry_type = 2; 
   tp->io_base_addr0 = 0x3f0 ;
   tp->irq0_assignment = 6 ;
   sprintf ( tp->handle,"%s","FLOPPY") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;
   tp->dma0 = 2 ;
 }
#endif

#if CORTEX || YUKONA
/* RT_TIMER */
   tp = &isacfg_table->table.device[6];
   tp->entry_type = 2 ;
   tp->dev_num = 6 ;
   tp->io_base_addr0 = DISCRETETIMER_BASE ;         /* 0x4000 for Cortex */
   tp->irq0_assignment = 10 ;  /* RT_TIMER phantom irq number for Unix */
   tp->irq1_assignment = 11 ;  /* PERIODIC_TIMER phantom irq number for Unix */
   sprintf ( tp->handle,"%s","RT_TIMER") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;
#endif


#if MONET
/*IDE */
   tp = &isacfg_table->table.device[6] ;
   tp->dev_num = 6 ;
   tp->entry_type = 2; 
   tp->io_base_addr0 = 0x1f0 ;
   tp->io_base_addr1 = 0x3f6 ;
   tp->io_base_addr2 = 0x170 ;
   tp->io_base_addr3 = 0x376 ;
   tp->irq0_assignment = 14 ;
   tp->irq1_assignment = 15 ;
   sprintf ( tp->handle,"%s","EIDE") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;

/*SOUND*/
   tp = &isacfg_table->table.device[7] ;
   tp->dev_num = 7 ;
   tp->entry_type = 2 ; 
   tp->io_base_addr0 = 0x220 ;
   tp->irq0_assignment = 5 ;
   sprintf ( tp->handle,"%s","ES1888") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 1 ;
   tp->dma0 = 1 ;
   tp->dma1 = 5 ;

/* USB Only the interrupt needs to be here.  Everything else is done by pci */
   tp = &isacfg_table->table.device[8] ;
   tp->dev_num = 8 ;
   tp->entry_type = 2 ; 
   tp->irq0_assignment = 10 ;
   sprintf ( tp->handle,"%s","USB") ;
   tp->total_devices = NUM_EMBEDDED_ENTRIES ;
   tp->device_enable = 0 ;		/* Turned off USB, was a 1 */

#endif
      
}

void isacfg_set_usb_enable(int control)
{
    struct TABLE_ENTRY *tp ;

    tp = &isacfg_table->table.device[8] ;
    if (control)
	tp->device_enable = 1 ;		/* Turn on USB */
    else
	tp->device_enable = 0 ;		/* Turn off USB */
}



/*+
 * ===================================================
 * = show_isacfg - display the curretn isa config data	  =
 * ===================================================
 *
**  FUNCTIONAL DESCRIPTION:
**
**	Displays the content of the isa table in a user readable format
**		
**  FORMAL PARAMETERS:
**
**      qual - pointer to qscan list if present
**
**	wildcard - display all/none
**
**  RETURN VALUE:
**
**	success or not found
**
**  SIDE EFFECTS:
**
**      None
**
 *
--*/

void show_isacfg(struct QSTRUCT *qual,int wildcard)
{
    unsigned long entry;
    int status = msg_isa_notfound;

    if (wildcard)
    {
	for ( entry = 0; entry < isacfg_table->table.header.num_entries; entry++)
	{
	    if (isacfg_table->table.device[entry].entry_type)
	    {
		dump_record(&isacfg_table->table.device[entry]);
	    }
	}
	status = msg_success;
    }
    else
    {
	for ( entry = 0; entry < isacfg_table->table.header.num_entries; entry++)
	{
	    if (isacfg_table->table.device[entry].entry_type)
	    {
		if ( qual[QDEV].present )
		{
		    if ((isacfg_table->table.device[entry].isa_slot == qual[QSLOT].value.integer) &&
			(isacfg_table->table.device[entry].dev_num == qual[QDEV].value.integer))
		    {
			dump_record(&isacfg_table->table.device[entry]);
			status = msg_success;
		    }
		}
		else
		{
		    if (isacfg_table->table.device[entry].isa_slot == qual[QSLOT].value.integer)
		    {
			dump_record(&isacfg_table->table.device[entry]);
			status = msg_success;
		    }
		}
	    }
	}
    }
    if (status != msg_success)
    {
	status = msg_isa_notfound;
	err_printf("ERROR: Device not found.\n");
    }
}

void show_isa_config()
{
    unsigned int old_slot;
    unsigned long entry;

    printf("\nISA\nSlot\tDevice\tName\t\tType\t     Enabled  BaseAddr\tIRQ\tDMA\n");
    old_slot = 99;

    for ( entry = 0; entry < isacfg_table->table.header.num_entries; entry++)
    {
	if (isacfg_table->table.device[entry].entry_type)
	{
	    dump_header(&isacfg_table->table.device[entry], old_slot);
	    old_slot = isacfg_table->table.device[entry].isa_slot;
	}
    }
}


static void dump_header(struct TABLE_ENTRY *isacfg_table_entry, unsigned int old_slot)
{
    int	count;
    int header_done = 0;
    int	line_printed;

    uint64 unused64 = 0x80000000;
    uint64  *io_base_addr;
    uint64  *mem_base_addr;
    unsigned long *dma_channel;
    unsigned long *irq_assignment;
        
    unused64 = unused64 << 32;

    if(isacfg_table_entry->isa_slot != old_slot) 
    { 
	  printf("%x\n",isacfg_table_entry->isa_slot);
    }

    printf("\t%x\t%s",isacfg_table_entry->dev_num,isacfg_table_entry->handle);
    if(strlen(isacfg_table_entry->handle) < 8)
    {
	printf("\t");
    }
    printf("\t%s\t%s\t",isa_type[isacfg_table_entry->entry_type], 
	isa_enable[isacfg_table_entry->device_enable]);

    io_base_addr = &isacfg_table_entry->io_base_addr0;
    mem_base_addr = &isacfg_table_entry->mem0_base_addr;
    dma_channel = &isacfg_table_entry->dma0;
    irq_assignment = &isacfg_table_entry->irq0_assignment;

    for (count = 0;  count < 6;  count++)
    {
	line_printed = 0;
	if (header_done)
	{
	    printf("\t\t\t\t\t\t\t");
	}

	if(*io_base_addr != unused64)
	{
	    printf("%x\t",*io_base_addr);
	    line_printed = 1;
	}
	else
	{
	    printf("\t");
	}
	io_base_addr++;
	
	if((count < 4) && (*irq_assignment != (unsigned long)0x80000000))
	{
	    printf("%d\t",*irq_assignment);
	    line_printed = 1;
	}
	else
	{
	    printf("\t");
	}
	irq_assignment++;
	
	if((count < 4) && (*dma_channel != (unsigned long)0x80000000))
	{
	    printf("%d",*dma_channel);
	    line_printed = 1;
	}
	dma_channel++;

	if (line_printed)
	{
	    printf("\n");
	}
	else
	{
	    printf("\r");   /* just reposition */
	}
	header_done = 1;
    }
}




/*+
 * ===================================================
 * = dump_record - [@short_overview@]	  =
 * ===================================================
 *
 * {@function_declaration@}
 *
 * RETURNS:
 *
 *      {@possible_returns@}
 *
 * ARGUMENTS:
 *
 *      {@auto_doc_argument@}
 *
 * {@globals_or_implicits@}
 *
 * [@registers_used@]
 *
 * FUNCTIONAL DESCRIPTION:
 *
 *      {@tbs@}...
 *
 * RETURN CODES:
 *
 *      {@possible_values_returned@}
 *
 * [@condition_values_returned_block@]
 *
 * CALLS:
 *
 *      {@possible_routines_called@}
 *
 * [@macros_used@]
 *
 * [@constants_used@]
 *
 * [@local_variables@]
 *
 * SIDE EFFECTS:
 *
 *      {@description_or_none@}
 *
 * ALGORITHM:
 *
 *    [@tbs@]...
 *
--*/
		      
static void dump_record(struct TABLE_ENTRY *isacfg_table_entry)
{
 int i;

 printf("\n\n=============================================================\n");
 printf("handle: %s\n",isacfg_table_entry->handle ) ;   
 printf("etyp: %x\n", isacfg_table_entry->entry_type ) ;      
 printf("slot: %x\n",isacfg_table_entry->isa_slot ) ;
 printf("dev: %x\n", isacfg_table_entry->dev_num ) ;
 printf("enadev: %x\n",isacfg_table_entry->device_enable ) ;
 printf("totdev: %x\n", isacfg_table_entry->total_devices ) ;

 printf("iobase0: %x   iobase1: %x\n",isacfg_table_entry->io_base_addr0,
	 isacfg_table_entry->io_base_addr1 ) ;
 printf("iobase2: %x   iobase3: %x\n",isacfg_table_entry->io_base_addr2,
	 isacfg_table_entry->io_base_addr3 ) ;
 printf("iobase4: %x   iobase5: %x\n",isacfg_table_entry->io_base_addr4,
	 isacfg_table_entry->io_base_addr5 ) ;

 printf("membase0: %x   memlen0: %x\n",isacfg_table_entry->mem0_base_addr,
	 isacfg_table_entry->mem0_length ) ;
 printf("membase1: %x   memlen1: %x\n",isacfg_table_entry->mem1_base_addr,
	 isacfg_table_entry->mem1_length ) ;
 printf("membase2: %x   memlen2: %x\n",isacfg_table_entry->mem2_base_addr,
	 isacfg_table_entry->mem2_length ) ;

 printf("rombase: %x   romlen:   %x\n",isacfg_table_entry->rom_base_addr,
	 isacfg_table_entry->rom_length);

    printf("dma0: %x\t\t",isacfg_table_entry->dma0);
    printf("irq0: %x\n",isacfg_table_entry->irq0_assignment);
    printf("dma1: %x\t\t",isacfg_table_entry->dma1);
    printf("irq1: %x\n",isacfg_table_entry->irq1_assignment);
    printf("dma2: %x\t\t",isacfg_table_entry->dma2);
    printf("irq2: %x\n",isacfg_table_entry->irq2_assignment);
    printf("dma3: %x\t\t",isacfg_table_entry->dma3);
    printf("irq3: %x\n",isacfg_table_entry->irq3_assignment);

 printf("=============================================================\n");
    
}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine can be called to get an entry from the table. 
**
**  FORMAL PARAMETERS:
**
**      handle:
**	  This is a string of the name of the device to search for 
**      ptr:
**	  This is a pointer to a structure passed in that is filled in.
**  RETURN VALUE:
**
**      {@description or none@}
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      {@description or none@}
**
**  [@logical properties@]...
**
**  [@optional function tags@]...
**
**--
*/

unsigned long isacfg_get_device(char *handle,int instance,struct TABLE_ENTRY *ptr)
{
int entry;
int j;	
int found;
struct TABLE_ENTRY *isacfg_table_entry;
int match;
    
    match = 0;
    found = 0;	

    for ( entry = 0; (entry < isacfg_table->table.header.num_entries) && (!found); entry++)
    {
	isacfg_table_entry = &isacfg_table->table.device[entry];
	if (strcmp(isacfg_table_entry->handle,handle) == 0)
	{
	    match++;
	    if (match == instance)
	    { 
		*ptr = *isacfg_table_entry;

		found = 1;
	    }
	}
    }
return(found);

}

/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine passes back the used IRQ
**
**  FORMAL PARAMETERS:
**
**      {@subtags@}
**
**  RETURN VALUE:
**
**      {@description or none@}
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      {@description or none@}
**
**  [@logical properties@]...
**
**  [@optional function tags@]...
**
**--
*/
int isacfg_get_irqs(char *irq_table)
{
int entry;
int j;	
struct TABLE_ENTRY *ptr;

    for ( entry = 0; entry < isacfg_table->table.header.num_entries; entry++)
    {
        ptr = &isacfg_table->table.device[entry];
	if (ptr->device_enable)
	{
	   if (ptr->irq0_assignment != (unsigned long)0x80000000)
	      irq_table[ptr->irq0_assignment & 0xf] = 1;   
	   if (ptr->irq1_assignment != (unsigned long)0x80000000)
	      irq_table[ptr->irq1_assignment & 0xf] = 1;
	   if (ptr->irq2_assignment != (unsigned long)0x80000000)
	      irq_table[ptr->irq2_assignment & 0xf] = 1;  
	   if (ptr->irq3_assignment != (unsigned long)0x80000000)
	      irq_table[ptr->irq3_assignment & 0xf] = 1;
	}
    }

return (msg_success) ;       
}
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      This routine turns off devices on the 87312 base on data
**	that is in the table.  We only look at the 4 items com1 com2
**	floppy and lpt1.
**
**  FORMAL PARAMETERS:
**
**      {@subtags@}
**
**  RETURN VALUE:
**
**      {@description or none@}
**
**  SIDE EFFECTS:
**
**      {@description or none@}
**
**  DESIGN:
**
**      {@description or none@}
**
**  [@logical properties@]...
**
**  [@optional function tags@]...
**
**--
*/
unsigned char isacfg_chk_local_devs()
{
struct TABLE_ENTRY temp;
unsigned long found;
unsigned char temp_fer = 0;

    found = isacfg_get_device("COM1",1,&temp);
    if (found && (temp.device_enable))
    {
	temp_fer |= 2;
    }
    found = isacfg_get_device("COM2",1,&temp);
    if (found && (temp.device_enable))
    {
	temp_fer |= 4;
    }
    found = isacfg_get_device("LPT1",1,&temp);
    if (found && (temp.device_enable))
    {
	temp_fer |= 1;
    }
    found = isacfg_get_device("FLOPPY",1,&temp);
    if (found && (temp.device_enable))
    {
	temp_fer |= 8;
    }

return(temp_fer);

}

