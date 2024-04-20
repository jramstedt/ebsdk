/* pvc.c -- Command language interface */

/*
 *	
 *	Copyright (c) 1992
 *	Digital Equipment Corporation, Maynard, Massachusetts 01754
 *	
 *	This software is furnished under a license for use only on a single computer
 *	system  and  may be  copied  only  with the inclusion of the above copyright
 *	notice.  This software, or  any other copies thereof, may not be provided or
 *	otherwise made  available  to any other person except for use on such system
 *	and to one who agrees to these license terms.  Title to and ownership of the
 *	software shall at all times remain in DEC.
 *	
 *	The information  in  this software  is  subject to change without notice and
 *	should not be construed as a commitment by Digital Equipment Corporation.
 *	
 *	DEC  assumes no responsibility for the use or reliability of its software on
 *	equipment which is not supplied by DEC.
 *
 * V3.26 fls    5/15/97 add #ifdef EV6 for ebsdk which is shipping before ev6.
 * V3.25 fls    3/27/97	add set map_file command. if set map_file not done revert
 *			to previous version that opens file based on .exe filename.
 *			
 * V3.25 fls    3/27/97	Remove set check_one_permutation - the flag will
 *			be assumed always set (FLG$CHECKNOW). This flag forces the
 *			restriction checking to be done after each
 *			permutation. There is no reason to do all
 *			permutations at once as was done in earlier versions.
 * V3.24 fls	9/26/96	added set cpu ev6 aka 21264
 *			default cpu is now 21264
 *			chip->name now 21064/21164/21264.
 *
 * V3.23 fls	7/12/96 	PVC V3.23 Merged "VMS only" with EBSDK.
 *			(this module is EBSDK V3.22 version with only the
 *			 version startup messaged changed to V3.23)
 *
 * $Id: pvc.c,v 1.1 2000/08/09 17:02:25 rickg Exp $
 * $Log: pvc.c,v $
 * Revision 1.1  2000/08/09 17:02:25  rickg
 * Added support in the Makefile to build pvc, Initial checkin of pvc itself
 * pvc known to build on Linux/Alpha.
 * BUG(S) FIXED:	None
 * BACKEND RELATED: None
 *
 * Revision 1.1.1.1  1998/12/29 21:36:24  paradis
 * Initial CVS checkin
 *
 * Revision 1.9  1995/11/14  20:56:04  cruz
 * Added prototypes needed to eliminate warnings.
 *
 * Revision 1.8  1995/09/18  16:57:32  cruz
 * Added the ability to show instruction frequencies and to
 * check a flow right away, rather than at the end of parsing.
 *
 * Revision 1.7  1995/08/02  19:49:13  cruz
 * V3.21:  Added code for creating flow chart.  To enable,
 *          compile with RRC_COMPILE_FLOW_CODE defined.
 *
 * Revision 1.6  1995/07/27  20:06:37  cruz
 * V3.21:  1) Added code to detect recursive BSR loops.
 *         2) Modified code to check all flows of a special
 *            combination of branches and bsrs.
 *         3) Fixed problem that caused stack to be wrong.
 *         See sched.c for more information on the above changes.
 *
 * Revision 1.5  1995/05/23  22:10:19  cruz
 * V3.20:  In ev5.c, there were a few places where the error
 *         count was not being updated, even though there was
 *         an error.
 *         In body.c, I changed the print out of total count
 *         of warnings and errors to be a single number.
 *
 * Revision 1.4  1995/05/23  21:23:05  cruz
 * V3.19:  Fixed two logic bugs in the follow_jump routine. See sched.c
 *
 * Revision 1.3  1995/05/22  15:18:41  cruz
 * Changed the version number to reflect the following changes:
 * 	1) Increased the number of characters a PVC label can
 * 	   have.  See body.c
 * 	2) Increased the number of characters an entry point label
 * 	   can have.  See types.h
 * 	3) Fixed a bug in the checkforrevisit routine which may, in
 * 	   some instances, cause PVC to not detect that it has been
 * 	   thru a piece of code before.  See sched.c
 * 	4) Changed a malloc to a calloc in the routine for allocating
 * 	   memory cycles.  It was causing problems with uninitialized
 * 	   data when EV4 was selected.  See memory.c
 *
 * Revision 1.2  1995/04/21  15:12:58  samberg
 * Deleted references to ev4 and ev5, though set cpu ev4, ev5 still work.
 *
 *	
 *	
 *	REVISION HISTORY:		
 *	
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *      3.11	sgm	OCT-92		Fix minor bug in error reporting - bogus free caused problems.
 *	3	sgm	SEP-92		Version 3.0 - Rewrite subroutine support
 *	2	gpc	AUG-92		Modified to support EV5
 *	1     	gpc     JUN-92          Modified upon return to DEC
 *	0     	gpc     JAN-92          First Created
 *	
 */

/* #include "varargs.h" */
#include <signal.h>
#include <ctype.h>
#include "types.h"
#include "list.h"
#include "pvc.h"

#ifdef EV6
extern void ev6_init (Chip *c);
#endif
extern void ev5_init (Chip *c);
extern void ev4_init (Chip *c);
extern int palcode_violation_checker (List *, char *, char *);
extern int open_map_file (char *);

int 	flags = FLG$ERR | FLG$WARN | FLG$CHECKNOW, pal_base = 0, pal_end = 0x0fffffff, interrupt, delay = 5;
FILE 	*fd, *flow_fptr, *freq_fptr;
FILE 	*log_fptr;
Chip	*chip;
unsigned long	pal_link_address=0;



main ()
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To allow the user to look at the program's variables and change them.
 *		I know there is a more elegant way to implement this, but since 
 *		performance for this code is not an issue, this is the simplest design.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		clear_flag
 *		load_entries
 *		palcode_violation_checker
 *		set_file
 *		set_flag
 *		show_files
 *		show_flags
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
    {
    List 	*entry_list;
    char 	buf[128], *tokptr,*tmp_tokptr, *strptr = buf, *bufptr;
    static char 	exe_name[80]="", entry_name[80]="", log_name[80] = "", 
                        map_name[80] = "",flow_name[80] = "", freq_name[80] = "";

    char *p;
    int  val,  input_len = 0;
    int  status;
    int  allow_go;
#ifdef VMS
int keyb_id;
#endif

    static struct 	desc
	{
	int 	size;
	char 	*address;
	} prompt = {sizeof("PVC> ")-1, "PVC> "}, input_line = {128};
      
#ifdef VMS
    smg$create_virtual_keyboard (&keyb_id);
#endif
      
    fd = stdout;
    input_line.address = buf;
    signal (SIGINT, (void *) handle_int);
    create_L(entry_list);
    chip = (Chip *) malloc (sizeof (Chip));
    strcpy (chip->name, "");  /* RRC:  Initialize string name to NULL string.  */
  
    /*	 
    *  Get the information out of the user's logicals.
    */	 

    tokptr = getenv ("PVC$LINK");
    if (tokptr == NULL) tokptr = getenv ("PVC_LINK");
    if (tokptr)
	{
	sscanf(tokptr,"%x",&pal_link_address);
	printf("	PVC start address set to %x\n",pal_link_address);
	}
      
    tokptr = getenv ("PVC$PAL");
    if (tokptr == NULL) tokptr = getenv ("PVC_PAL");
    if (tokptr) set_file (tokptr, exe_name);

    tokptr = getenv ("PVC$MAP");
    if (tokptr == NULL) tokptr = getenv ("PVC_MAP");		/* V3.25 */
    if (tokptr) set_file (tokptr, map_name);

    tokptr = getenv ("PVC$ENTRY");
    if (tokptr == NULL) tokptr = getenv ("PVC_ENTRY");
    if (tokptr)
	{
	set_file (tokptr, entry_name);
	load_entries(entry_name, entry_list);
	}


    tokptr = getenv ("PVC$FLAG");
    if (tokptr == NULL) tokptr = getenv ("PVC_FLAG");
    if (tokptr)
	{
	s_upper(tokptr);
	set_flag(tokptr);
	}


    tokptr = getenv ("PVC$LOG");
    if (tokptr == NULL) tokptr = getenv ("PVC_LOG");
    if (tokptr)
	{
	set_file (tokptr, log_name);
	log_fptr = fopen (log_name, "w");
	}
#ifdef RRC_COMPILE_FLOW_CODE
    tokptr = getenv ("PVC$FLOW");
    if (tokptr == NULL) tokptr = getenv ("PVC_FLOW");
    if (tokptr)
	{
	set_file (tokptr, flow_name);
	flow_fptr = fopen (flow_name, "w");
	}
#endif /* RRC_COMPILE_FLOW_CODE */
    tokptr = getenv ("PVC$FREQ");
    if (tokptr == NULL) tokptr = getenv ("PVC_FREQ");
    if (tokptr)
	{
	set_file (tokptr, freq_name);
	freq_fptr = fopen (freq_name, "w");
	}

    /*	 
    *  Print banner
    */	 
    printf ("\nPALcode Violation Checker V3.29 Sep 18,1997 \n");

    tokptr = getenv ("PVC$CPU");
    if (tokptr == NULL) tokptr = getenv ("PVC_CPU");
    if (tokptr)
	{
	s_upper(tokptr);
	set_cpu(tokptr,0);		/* V3.25  ,0 indicates PVC_CPU or PVC$CPU defined */
	}
    else
#ifdef EV6
	set_cpu ("21264",0);		/* V3.25  ,0 indicates PVC_CPU or PVC$CPU defined */
#else
	set_cpu ("21164",0);		/* V3.26  ,0 indicates PVC_CPU or PVC$CPU defined */
#endif
    /*	 
    *  MAIN Input and parsing loop.
    */	 
    printf ("\n");


   for (;;)
	{
	    
#ifdef VMS
	smg$read_composed_line (&keyb_id, 0, &input_line, &prompt, &input_len);
	buf[input_len] = 0;
	chop_space(buf);
#else
	printf ("\nPVC> ");
	gets(buf);
#endif
	    
	interrupt = 0;
	strptr = buf;
	    
	tokptr = strtok (strptr, " ");                  /* get pointer to first char in command */
	strptr = NULL; 
	if (tokptr != NULL)
	    {
	    s_upper(tokptr);
	    if (!strncmp (tokptr, "HELP", strlen(tokptr)) || !strncmp (tokptr, "MAN", strlen(tokptr)) )
		{                                                    
		tokptr = strtok (strptr, " ");		/* get next token in command line */
		if (tokptr != NULL)
		   {
			strptr = NULL; 
			s_upper(tokptr);
			if (!strncmp (tokptr, "COMMANDS", (strlen(tokptr) > 1) ? strlen(tokptr) : 1))
		   	    {			
#ifdef EV6
			        printf("  set cpu 21264             Check DECchip 21264 family.\n");
#endif
			        printf("  set cpu 21164             Check DECchip 21164 family.\n");
			        printf("  set cpu 21064             Check DECchip 21064 family.\n");
			        printf("\n");
			        printf("  set code_file	 pal.exe    PALcode executable. \n");
			        printf("  set map_file   pal.map    PALcode map file. \n");
			        printf("  set entry_file pal.entry  PALcode entry point addresses and names.\n");
			        printf("  set log_file   pal.log    Optional Log file. Use CLear log_file to close.\n");
			        printf("  set freq_file  pal.freq   Optional address usage count file.\n");
			        printf("\n");
			        printf("  go                        Check all PAL addresses in entry_file.\n");
			        printf("  do n			    Check PAL entry point at address n.\n");
			        printf("  exit			    Terminate PVC session. \n");
			        printf("\n");
			        printf("  set pal_base n	    Offset all PAL addresses by n. The default is 0.\n");
			        printf("  set flag x		    Set PVC flag x , enter HELP FLAGS for a list.\n");
			        printf("  show all                  Show files, cpu type, and flags set.\n");
			    }
			else if (!strncmp (tokptr, "FLAGS", (strlen(tokptr) > 1) ? strlen(tokptr) : 1))
			        {
			        printf("  No flag commands are required for a typical PVC run.\n");
			        printf("  The errors and Warnings flags are set by default.\n");
			        printf("\n");
			        printf("  set flag all		    set all flags.\n");
			        printf("           errors	    Display restriction errors.\n");
			        printf("           warnings	    Display restriction warnings and guidelines.\n");
			        printf("           permutations	    Report number of code paths.\n");
			        printf("           scheduled_code   Display instructions as cpu would execute them.\n");
			        printf("           dead_code	    Report code that is not reached.\n");
			        printf("           memory_usage	    Report address and cycle usage.\n");
			        printf("           cycle_count	    Report permutation cycle counts.\n");
			        printf("           trace_code	    Disassemble instructions for each permutation.\n");
			        printf("\n");
			        printf("  There is a Clear Flag command for each SEt Flag command.\n");
			        printf("  The SHow FLags command will display flags currently set.\n");
			        printf("\n");
				}
			else if (!strncmp (tokptr, "ENVIRONMENT_VARIABLES", (strlen(tokptr) > 1) ? strlen(tokptr) : 1))
				{
			        printf("  PVC environment variables.\n");
			        printf("\n");
			        printf("  PVC_PAL                   Executable file (pal.exe) \n");
			        printf("  PVC_MAP                   Map file (pal.map)\n");
			        printf("  PVC_ENTRY                 PALcode entry point file (pal.entry) \n");
			        printf("  PVC_LOG                   Log file (pal.log)\n");
			        printf("  PVC_CPU                   CPU type \n");
			        printf("\n");
			        printf("  Example command to set a variable under Unix:\n");
			        printf("    > setenv PVC_PAL ~fred/pvc/pal.exe\n");
			        printf("\n");
			        printf("  Example command to set a variable under Windows NT:\n");
			        printf("    > set PVC_PAL = a:pal.exe\n");
			        printf("\n");
			        printf("  Example command to set a variable under OpenVMS:\n");
			        printf("    > define PVC_PAL sys$login_device:[.pvc]pal.exe\n");
			        printf("\n");
				}

			else
			    printf ("	Invalid HELP command. Enter HELP COMMANDS.\n");

		   }
	        else {
		    printf("\n");
		    printf(" PVC is primarily used to check for Alpha Palcode violations.\n");
		    printf(" It can also be used to disassemble executable code (set flag trace) and\n");
		    printf(" display code as the cpu would execute it (set flag scheduled_code).\n");
		    printf("\n");
		    printf(" Here is a sample PVC run:\n");
		    printf("\n");
#ifdef EV6
		    printf("     PVC> set cpu 21264\n");
#endif
		    printf("     PVC> set code_file  pal.exe   \n");
		    printf("     PVC> set entry_file pal.entry\n");
		    printf("     PVC> set log_file  pal_pvc.log\n");
		    printf("     PVC> go\n");
		    printf(" 	.\n");
		    printf("     PVC> exit\n");
		    printf("\n");
		    printf(" For more help enter:\n");
		    printf("\n");
		    printf("    HELP Commands\n");
		    printf("    HELP Flags\n");
		    printf("    HELP Environment_variables\n");
	         }
		}
	    else if (!strncmp (tokptr, "SET", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		{
		tokptr = strtok (strptr, " ");
		strptr = NULL;
		if (tokptr != NULL) {
		s_upper(tokptr);
		if (!strncmp (tokptr, "FLAGS", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    if (tokptr) s_upper(tokptr);
		    set_flag (tokptr);
		    }
		    /* set EXE_FILE same as set CODE_FILE */
		else if (!strncmp (tokptr, "CODE_FILE", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, exe_name);
		     if (strcmp (exe_name, ""))		/* set code file done so make a default .map filename */
		        {
			     strcpy (map_name, exe_name);
  			      p = strrchr (map_name, '.');
			      p++;
			      strcpy (p, "map");
		        }
		    }
		    /* set EXE_FILE same as set CODE_FILE */
		else if (!strncmp (tokptr, "EXE_FILE", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, exe_name);
		     if (strcmp (exe_name, ""))		/* set code file done so make a default .map filename */
		        {
			     strcpy (map_name, exe_name);
  			      p = strrchr (map_name, '.');
			      p++;
			      strcpy (p, "map");
		        }
		    }
		else if (!strncmp (tokptr, "MAP_FILE", (strlen(tokptr) > 2) ? strlen(tokptr) : 2)) /* V3.25 */
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, map_name);
		    }
		else if (!strncmp (tokptr, "CPU", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    tokptr = strtok (strptr, " ");
		    if (tokptr) s_upper(tokptr);
		    set_cpu (tokptr,1);		/* V3.25 ,1 indicates from command line*/
		    }
		else if (!strncmp (tokptr, "ENTRY_FILE", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, entry_name);
		    if (strcmp (entry_name, ""))
			load_entries(entry_name, entry_list);
		    }
		else if (!strncmp (tokptr, "LOG_FILE", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, log_name);
		    log_fptr = fopen (log_name, "w");
		    }
#ifdef RRC_COMPILE_FLOW_CODE
		else if (!strncmp (tokptr, "FLOW_FILE", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, flow_name);
		    flow_fptr = fopen (flow_name, "w");
                    flags |= FLG$FLOW;
		    }
#endif /* RRC_COMPILE_FLOW_CODE */
		else if (!strncmp (tokptr, "FREQ_FILE", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    set_file (tokptr, freq_name);
		    freq_fptr = fopen (freq_name, "w");
                    flags |= FLG$FREQ;
		    }
		else if (!strncmp (tokptr, "PAL_BASE", (strlen(tokptr) > 5) ? strlen(tokptr) : 5))
		    {
		    tokptr = strtok (strptr, " ");
		    if (!tokptr) continue;
		    tmp_tokptr = getenv (tokptr);
		    if (tmp_tokptr == 0)
			{
			s_upper(tokptr);
			tmp_tokptr = getenv (tokptr);
			}
		    if (!tmp_tokptr) tmp_tokptr = tokptr;
		    pal_base = strtol (tmp_tokptr, NULL, 16);
		    printf ("	PAL base noted. All entry points will be displaced from that offset.\n");
		    }
		else if (!strncmp (tokptr, "PAL_END", (strlen(tokptr) > 5) ? strlen(tokptr) : 5))
		    {
		    tokptr = strtok (strptr, " ");
		    if (!tokptr) continue;
		    tmp_tokptr = getenv (tokptr);
		    if (tmp_tokptr == 0)
			{
			s_upper(tokptr);
			tmp_tokptr = getenv (tokptr);
			}
		    if (!tmp_tokptr) tmp_tokptr = tokptr;
		    pal_end = strtol (tmp_tokptr, NULL, 16);
		    printf ("	PAL end noted. PVC won't look for dead code past that address.\n");
		    }
		else if (!strncmp (tokptr, "DELAY", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    tokptr = strtok (strptr, " ");
		    if (!tokptr) continue;
		    tmp_tokptr = getenv (tokptr);
		    if (tmp_tokptr == 0)
			{
			s_upper(tokptr);
			tmp_tokptr = getenv (tokptr);
			}
		    if (!(tmp_tokptr)) tmp_tokptr = tokptr;
		    delay = strtol (tmp_tokptr, NULL, 16);
		    }
		else if (!strncmp (tokptr, "PAL_LINK", (strlen(tokptr) > 5) ? strlen(tokptr) : 5))
		    {
		    tokptr = strtok (strptr, " ");
		    if (!tokptr) continue;
		    tmp_tokptr = getenv (tokptr);
		    if (tmp_tokptr == 0)
			{
			s_upper(tokptr);
			tmp_tokptr = getenv (tokptr);
			}
		    if (!tmp_tokptr) tmp_tokptr = tokptr;
		    sscanf (tmp_tokptr, "%x",&pal_link_address);
		    printf("	Link address set to %x.  All Map file addresses will be adjusted.\n",pal_link_address);
		    }
		else
		    printf ("	Invalid PVC command. Enter HELP COMMANDS.\n");
		} /* if (tokptr != NULL.. */
		else 
		    printf ("	Invalid PVC command. Enter HELP COMMANDS.\n");

		}
	    else if (!strncmp (tokptr, "CLEAR", strlen(tokptr)))
		{
		tokptr = strtok (strptr, " ");
		if (tokptr) s_upper(tokptr);
		strptr = NULL;
		if (!tokptr) {
		    printf ("	Invalid Clear command. Enter HELP COMMANDS.\n");
		}
		else if (!strncmp (tokptr, "FLAG", strlen(tokptr)))
		    {
		    tokptr = strtok (strptr, " ");
		    if (tokptr) s_upper(tokptr);
		    clear_flag(tokptr);
		    }
		else if (!strncmp (tokptr, "LOG_FILE", strlen(tokptr)))
		    {
		    strncpy (log_name, "", sizeof(log_name));
		    printf ("	Log file closed.\n");
		    fclose (log_fptr);
		    }
#ifdef RRC_COMPILE_FLOW_CODE
		else if (!strncmp (tokptr, "FLOW_FILE", strlen(tokptr)))
		    {
		    strncpy (flow_name, "", sizeof(flow_name));
		    printf ("	Flow file closed.\n");
		    fclose (flow_fptr);
                    flags &= ~FLG$FLOW;
		    }
#endif /* RRC_COMPILE_FLOW_CODE */
		else if (!strncmp (tokptr, "FREQ_FILE", strlen(tokptr)))
		    {
		    strncpy (freq_name, "", sizeof(freq_name));
		    printf ("	Freq file closed.\n");
		    fclose (freq_fptr);
                    flags &= ~FLG$FREQ;
		    }
		else
		    printf ("	Invalid Clear command. Enter HELP COMMANDS.\n");
		}
	    else if (!strncmp (tokptr, "SHOW", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		{
		tokptr = strtok (strptr, " ");
		strptr = NULL;	
		if (tokptr) s_upper(tokptr);
		if (!tokptr) {}
		else if (!strncmp (tokptr, "FLAGS", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    show_flags();
		    }
		else if (!strncmp (tokptr, "FILES", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		    {
		    show_files(exe_name, entry_name, log_name, map_name, freq_name);
		    }
		else if (!strncmp (tokptr, "ALL", strlen(tokptr)))
		    {
		    show_files(exe_name, entry_name, log_name, map_name, freq_name);
		    printf ("The CPU is set to %s.\n", chip->name);
		    show_flags();
		    show_palbase();			/* V3.25 */
		    }
		else if (!strncmp (tokptr, "CPU", strlen(tokptr)))
		    {
		    printf ("The CPU is set to %s.\n", chip->name);
		    }
		else if (!strncmp (tokptr, "ENTRIES", strlen(tokptr)))
		    {
		    int i=1;
                    for_L(entry_list, Entry, e)
			{
			printf ("#%3d:%6X\t%s\n", i++, e->offset, e->name);
			} end_L(entry_list, Entry, e);
		    }	    
		else
		    printf ("	Invalid Show command. Enter HELP FLAGS.\n");
		}
	    else if (!strncmp (tokptr, "DELETE", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		{
		int min, max, i=1;
		tokptr = strtok (strptr, "-");
		strptr = NULL;
		max = min = strtol (tokptr, NULL, 10);
		tokptr = strtok (strptr, " ");
		strptr = NULL;
		if (tokptr)
		    {
		    max = strtol (tokptr, NULL, 10);
		    if (max < min)
			{
			printf ("	Upper bound less than lower bound. Deleting only");
			printf (" entry %d.\n", min);
			max = min;
			}
		    }
		for_L(entry_list, Entry, e)
		    {
		    if (i >= min && i <= max) mark_for_death(e);
		    i++;
		    } end_L(entry_list, Entry, e);
		}		    		
	    else if (!strncmp (tokptr, "ADD", strlen(tokptr)))
		{
		Entry *ent = (Entry *) calloc (1, sizeof (Entry));
		  
		tokptr = strtok (strptr, " ");
		strptr = NULL;
		printf ("_address (in hex): ");
		gets (buf);
		val = strtol (buf, NULL, 16);
		printf ("_name: ");
		gets (buf);
		s_upper(buf);
		ent->offset = val;
		strncpy (ent->name, buf, sizeof(ent->name));
		add_L(entry_list, Entry, ent);
		}
	    else if (!strncmp (tokptr, "EXIT", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		{
		close_files();			/* V3.25 */
		exit (EXIT_SUCCESS);
		}
	    else if (!strncmp (tokptr, "QUIT", strlen(tokptr)))
		{
		close_files();			/* V3.25 */
		exit (EXIT_SUCCESS);
		}
	    else if (!strncmp (tokptr, "GO", strlen(tokptr)))
		{
		allow_go = 1;			/* assume entry and map files exist */
		if (strcmp (log_name, ""))
		    {
		    fd = log_fptr;
		    show_files(exe_name, entry_name, log_name, map_name, freq_name);
		    show_flags();
		    show_palbase();			/* V3.25 */
		    }
		fprintf (fd, "\n");			/* send cr to stdout or log_file */

		if (!strcmp (map_name, ""))		/* V3.25 if no set map_file command done */
		   {
		   if (strcmp (exe_name, ""))		/* but set code file done then make a .map filename */
		      {
			     strcpy (map_name, exe_name);
  			      p = strrchr (map_name, '.');
			      p++;
			      strcpy (p, "map");
		      }
		   }
		if (!strcmp (entry_name, ""))		/* if no .entryfile complain*/
		   {
		      printf ("	You must specify an entry file. \n");
		      printf ("	Use the SET ENTRY_FILE xxx.entry command.\n");
		      allow_go = 0;		/* don't allow GO*/
		   }

		if (!strcmp (exe_name, ""))		/* if no .exe file complain*/
		   {
		      printf ("	You must specify an executable file. \n");
		      printf ("	Use the SET CODE_FILE xxx.exe command.\n");
		      allow_go = 0;		/* don't allow GO*/
		
		   } 

		if (!open_map_file(map_name))
			allow_go = 0;         /* can't open map file;  don't allow GO*/
			
		if (allow_go)
		   {
	            status = palcode_violation_checker (entry_list, exe_name, map_name);
		    if (status)
		        {
		        fprintf (fd, "\nEnd of PALcode check...\n");
		        if (strcmp (log_name, ""))
		            fprintf (fd, "\nRun Completed.\n");  /* log file only */
        		fd = stdout;
		        }
		   }
	        }
	    else if (!strncmp (tokptr, "DO", (strlen(tokptr) > 2) ? strlen(tokptr) : 2))
		{
		tokptr = strtok (strptr, "");
		val = -1;
		if (!tokptr)
		    {
		    printf ("_address (if known): ");
		    gets (buf);
		    if (strcmp (buf, ""))
			val = strtol (buf, NULL, 16);
		    else
			val = -1;
		    printf ("_name: ");
		    gets (buf);
		    s_upper(buf);
		    }
		else
		    {
		    int isnum=TRUE;
			
		    for (bufptr = tokptr; *bufptr != '\0'; ++bufptr)
			if (!isxdigit (*bufptr))
			    isnum = FALSE;
		    if (isnum)
			{
			val = strtol (tokptr, NULL, 16);
			strncpy (buf, "", sizeof(buf));
			}
		    else
			{
			strncpy (buf, tokptr, sizeof(buf));
			s_upper(buf);
			}
		    }
		  
		if (val == -1)
		    {
		    for_L(entry_list, Entry, e)
			{
			if (buf != NULL)
			    {
			    if (!strcmp (e->name, buf))
				{
				val = e->offset;
				break;
				} 
			    }
			} end_L(entry_list, Entry, e);
		    }
		else if (!strcmp (buf, ""))
		    {
			
		    for_L(entry_list, Entry, e)
			{
			if (e->offset == val)
			    {
			    strncpy (buf, e->name, sizeof(buf));
			    break;
			    }
			} end_L(entry_list, Entry, e);
		    }
		  
		if (!strcmp (buf, ""))
		    strncpy (buf, "UNNAMED", sizeof(buf));
		if (val == -1)
		    printf ("	Can't find that name in the PAL entry list.\n");
		else
		    {
		    Entry *ent = (Entry *) calloc (1, sizeof(Entry));
		    List *one_and_only;
			
		    create_L(one_and_only);
		    ent->offset = val;
		    if (buf != NULL)
			strncpy (ent->name, buf, sizeof(ent->name));
		    add_L(one_and_only, Entry, ent);
		    if (strcmp (log_name, ""))
			{
			fd = log_fptr;
		        show_files(exe_name, entry_name, log_name, map_name, freq_name);
			show_flags();
		        show_palbase();			/* V3.25 */
			}
		    fprintf (fd, "\n");
		    if (!strcmp (map_name, ""))		/* V3.25 if no set map_file command done */
		       {
		       if (strcmp (exe_name, ""))		/* but set code file done then make a .map filename */
		          {
			     strcpy (map_name, exe_name);
  			      p = strrchr (map_name, '.');
			      p++;
			      strcpy (p, "map");
		          }
		       }

		allow_go=1;				/* assume ok to do  */
		if (!strcmp (exe_name, ""))		/* if no .exe file complain*/
		   {
		      printf ("	You must specify an executable file. \n");
		      printf ("	Use the SET CODE_FILE xxx.exe command.\n");
		      allow_go = 0;
		   } 

		if (!open_map_file(map_name))
			allow_go = 0;         /* can't open map file;  don't allow GO*/

		if (allow_go) 
		    {
		    status = palcode_violation_checker (one_and_only, exe_name, map_name);

		    if (status)
			{
		        fprintf (fd, "\nEnd of PALcode check...\n");
			fd = stdout;
			}
		    destroy_L(one_and_only, Entry);
		    }
		  }

		}   
	    else
		printf ("	Invalid PVC command. Enter HELP COMMANDS.\n");
	    }
	}
    return 0;			/* not executed but saves Windows NT warning */
    }


void  close_files()
/*
 ********************************************************************************
 *	PURPOSE:
 *		Close open files
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.25 FLS  4/2/97 
 ********************************************************************************
 */
{

    if (log_fptr) fclose (log_fptr);
#ifdef RRC_COMPILE_FLOW_CODE
    if (flow_fptr) fclose (flow_fptr);
#endif /* RRC_COMPILE_FLOW_CODE */
    if (freq_fptr) fclose (freq_fptr);

}


void show_flags()
/*
 ********************************************************************************
 *	PURPOSE:
 *		make main smaller
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *	V3.25 FLS  4/2/97 added debug_dump flag and changed format.
 *			     also  moved pal_base, pal_end, latency to 
 *			     show_palbase.
 ********************************************************************************
 */
{
      int count=0;
      char buf[256];
      strncpy (buf, "", sizeof(buf));
      if (flags & FLG$TRACE) {
	    fprintf (fd, "The %s flag is set.\n", "trace_code");
	    count++;
      }
      if (flags & FLG$SCHED2) {
	    fprintf (fd, "The %s flag is set.\n", "scheduled_code");
	    count++;
      }
      if (flags & FLG$WARN) {
	    fprintf (fd, "The %s flag is set.\n", "warnings");
	    count++;
      }
      if (flags & FLG$ERR) {	
	    fprintf (fd, "The %s flag is set.\n", "errors");
	    count++;
      }
      if (flags & FLG$CYCLES) {	
	    fprintf (fd, "The %s flag is set.\n", "cycle_count");
	    count++;
      }
      if (flags & FLG$PERM) {	
	    fprintf (fd, "The %s flag is set.\n", "permutations");
	    count++;
      }
      if (flags & FLG$DEAD) {	
	    fprintf (fd, "The %s flag is set.\n", "dead_code");
	    count++;
      }

      if (flags & FLG$DEBUG_DUMP) {	
	    fprintf (fd, "The %s flag is set.\n", "debug_dump");
	    count++;
      }

/* V3.25 SET FLAG DELAY this is not supported in EV5 and EV6 so will remove */
/*       displaying it, but it can still be set (for EV4) */   
 /* V3.25
  *     if (flags & FLG$DELAY) {	
  *	    fprintf (fd, "The %s flag is set.\n", "delay");
  *	    count++;
  *    }
  */
      if (flags & FLG$MEM) {	
	    fprintf (fd, "The %s flag is set.\n", "memory_usage");
	    count++;
      }
      if (count == 0) {
	    fprintf (fd, "No flags are set.\n");
      }

}



void show_files(char *exe_name, char *entry_name, char *log_name, char *map_name, char *freq_name)
/*
 ********************************************************************************
 *	PURPOSE:
 *		make main smaller
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{

      if (strcmp (exe_name, ""))
      	    	fprintf (fd, "The executable file is  %s\n", exe_name);
      if (strcmp (map_name, ""))
          	fprintf (fd, "The map file is         %s\n", map_name);
      if (strcmp (entry_name, ""))
      		fprintf (fd, "The entry point file is %s\n", entry_name);
      if (strcmp (log_name, "")) {
      	    fprintf (fd, "The log file is %s\n", log_name);
      }
      else {
	    fprintf (fd, "There is no log file specified.\n");
      }
      if (strcmp (freq_name, ""))
      		fprintf (fd, "The frequency file is   %s\n", freq_name);
}

void show_palbase()
/*
 ********************************************************************************
 *	PURPOSE:
 *		Show pal_base, pal_end
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.25 FLS  4/2/97 moved pal_base, pal_end, latency from show_flags
 ********************************************************************************
 */
{
      fprintf (fd,"The PAL base is %X.\n", pal_base);
      fprintf (fd,"The PAL end  is %X.\n", pal_end);
      if (pal_link_address != 0)
        fprintf (fd,"The PAL link address is %X.\n",pal_link_address);
      /* following is supported in ev4, but not ev5 and ev6 */
      if (flags & FLG$DELAY) fprintf (fd, "The latency for bubbles and cache misses is %X.\n", delay);
}


void clear_flag(char *tokptr)
/*
 ********************************************************************************
 *	PURPOSE:
 *		good programming style to isolate actions to functions
 *	INPUT PARAMETERS:
 *		tokptr: string containing the flag to clear
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      if (tokptr == NULL) {
	    printf ("	Invalid Clear Flag specified.  Enter HELP FLAGS.\n");
      }
      else if (!strncmp (tokptr, "TRACE_CODE", strlen(tokptr))) {
	    flags &= ~FLG$TRACE;
      }
      else if (!strncmp (tokptr, "SCHEDULED_CODE", strlen(tokptr))) {
	    flags &= ~FLG$SCHED2;
      }
      else if (!strncmp (tokptr, "RESTRICTIONS", strlen(tokptr))) {
	    flags &= ~FLG$REST;
      }
      else if (!strncmp (tokptr, "WARNINGS", strlen(tokptr))) {
	    flags &= ~FLG$WARN;
      }
      else if (!strncmp (tokptr, "ERRORS", strlen(tokptr))) {
	    flags &= ~FLG$ERR;
      }
      else if (!strncmp (tokptr, "CYCLE_COUNT0", strlen(tokptr))) {
	    flags &= ~FLG$CYCLES;
      }
      else if (!strncmp (tokptr, "PERMUTATIONS", strlen(tokptr))) {
	    flags &= ~FLG$PERM;
      }
      else if (!strncmp (tokptr, "MEMORY_USAGE", strlen(tokptr))) {
	    flags &= ~FLG$MEM;
      }
      else if (!strncmp (tokptr, "DEAD_CODE", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags &= ~FLG$DEAD;
      }
      else if (!strncmp (tokptr, "DEBUG_DUMP", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags &= ~FLG$DEBUG_DUMP;
      }
      else if (!strncmp (tokptr, "DELAY", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags &= ~FLG$DELAY;
      }
      else if (!strncmp (tokptr, "ALL", strlen(tokptr))) {
	    flags = 0;
      }
      else {
	    printf ("	Invalid Clear Flag specified.  Enter HELP FLAGS.\n");
      }
}



void set_file(char *source, char *dest)
/*
 ********************************************************************************
 *	PURPOSE:
 *		good programming style
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
    {
    char SOURCE[80];
    if (source == NULL)
	{
	printf ("	You must specify a filename\n");
	return;
	}

    strcpy (SOURCE, source);
    s_upper(SOURCE);
    while (getenv(SOURCE))
	{
	strcpy (source, getenv(SOURCE));
	strcpy (SOURCE, source);
	s_upper(SOURCE);
	}
    while (getenv(source)) 
	strcpy (source, getenv(source));
    strcpy (dest, source);
    }




void set_cpu (char *chip_name, int command) 
/*
 ********************************************************************************
 *	PURPOSE:
 *		allow the user to change which implementation of the Alpha 
 *		architecture he is checking with PVC
 *	INPUT PARAMETERS:
 *		chip_name: a string containing the implementation
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 *	V3.24 FLS modified to set chip names to 21064/21164/21264 instead
 *		  of EV4/EV5/EV6, for outside users.
 ********************************************************************************
 */
{

    if(chip_name != NULL) {

      if ( !strcmp (chip_name, "EV4") || !strcmp (chip_name, "21064") )
	{
	   if (command)
	      printf ("Cpu set to DECchip 21064 family.\n");
	   else
	      printf ("Default Cpu set to DECchip 21064 family.\n");
		      ev4_init (chip);
		      chip->initialized = FALSE;
		      strcpy (chip->name, "21064");
	}
      else if ( !strcmp (chip_name, "EV5") || !strcmp (chip_name, "21164") )
	{
	   if (command)
	      printf ("Cpu set to DECchip 21164 family..\n");
	   else
	      printf ("Default Cpu set to DECchip 21164 family.\n");
		      ev5_init (chip);
		      strcpy (chip->name, "21164");
		      chip->initialized = FALSE;
	}
#ifdef EV6
      else if ( !strcmp (chip_name, "EV6")  || !strcmp (chip_name, "21264")  )
	{
	   if (command)
	      printf ("Cpu set to DECchip 21264 family\n");
	   else
	      printf ("Default Cpu set to DECchip 21264 family.\n");
		      ev6_init (chip);
		      strcpy (chip->name, "21264");
		      chip->initialized = FALSE;
	}
#endif
      else
	      printf ("	Invalid CPU type. Enter HELP COMMANDS.\n");
    } /*if(chip_name != NULL)... */
    else   printf ("	You must specify a cpu type. Enter HELP COMMANDS.\n");
}



void set_flag(char *tokptr)
/*
 ********************************************************************************
 *	PURPOSE:
 *		make main smaller
 *	INPUT PARAMETERS:
 *		tokptr: a string containing the name of the flag to set
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      if (tokptr == NULL) {
	    printf ("	Invalid SET command. Enter HELP COMMANDS.\n");
      }
      else if (!strncmp (tokptr, "TRACE_CODE", strlen(tokptr))) {
	    flags |= FLG$TRACE;
      }
      else if (!strncmp (tokptr, "SCHEDULED_CODE", strlen(tokptr))) {
	    flags |= FLG$SCHED2;
      }
      else if (!strncmp (tokptr, "RESTRICTIONS", strlen(tokptr))) {
	    flags |= FLG$REST;
      }
      else if (!strncmp (tokptr, "WARNINGS", strlen(tokptr))) {
	    flags |= FLG$WARN;
      }
      else if (!strncmp (tokptr, "ERRORS", strlen(tokptr))) {
	    flags |= FLG$ERR;
      }
      else if (!strncmp (tokptr, "PERMUTATIONS", strlen(tokptr))) {
	    flags |= FLG$PERM;
      }
      else if (!strncmp (tokptr, "DEBUG_DUMP", strlen(tokptr))) {
	    flags |= FLG$DEBUG_DUMP;
      }
      else if (!strncmp (tokptr, "CYCLE_COUNT", strlen(tokptr))) {
	    flags |= FLG$CYCLES;
      }
      else if (!strncmp (tokptr, "MEMORY_USAGE", strlen(tokptr))) {
	    flags |= FLG$MEM;
      }
      else if (!strncmp (tokptr, "DEAD_CODE", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags |= FLG$DEAD;
	    printf ("	Remember to set pal_base and pal_end accordingly.\n");
      }
      else if (!strncmp (tokptr, "DEBUG_DUMP", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags |= FLG$DEBUG_DUMP;
      }
      else if (!strncmp (tokptr, "DELAY", (strlen(tokptr) > 3) ? strlen(tokptr) : 3)) {
	    flags |= FLG$DELAY;
      }
      else if (!strncmp (tokptr, "ALL", strlen(tokptr))) {
	    flags |= FLG$ERR | FLG$WARN | FLG$SCHED2 | FLG$TRACE | 
	      FLG$REST | FLG$PERM | FLG$CYCLES | FLG$DEAD | FLG$DELAY | FLG$MEM | FLG$CHECKNOW; 
      }
      else {
	    printf ("	Invalid SET command. Enter HELP COMMANDS.\n");
      }
}



int load_entries(char *entry_name, List *entry_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *		load a list of entry points into the palcode that we're looking at
 *	INPUT PARAMETERS:
 *		entry_name: the name of the entry points file
 *		entry_list: the list to put all the entries we find into
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
    int offset;
    char name[50];
    char buffer[128];
    FILE *entry_file;
      
    if ( (entry_file = fopen (entry_name, "r")) == NULL )
	{
	printf ("	Can't read the PAL entry points file.\n");
	printf ("	Use set entry_file xxx.entry command.\n");
	return(0);
	}     
    empty_L (entry_list, Entry);
    while ( fgets(buffer, 128, entry_file) != NULL )
	{
	if ((buffer[0] != ';') && (buffer[0] != '!') && (buffer[0] != '\n'))
	if (sscanf (buffer, "%x\t%s", &offset, name) == 2)
	    {
	    Entry *ent = (Entry *) calloc (1, sizeof(Entry));
	    
	    ent->offset = offset;
	    s_upper(name);
	    strncpy (ent->name, name, sizeof(ent->name));
	    add_L(entry_list, Entry, ent);
	    }
	else
	    printf("	Unable to parse entry file line <%s> in entry file.\n",buffer);
	}
    fclose (entry_file);
/* V3.25    printf ("Entry points loaded.\n"); */
    return(1);
    }



int wr (int origin, char *format, ...)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	        A general purpose print function that only prints when it has to.
 *		Depending on the flags that the user has enabled, some error messages
 *		will be suppressed, while others will be broadcasted.
 *	INPUT PARAMETERS:
 *	        The priority level of the message, and the message.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{
  va_list args;

  va_start (args, format);

  if (origin & flags & FLG$FREQ)
      vfprintf (freq_fptr, format, args);

#ifdef RRC_COMPILE_FLOW_CODE
  if (origin & flags & FLG$FLOW)
    vfprintf (flow_fptr, format, args);
  if (origin & flags & ~(FLG$FREQ | FLG$FLOW))
#else
  if (origin & flags & ~FLG$FREQ)
#endif /* RRC_COMPILE_FLOW_CODE */
    vfprintf (fd, format, args);

  va_end (args);
return(1);
}	



void handle_int (int x, ...)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	      This routine allows us to abort PVC's processing by pressing CTRL-C
 *	INPUT PARAMETERS:
 *		no parameters are necessary
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{
      interrupt = TRUE;
      signal (SIGINT, (void *) handle_int);
}


