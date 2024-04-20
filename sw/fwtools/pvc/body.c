/* body.c -- main pal code routine */
/*
 *	
 *	Copyright (c) 1992, 1993
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
 *	
 *	REVISION HISTORY:		
 *	
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *	  4     fls	Jul-96		PVC V3.23 Merged "VMS only" with EBSDK.
 *			(this module is EBSDK V3.22 version with no changes)
 *	  3     pir     JAN-93		Further EV5 mods
 *	  2	gpc	AUG-92		Modifed to support EV5
 *	  1     gpc     JUN-92          Updated
 *	  0     gpc     JAN-92          First Created
 *	
 * $log$
 */

#ifdef VMS
#include <stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <ctype.h>

#include "types.h"
#include "list.h"
#include "body.h"
#include "memory.h"


extern void alpha_init_classes(void);
extern int fill_common_tables (void);
extern int wr (int, char *, ...);
extern void follow_code (NODE *);
extern void get_next_branch (NODE *, INSTR *next_flow[]);
extern int    parse_get_instr_idx(union INSTR_FORMAT *instr);
extern int isp_decompose(union squad *pcptr,union INSTR_FORMAT *ins,char *buffer);

INSTR 	*exe;
CYCLE 	*runtime[MAXRUNCYCLES];
int 	perm;
int	pal_warning_count;
int	pal_error_count;
Entry  *pal_entry;		/* pal_entry->next , pal_entry->prev, pal_entry->killed,pal_entry->offset and pal_entry->name */
char 	pheader[80];
List 	*warnings, *errors, *exception_list;
List 	*go_no_further, *bsr_stack;
FILE    *map_file;

extern int 	pal_base, pal_end, interrupt, flags;  
extern int	pal_link_address;
extern FILE 	*fd;
extern Chip	*chip;
extern int 	delay;

#ifdef RRC_COMPILE_FLOW_CODE
/* RRC:  Added this code for flow display */
extern int RRC_node;
extern void print_connect_line();
extern void print_current_instruction(NODE *current, char *box_str);
extern void print_message_in_box(char *message, char *box_str);
extern int print_current_instruction_with_arrow(NODE *current, char *box_str, int node);
#endif /* RRC_COMPILE_FLOW_CODE */
void check_current_path (int permutation);  /* RRC: Created this routine so it can be called WHILE a tree is being created */
extern void destroy_tree(NODE *root); 


int palcode_violation_checker (List *entry_list, char *exe_name, char *map_name)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This is the main loop of the PALcode violation checker. In pseudo-code,
 *	        it does the following:
 *	
 *	        initialize tables and variables
 *	        read the raw executable into bytes[]
 *	        convert that information into array of instructions in exe[]
 *	        search the map file for violation exceptions
 *	        while there are entry points remaining	
 *	                put the code into a tree of instructions
 *	                while there are branches remaining on the tree	
 *	                        remove a branch
 *	                        schedule it (using dual-issue rules, latency matrix, scoreboard) 
 *	                        check it for PALcode violations
 *	
 *	INPUT PARAMETERS:
 *	        entry_list: a list of entry points that need to be checked. 
 *		exe_name: the name of the file with the executable in it.
 *	OUTPUT PARAMETERS:
 *		None
 *	IMPLICIT INPUTS:
 *		None
 *	IMPLICIT OUTPUTS:
 *		None
 *	FUNCTIONS CALLED DIRECTLY:
 *		alpha_init_classes
 *		ev4_init
 *		get_exe_map_files	renamed from get_exe_file v3.25
 *		loop_on_entry_list
 *		Mem_FreeBlock_*
 *		Mem_GetMax_*
 *		Mem_GrabBlock_*
 *			* = Cycle, Node, Restriction
 *		map_file
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, November 1991
 ********************************************************************************
 */
{
      int 	count, num_instrs;
      static int alpha_classes_initialized=0;
      
      /*   Initialize data structures  */	 
      if (length_L(entry_list) == 0) return(0);			/* V3.25 */
      
      /* These tables only have to be initialized once */
      if (!alpha_classes_initialized) 
	{
	      fprintf (fd, "Initializing Alpha dependent tables..\n");
	      alpha_init_classes();
	      alpha_classes_initialized = TRUE;
	      fill_common_tables();
	}
      
      /* 
       * Whenever the user changes the chip he's pointing at, we have to re-
       * initialize these tables
       */
      if (!chip->initialized) 
	{
	      fprintf (fd, "Initializing %s dependent tables..\n", chip->name);
	      (*chip->chip_init)();
	      chip->initialized = TRUE;
	}
      
      /*
       *  Allocate blocks of memory for the three different data structures, so that we can avoid
       *  doing numerous malloc's and free's, which are very time consuming. I imagine this is 
       *  because the C memory RTLs suck wind. (I looked at them. The VAXC rtl's _DO_ suck)
       */
/*  RRC:  Don't ask for this memory now.  Ask for one block at a time only when it's needed.
      Mem_GrabBlock_NODE();
*/
      Mem_GrabBlock_CYCLE();
      Mem_GrabBlock_Restriction();
      
      create_L(warnings);
      create_L(errors);
      
      /* Try to load the executable into the array exe[], as well as  read map file*/
      num_instrs = get_exe_map_files (exe_name, map_name);	/* V3.25 */
      if (num_instrs == 0) return (0);				/* V3.25 return error */
      
      /* For each permutation (branch tree) that the user wants to check, let 'er rip  */
      fprintf (fd, "Beginning PALcode check...\n");
      loop_on_entry_list ((List *) entry_list, num_instrs, exe_name);
      
      /* Go through the executable looking for code that was never reached  */
      if  ((FLG$DEAD & flags) || (FLG$FREQ & flags)) /* RRC: Or print out freq info if desired. */ 
	{
	      int stop;
	      int dead_found=FALSE;
	      stop = (pal_end/4 > num_instrs) ? num_instrs : pal_end/4;
	      
	      wr (FLG$DEAD, "\nLooking for dead code:\n");
	      for (count=pal_base/4; count<stop; count++) 
		{
		      if (!exe[count].reached && exe[count].bits.instr) 
			{
			      /* that is, if the instruction hasn't been reached and it's non-zero, */
			      wr (FLG$DEAD, "Addr: %4X\t%s\n", exe[count].address, exe[count].decoded);
			      dead_found=TRUE;
			}
                      else /* RRC:  Print out frequency info */
		      if (exe[count].reached && exe[count].bits.instr)
			{
			      wr (FLG$FREQ, "Addr: %4X \tFreq: %7u \t%s\n", exe[count].address, exe[count].reached, exe[count].decoded);
			}

		      if (interrupt) break;
		}
	      if (!dead_found)
	         wr (FLG$DEAD, "No dead code found\n");
	}
      
      /*
       * This information is printed out so that the user knows how big his data structures have to be
       * We don't compile this same accounting information for the Restrictions data block because
       * I don't think it will ever have to be increased. That belief is based on the assumption that
       * there is no way 500 new restrictions could caused before another died of old age. Translation:
       * restrictions don't last long enough to bump into this boundary. 
       */
      wr (FLG$MEM, "\nNode usage: %d.\n", Mem_GetMax_NODE());
      wr (FLG$MEM, "Cycle usage: %d.\n", Mem_GetMax_CYCLE());

/*  RRC:  Memory was freed when used.  Don't do this anymore.
      Mem_FreeBlock_NODE();
*/
      Mem_FreeBlock_CYCLE();
      Mem_FreeBlock_Restriction();
      
      destroy_L(errors, Address);
      destroy_L(warnings, Address);
      
      if (interrupt) 
	{
	      printf ("\nAborted by Control-C. Returning to prompt...\n\n");
	      interrupt = 0;
	}    
  return (1);	/* V3.25 */
}


NODE 	  parent;

void loop_on_entry_list (List *entry_list, int num_instrs, char *exe_name)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Run the heart of the PVC code on each of the entries
 *	INPUT PARAMETERS:
 *		entry_list: a list of the entries the user wants to have PVC check
 *		num_instrs: so we know how large the executable is, in case the
 *			user tells us to start outside the bounds of the code
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		ev4_check_restrictions
 *		follow_code
 *		get_next_branch
 *		Mem_ResetCount_*
 *		schedule_code
 *		wr
 *	AUTHOR/DATE:
 *		Greg Coladonato, July 92
 *	V3.24 FLS Sep-96  added chip specific init_pal_flow entry to be
 *			called at the beginning of each pal flow.
 *			This allows initing EV6 specific restrictions that
 *			span from pal entry points to end of flow.
 *			An example is the requirement for a HW_RET_STALL
 *			to return to native (non palmode) after HW_MTPR ASN0.
 ********************************************************************************
 */
{
      int offset;

      for_L(entry_list, Entry, e)
	{
	      if (!interrupt) 
		{
		      int     init_perm;
		      
		      parent.previous = (NODE *) NULL;
		      parent.inline_tree = (NODE *) NULL;
		      parent.branch = (NODE *) NULL;
		      parent.jump_dests = (List *) NULL;
		      parent.instruction = (INSTR *) NULL;
		      
		      offset = e->offset + pal_base;

		      if (offset > num_instrs*4) 
			{
			      printf ("\n	File %s doesn't contain address: %X \n ",exe_name, offset);
			      break;
			}
		      
		      /*
		       * go_no_further is a list of addresses. This list gives us a way to tell
		       * when we've looked far enough into a certain basic block. Once we've 
		       * gone 15 or 16 instructions into a basic_block, for all intents and purposes
		       * the instructions that became before this basic block are irrelevant -- 
		       * any violation in connection with them would have happened by now. Therefore,
		       * we only have to go past this threshhold once -- in the future we can stop
		       * right there. go_no_further is a list of address at which we can safely
		       * stop. The list is zeroed before we do each entry point and is destroyed
		       * afterwards
		       *   THIS FLAG IS NO LONGER SET IN ANY CODE I COULD FIND as V3.24.
		       */

		      pal_entry = e;			/* V3.24 save entry pointer in global  */

		      /*V3.24 call chip specific start of pal entry - not used by ev4/ev5 */
		      (*chip->at_pal_entry)();

		      /* print Checking the xxxxx routine, entry point nnnn: */
		      sprintf (pheader, "\nChecking the %s routine, entry point %X:", e->name, e->offset);

		      wr (FLG$TRACE + FLG$SCHED1 + FLG$BRANCH + FLG$SCHED2 + FLG$REST, "%s\n", pheader);
		      offset /= 4;
		      perm = 0;
		      
		      if ((!strcmp (e->name, "MCHK")) &&
			  (!strcmp (chip->name, "21064")))	/* EV4 */
			{
			      wr (FLG$WARN, "%s\n", pheader);
			      wr (FLG$WARN, "Make sure you don't HW_REI from this flow and land on an ");
			      wr (FLG$WARN, "MXPR. That\nwould be bad. Also, you better have checked ");
			      wr (FLG$WARN, "for outstanding traps.\n\n");
			      pal_warning_count++;
			}
#ifdef RRC_COMPILE_FLOW_CODE		     
                      { /* RRC:  Added this code for flow display */
                        char temp[PAGE_WIDTH+20];
                        RRC_node = 1;
                        sprintf (temp, "%s [0x%X]", e->name, e->offset);
                        print_message_in_box(temp, "*");
                      }
#endif /* RRC_COMPILE_FLOW_CODE */
		      parent.instruction = &exe[offset];
		      create_L(go_no_further);
		      
		      /*	 
		       *  hand this recursive function a pointer to the beginning of an empty tree
		       *  and it will create a tree pointed to by parent. This tree represents all the 
		       *  possible ways to traverse through the code. Of course, some assumptions were 
		       *  made, since we don't know how many times to go through loops just by looking
		       *  at the code staticly. 
		       */	 

		      Mem_ResetCount_NODE();
		      create_L(bsr_stack);
		      follow_code (&parent);
                      if (flags & FLG$CHECKNOW) /* RRC:  If this flag is set, then the tree has been checked already. */
                        destroy_tree(parent.inline_tree);  /* it can be destroyed. */

		      destroy_L(bsr_stack, Bsr_Stack);
		      init_perm = perm;
		      wr (FLG$TRACE, "A total of %d permutations were traced\n", perm);
		      wr (FLG$PERM, "There are %d permutations to the %s entry point.\n", perm, e->name);

		      /*	 
		       *  Schedule each of the branches individually. By schedule, I mean figure out the
		       *  timing they would have resulted in on the EV-4. 
		       */	 
		      while (perm > 0 && !interrupt && !(flags & FLG$CHECKNOW)) /* RRC: Moved this code to a separate routine to allow a different method of checking */
			{
                          check_current_path(init_perm - perm + 1);
                          perm--;
                        }
		      destroy_L(go_no_further, Address);

		      /*V3.24 call chip specific end of pal entry - not used by ev4/ev5 */
		      (*chip->at_pal_return)();

/* pal_warning_count is barely used in EV4 and none in EV5, even though some messages
   printed say "warning".  This is confusing to the user.  I'll print the combined
   output instead. RRC		      
		      wr (FLG$MEM, "Total Warnings: %d\n",pal_warning_count);
		      wr (FLG$MEM,"Total Errors: %d\n",pal_error_count);
*/
		      wr (FLG$MEM,"Total Errors & Warnings: %d\n",pal_error_count+pal_warning_count);
		      pal_warning_count=0;
		      pal_error_count=0;
		}

	} end_L(entry_list, Entry, e);
}

void check_current_path (int permutation)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Perform the scheduling and violation checking on a current path.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *		Greg Coladonato, July 92
 *              Rogelio R. Cruz 8/2/95 - This code used to be part of the
 *              loop_on_entry_list routine which would create an tree
 *              of ALL the possible code paths and then check for violations.
 *              For simple programs this approach was OK, but for larger ones,
 *              the tree would grow to big to fit in memory, thus I changed the
 *              approach to do the violation checking AFTER one path has been
 *              obtained.
 ********************************************************************************
 */
{

		      /*	 
		       *  Schedule each of the branches individually. By schedule, I mean figure out the
		       *  timing they would have resulted in on the EV-4. 
		       */	 

			{
			      INSTR 	*flow[MAXFLOW];
			      int 	cycle_total;

			      wr (FLG$PERM, "Checking Permutation #%d...\n", permutation);
			      get_next_branch (&parent, flow);
			      
			      /*	 
			       * Schedule the code and pass it to the restriction checker, WITHOUT
			       * regard for cache latency and branch bubbles. In this pass we'll check
			       * to make sure there is no unsafe use of load data.
			       */	 
			      Mem_ResetCount_CYCLE();
			      (*chip->schedule_code) (flow, FALSE, &cycle_total);
			      wr (FLG$CYCLES, "Permutation %d was %d cycles long (not counting latencies).\n", 
				  permutation, cycle_total);
			      Mem_ResetCount_Restriction();
			      (*chip->restriction_check)(cycle_total);
			      
			      /*	 
			       * Schedule the code and pass it to the restriction checker, taking care
			       * to simulate the cache latency and branch bubbles
			       */
			      if (delay != 0)
			      {
			      Mem_ResetCount_CYCLE();
			      (*chip->schedule_code) (flow, TRUE, &cycle_total);
			      wr (FLG$CYCLES, "Permutation %d was %d cycles long (taking latencies into account).\n",
				  permutation, cycle_total);
			      Mem_ResetCount_Restriction();
			      (*chip->restriction_check)(cycle_total);
			      }
			}

}


int get_exe_map_files (char *exe_name, char *map_name)
/*
 *******************************************************************************
 *	PURPOSE:
 *		Find the executable file and load its bits into a variable
 *	INPUT PARAMETERS:
 *		exe_name: a character string holding the name of the file the
 *			 executable (aka code_file) user wants PVC to check
 *		map_name: a character string holding the name of the map file.
 *
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		parse_get_instr_idx
 *		free_exe
 *	AUTHOR/DATE:
 *		Greg Coladonato, July 1992
 ********************************************************************************
 */
{
      FILE 	*exe_file;
      short int	*byte;
      struct stat file_info;
      int 	count;
      static int num_instrs;
      static ino_t last_file_loaded[3];
      
      /*
       * Read the executable file in exe_name
       * and put it into byte[]
       */

      if ( (exe_file = fopen (exe_name, "rb")) == NULL ) 	/* V3.25 */
	{
	      printf ("	Can't open executable file %s.\n",exe_name);
	      printf ("	Use the SET CODE_FILE xxx.exe command.\n");
	      return(0);				/* return num_instrs=0 to force error */
	}
      fstat (fileno(exe_file), &file_info);
      
      if (!memcmp (last_file_loaded, &file_info.st_ino, 3 * sizeof (ino_t)))
	{
	      /* We already loaded this same file */
	      /* Don't waste time by reloading it */
	      return (num_instrs);
	}
      
      if (exe && num_instrs)
        {
	      /* hopefully this means that we've already declared an exe block, */
	      /* because now I'm going to free it */
	      free_exe (num_instrs);
	}
      
      memcpy (last_file_loaded, &file_info.st_ino, 3 * sizeof (ino_t));
      byte = malloc ((file_info.st_size + 1) * sizeof(short int));    /* +1 needed to stop windows nt error at free (byte) - probably legit */
      fprintf (fd, "Disassembling executable...\n");
      /* fread (bytes, sizeof (byte), 1, exe_file); this isn't necessary on our size files  */
      for (count=0; (byte[count] = getc(exe_file)) != EOF; count++); 
      num_instrs = count/4;
      exe = malloc ((num_instrs + 1) * sizeof (INSTR));
      
      /*
       * Decode the bits and put all the information
       * about the instruction into exe[]
       */
      for (count=0; count<num_instrs; count++) 
	{
	      char tmpname[80];
	      exe[count].address = count*4;
	      exe[count].bits.instr = (byte[4*count] | 
				       byte[4*count+1] << 8 | 
				       byte[4*count+2] << 16 | 
				       (unsigned int)(byte[4*count+3]) << 24);
	      exe[count].index = parse_get_instr_idx(&(exe[count].bits));
	      exe[count].reached = 0;
	      exe[count].jsr_dest_list = NULL;
	      create_L(exe[count].bsr_history);
	      create_L(exe[count].bsr_stack_clone_list);
	      exe[count].use_bsr_stack = FALSE;
	      exe[count].PVC_Control_Flag = PVC_CTRL_CLEAR_FLAGS;
	      isp_decompose((union squad *)&exe[count].address, (union INSTR_FORMAT *)&exe[count].bits.instr, tmpname);
	      chop_space(tmpname);
	      exe[count].decoded = (char *) malloc ((strlen(tmpname) + 1) * sizeof(char));
	      if (!exe[count].decoded)
		{
		      printf ("	Not enough system memory to read in your code executable file. \n");
		      printf ("  	Suggestion: Check that your code executable file is a valid executable.. \n");
		      printf ("  		    If it is okay, you will have to reduce the size of your in\n");
		      printf ("  		    source code , before running PVC again.\n");
		      exit (0);
		}
	      strcpy (exe[count].decoded, tmpname);
	}
      free (byte);
      if (exception_list)
	/* 
	 * since exception_list is a global variable, it will start out as zero. If
	 * it's non-zero, there must be a list there, so we can feel safe destroying it
	 */
	destroy_L(exception_list, Exception);

      read_map_file (map_file);			/* V3.25 map file is now opened in pvc.c */
      fclose(exe_file);				/* V3.25 */      
      fclose(map_file);				/* V3.25 */      
      return (num_instrs);
}



void print_cycle (int org, int i)
/*
 *******************************************************************************
 *	PURPOSE:
 *	        This small function prints out the instructions in one cycle.
 *	INPUT PARAMETERS:
 *	        i: an index into the runtime[] array so we know which cycle to print
 *	        org: a value which designates what information is being printed.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:	wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{
    if (runtime[i] == NULL) {
	      /* This is more polite than crashing */
	      fprintf(stderr, "\n %%PVC_BUG in BODY.C->print_cycle -- function print_cycle given a null pointer\n\n");
		exit(1);
    }
      
    if (runtime[i]->is1 != NULL) {
	wr (org, "Cycle: %3d Addr: %4X %-30.30s", i, 
		  runtime[i]->is1->address, 
		  runtime[i]->is1->decoded);
	if (runtime[i]->is2 != NULL) {
	    wr (org, "\t\t%-.30s\n", runtime[i]->is2->decoded);
	    if (runtime[i]->is3 != NULL) {
		wr (org, "Cycle: %3d Addr: %4X %-30.30s", i, 
		      runtime[i]->is3->address, 
		      runtime[i]->is3->decoded);
	        if (runtime[i]->is4 != NULL) {
		     wr (org, "\t\t%-.30s\n", runtime[i]->is4->decoded);
		} else
		    wr (org, "\n");
	    }
	} else
	    wr (org, "\n");
    } else
	wr (org, "Cycle: %3d **Stall**\n", i);
}


int open_map_file (char *map_name)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Check that map file exists.

 *	INPUT PARAMETERS:
 *	        map_name: the name of the map file
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *	AUTHOR/DATE:
 *					can not run out of a directory named PVC, etc...)
 ********************************************************************************
 */
{
      
      if ( (map_file = fopen (map_name, "r")) == NULL ) 
	{
	      printf ("	Can't open map file %s.\n",map_name);
	      printf ("	Use the SET MAP_FILE xxx.MAP command.\n");
	      return (0);		
	}
      else    return (1);
}


int read_map_file (FILE *map_file)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look through a map file for symbols.
 *	INPUT PARAMETERS:
 *	        map_name: map file name
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		found_pvc_label
 *		process_goto_information
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 *	REVISIONS:
 *		SGM	15-SEP-1992	Map file parsing tweaked to look for PVC$
 *					in line rather than just PVC (otherwise PVC 
 *					can not run out of a directory named PVC, etc...)
 *	V3.25 Fls 28-Mar-1997 assumed open_map_file called 
 ********************************************************************************
 */
{
      List *computed_goto;
      char 	*rtwp, buf[300];
      
      create_L(exception_list);
      create_L(computed_goto);
      
	      
	      fprintf (fd, "Searching through map file for violation exceptions...\n");
/*v3.24 bug missed first .map entry	      fgets (buf, 300, map_file); */
/*v3.24 bug.. */			      s_upper(buf);

	      while (fgets (buf, 300, map_file) != (char *) NULL && 
		    (strstr (buf, "pvc$") == (char *) NULL) &&
		    (strstr (buf, "PVC$") == (char *) NULL))
		;
	      s_upper(buf);
	      rtwp = &buf[0];
	      while (rtwp != NULL) 
		{
		      if (strstr (buf, "SYMBOLS BY VALUE"))
		        {
			      /* some linkers include this info, too. Go no further. */
			      break;
			}
		      else if (strstr (buf, "PVC$")) 
			{
			      char 	*ptr = buf;
			      
			      ptr = strstr (ptr, "PVC$");
			      while (ptr != NULL) 
				{
				      ptr += 3;
				      found_pvc_label (ptr, computed_goto);
				      ptr = strstr (ptr, "PVC$");
				}		
			}	
		      rtwp = fgets (buf, 300, map_file);
		      s_upper(buf);
		}
      if (length_L(computed_goto)) process_goto_information(computed_goto); 
      destroy_L(computed_goto, GotoInfo);
      fprintf (fd, "\n");
   return (1);			/* V3.25 */
}



void process_goto_information (List *info_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	      This goes through the list that we build up in read_map_file corncering
 *	      computed gotos. All the elements of info_list fall into one of two
 *	      categories -- each is either a source or a destination. The first
 *	      loop of the code goes through and attaches each destination to the
 *	      corresponding source, subsequently deleting the destination from the 
 *	      list. The second phase is to go to each of the source addresses and 
 *	      imbed the information we've collected in the rest of the information.
 *	INPUT PARAMETERS:
 *	      info_list
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, June 92
 ********************************************************************************
 */
{	
      for_L(info_list, GotoInfo, i)
	{
	      if (!i->source) 
		{
		      for_L(info_list, GotoInfo, j) 
			{
			      if (j->source && i->set == j->set) 
				{
				      Address *a = (Address *) malloc (sizeof (Address));
				      
				      a->addr = i->address;
				      add_L(j->destinations, Address, a);
				      mark_for_death(i);
				      break;
				}	
			}
		      end_L(info_list, GotoInfo, j);
		}		
	} 
      end_L(info_list, GotoInfo, i);
      
      for_L(info_list, GotoInfo, i) 
	{
	      if (i->source) 
		{
		      exe[i->address/4].jsr_dest_list = i->destinations;
		      mark_for_death(i);
		}
	      else 
		{
		      printf ("(PVC #xx)  You are missing a computed goto PVC source label.\n");
		      printf ("	An example PVC computed goto sequence is: \n");
		      printf ("		pvc$label1$2100: 			! source label.\n");
		      printf ("			hw_jmp_stall (r3) 		! computed goto.\n");
		      printf ("		pvc$label1$2100.1: 			! destination label.\n");
		      printf ("	Suggestion: Find the  destination label that has no\n");
		      printf ("	source label associated with it.\n");
		}
	} 
      end_nofree_L(info_list, GotoInfo, i);
}



void found_pvc_label (char *ptr, List *computed_goto)
/*
 ********************************************************************************
 *	PURPOSE:
 *		parse a line that has a PVC$$ label on it and figure out what the
 *		palcode writer is trying to tell us.
 *	INPUT PARAMETERS:
 *		ptr: the string immediately following the second delimeter
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, July 92
 ********************************************************************************
 */
{                                                                                     
      char 	*found_ptr;                                                                
      int 	vio_num, addr_num;                                                          
      char 	vio[101], addr[20];
      
      sscanf (ptr, "%s %s", vio, addr);
      found_ptr = strchr (&vio[1], vio[0]);                                           
      found_ptr++;                                                                    
      sscanf (found_ptr, "%d", &vio_num);                                             
      sscanf (addr, "%X", &addr_num);
      addr_num = addr_num - pal_link_address;
                                                 
      if (vio_num >= 4000)                                                       
        {                                                                             
              /*                                                                      
               * This flag tells us that when we get to this instruction              
               * in the code following phase, to use the value on the                 
               * BSR stack to determine where to go next.                             
               */                                                                     
              exe[addr_num/4].use_bsr_stack = vio_num - 4000 + 1;                     
        }                                                                             
      else if (vio_num >= 2000)                                                       
        {                                                                             
              /*                                                                      
               * We have information about a computed                                 
               * goto here.                                                           
               */                                                                     
	      
              GotoInfo *i;                                                            
              i = (GotoInfo *) malloc (sizeof (GotoInfo));                            
              i->address = addr_num;                                                  
              i->set = vio_num - 2000;                                                
              found_ptr = strchr (found_ptr, '.');                                    
              if (found_ptr == NULL)                                                  
                {                                                                     
                      /* This is a jsr. */                                            
                      i->source = TRUE;                                               
                      create_L(i->destinations);                                   
                }                                                                     
              else                                                                    
                {                                                                     
                      /* This is a jsr destination */                                 
                      i->source = FALSE;                                              
                      i->destinations = NULL;                                         
                }                                                                     
              add_L(computed_goto, GotoInfo, i);                                      
        }                                                                             
      else                                                                            
        {                                                                             
              /*                                                                      
               * Otherwise, we have normal exception information.                     
               */                                                                     
	      
	      if (vio_num == 1008)
		{                                                                             
		      /*                                                                      
		       * This flag tells us not to follow a branch,                           
		       * because the code writer feels we don't                               
		       * need to.                                                             
		       */                                                                     
		      exe[addr_num/4].jsr_dest_list = (List *) TRUE;                                   
		}                                                                             
              else if (vio_num == 1009)
		{                                                                             
		      /*                                                                      
		       * This flag tells us not to follow a BSR branch,                           
		       * because the code writer feels we don't                               
		       * need to.  It will continue with the instruction
                       * following the BSR.
		       */                                                                     
		      exe[addr_num/4].PVC_Control_Flag |= PVC_CTRL_DONT_FOLLOW_BSR;
		}                                                                             
              else if (vio_num == 1020)
		{                                                                             
		      /*                                                                      
		       * This flag tells to stop the current permutation at the
                       * current instruction.
		       */                                                                     
		      exe[addr_num/4].PVC_Control_Flag |= PVC_CTRL_GO_NO_FURTHER;
		}                                                                             

	      else 
		{
		      
		      Exception *exp;                                                         
		      exp = (Exception *) malloc (sizeof (Exception));                        
		      exp->addr = addr_num;                                                   
		      exp->errnum = vio_num;                                                  
		      add_L(exception_list, Exception, exp);                                  
		      /* printf ("exc num %d addr %x \n", exp->errnum, exp->addr); */         
		}                                                                             
	}                                                                                     
}      



void free_exe (int len)
/*
 ********************************************************************************
 *	PURPOSE:
 *		In an effort to use memory more politely, this is called when 
 *		the user switches from one executable to another. In it, 
 *		we free() all the string allocated for holding the decoded
 *		instruction strings and also destroy_L() any lists that we
 *		created during prior runs through the code.
 *	INPUT PARAMETERS:
 *		len: the length of the executable code.
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int i;
      
      for (i=0; i<len; i++) 
	{
	      free (exe[i].decoded);
	      if (exe[i].bsr_history) destroy_L(exe[i].bsr_history, Bsr_History);
	      /* Need to destroy all bsr_stacks left on the bsr_stack_clone_list */
	      if (exe[i].bsr_stack_clone_list) {
		  for_L(exe[i].bsr_stack_clone_list,Bsr_Stack_List,b) {
		      if (b->bsr_stack)
		          destroy_L(b->bsr_stack,Bsr_Stack);
		  }
	          end_L(exe[i].bsr_stack_clone_list,Bsr_Stack_List,b);
                  destroy_L(exe[i].bsr_stack_clone_list, Bsr_Stack_List);
              }

	      if (exe[i].jsr_dest_list && exe[i].jsr_dest_list != (List *) 1)
		{
		      /* A list has been allocated for this variable. destroy it */
		      destroy_L (exe[i].jsr_dest_list, Address);
		}
	}
      
      free (exe);
}
