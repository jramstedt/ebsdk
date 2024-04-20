/* rest.c -- Restriction handler module */

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
 *	
 *	REVISION HISTORY:		
 *	
 *	Rev	Who	When		What		
 *	----------------------------------------------------------------------
 *	V3.24	fls	Sep-96		Modify to support EV6
 *	2	gpc	AUG-92		Modifiy to support EV5
 *	1     	gpc     JUN-92          Updated and cleaned up
 *	0     	gpc     JAN-92          First Created
 *	
 */

#include "types.h"
#include "rest.h"
#include "memory.h"
#include <ctype.h>

#define MAX_RESTRICTIONS 1000
List *restrictions[MAX_RESTRICTIONS];

extern int 	interrupt, delay;
extern char 	pheader[];
extern CYCLE 	*runtime[];
extern List	*warnings, *errors, *exception_list;  
extern Restriction *Mem_Alloc_Restriction (int , int , int , char *);

void add_one_caveat (int address, int prohibited_event, int cycle, char *msg,  List *cav_list);


/*
 * Since so much of restriction checking is implementation dependent, this module
 * has been reduced to only the accounting functions of checking for restrictions.
 *     
 * All the interesting code will be found in ev4.c or ev5.c, or whatever.
 */



int not_ignored (int address, int errnum)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look at an address and see if it is one which the user has in his list
 *	        of allowable violations.
 *	INPUT PARAMETERS:
 *	        address: the pc of the instruction that is worthy of warning.
 *	        errnum: the number of the error that can be ignored.
 *	OUTPUT PARAMETERS:
 *	        return true if this error message has not been silenced by the user.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      int in_there = FALSE;
      
      /*	 
       *  Go through the list of exceptions and see if our address is in there.
       */	 
      for_L(exception_list, Exception, e) 
	{
	      if (e->addr == address && e->errnum == errnum) in_there = TRUE;
	} 
      end_L(exception_list, Exception, e);
      /*	 
       *  Depending on whether it is or not, return true or false.
       */	 
      if (!in_there) return (TRUE);
      else return (FALSE);
}



int new_warning (int address, int errnum)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look at an address and see if we have already issued a error or a
 *	        warning for it. If it's not in our list, add it.
 *	INPUT PARAMETERS:
 *	        address: the pc of the instruction that is worthy of warning.
 *	OUTPUT PARAMETERS:
 *	        return true if this address has not been caught yet.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		not_ignored
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      int in_there = FALSE;
      
      /*	 
       *  Go through the list of error addresses and see if our address is in there.
       */	 
      for_L(warnings, Address, a) 
	{
	      if (a->addr == address && a->data == errnum)
		{
		      in_there = TRUE;
		      break;
		}
	} end_L(warnings, Address, a);
      
      /*	 
       *  if it isn't, add it to our list.
       */	  
      if (!in_there) 
	{
	      Address *new_addr;
	      
	      new_addr = (Address *) malloc (sizeof (Address));
	      if (!new_addr) perror("new_warning: malloc failed");
	      new_addr->addr = address;
	      new_addr->data = errnum;
	      add_L(warnings, Address, new_addr);
	      if (exception_list && not_ignored (address, errnum))
		return (TRUE);
	      else
		return (FALSE);
	}
      else
	return (FALSE);
}



int new_error (int address, int errnum)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To look at an address and see if we have already issued a error or a
 *	        warning for it. If it's not in our list, add it.
 *	INPUT PARAMETERS:
 *	        address: the pc of the instruction that is worthy of warning.
 *	OUTPUT PARAMETERS:
 *	        return true if this address has not been caught yet.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		not_ignored
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */
{
      int in_there = FALSE;
      
      /*	 
       *  Go through the list of error addresses and see if our address is in there.
       */	 
      for_L(errors, Address, a) 
	{
	      if (a->addr == address && a->data == errnum) in_there = TRUE;
	} 
      end_L(errors, Address, a);
      
      /*	 
       *  if it isn't, add it to our list.
       */	  
      if (!in_there) 
	{
	      Address *new_addr;
	      
	      new_addr = (Address *) malloc (sizeof (Address));
	      if (!new_addr) perror("new_error: malloc failed");

	      new_addr->addr = address;
	      new_addr->data = errnum;
	      add_L(errors, Address, new_addr);
	      if (exception_list && not_ignored (address, errnum))
		return (TRUE);
	      else
		return (FALSE);
	}
      else
	return (FALSE);
}



int not_ignored_and_new_error (int address, int errnum)
/*
 ********************************************************************************
 *	PURPOSE: 
 *		To look at an address and see if we have already issued a error or a
 *		warning for it, and that the error is not to be ignored.
 *	INPUT PARAMETERS:
 *		address: the pc of the instruction that is worthy of warning.
 *	OUTPUT PARAMETERS:
 *		return true if this address has not been caught yet.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		not_ignored
 *	AUTHOR/DATE:
 *	Greg Coladonato, December 1991  
 ********************************************************************************
 */
    {
    /*	 
    *  Go through the list of error addresses and see if our address is in there.
    */	 
    for_L(errors, Address, a) 
	{
	if (a->addr == address && a->data == errnum) return(FALSE);
	} 
    end_L(errors, Address, a);
      

    /*	 
    *  Go through the list of exceptions and see if our address is in there.
    */	 
    for_L(exception_list, Exception, e) 
	{
	if (e->addr == address && e->errnum == errnum) return(FALSE);
	} 
    end_L(exception_list, Exception, e);


    return (TRUE);
    }



void remove_old_caveats(List *list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To clean out entries that have already expired.
 *	INPUT PARAMETERS:
 *	        A pointer to a list of things to watch out for
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	        The revised list, with each of the cycle counts decremented by one, and
 *	        the ones that were already at zero removed.
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{   
      for_L(list, Restriction, r) 
	{
	      if (r->cycles == 0) 
		{
		      mark_for_death(r);
		}
	      else 
		{
		      r->cycles--;
		}
	} end_L(list, Restriction, r);   
}






void kill_old_caveats(List *list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        To clean out all entries
 *	INPUT PARAMETERS:
 *	        A pointer to a list of things to watch out for
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	        The revised list, with each of the cycle counts decremented by one, and
 *	        the ones that were already at zero removed.
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
    {   
    for_L(list, Restriction, r) 
	{
	mark_for_death(r);
	} end_L(list, Restriction, r);   
    }



void add_caveat(int event, List *cav_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        This function searches through the restrictions array and pulls out all
 *	        restrictions associated with that event and puts them in the cav_list.
 *	INPUT PARAMETERS:
 *	        An event. We will look in restrictions[] to see if this ties up any resources.
 *	        A pointer to a list of events to watch out for.
 *
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	        the restrictions[] array.
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		mem_alloc_restriction
 *		wr
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */
{


#ifdef DEBUG	/* this looks like junk code and it caused EV5 to ACVIO, removed - SGM */
      if (restrictions[139]->_head < 5000) printf("Restrictions[139] corrupt.  Event %d\n",event);
#endif
      if (restrictions[event]) 
	{
	      for_L(restrictions[event], Restriction, r) 
		{
		      /*	 
		       *  Copy the information from the restriction structure to our caveat entry
		       */	 
		      Restriction *new_caveat;
		      new_caveat = Mem_Alloc_Restriction(r->event, r->prohibited_event, r->cycles, r->msg);
		      add_L(cav_list, Restriction, new_caveat);
		      if (cav_list->_head < (void *) 5000) printf("Add_L failed : add_caveat\n");
		      
		} end_L(restrictions[event], Restriction, r); 
	}
}	


ERR_MSG *search_ev6_caveat(int event_id, List *cav_list, int start_address)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Look through our ev6 styled caveat list and see if the given 
 * 		event id is in the 
 *	        prohibited_event field of any of the restrictions.
 *	INPUT PARAMETERS:
 *	        event_id    An event_id that we will search for occurences of.
 *	        cav_list:   A list of restrictions.
 *		start_addr: Found prohibited event must also be after this
 *			    address.
 *		The reason none of  the variable naming makes sense is that they are artifacts of
 *		EV4/EV5 related routines and structures.
 *
 *	OUTPUT PARAMETERS:
 *	        This routine returns a pointer to a structure containing:
 *			probs->errnum:      palcode address
 *			probs->explanation: address of decoded instruction such as HW_MTPR r1,ASN0.

 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96
 ********************************************************************************
 */

{
      for_L(cav_list, Restriction, r) 
	{
	      
	      if (r->prohibited_event == event_id && r->cycles == 0  &&  r->event > start_address ) 
		{
		      /*	 
		       *  Found a possible problem. Send  back palcode address and instruction decode address
		       */	 
		      ERR_MSG *probs = (ERR_MSG *) malloc (sizeof (ERR_MSG));
	       	      if (!probs) perror("search_caveat: malloc failed");

		      probs->errnum = r->event;
		      probs->explanation = r->msg;
		      return (probs);
		}
	} 
      end_L(cav_list, Restriction, r);	
      return (NULL);
}


ERR_MSG *search_addr_caveat(int event_id, List *cav_list, int address)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Look through our ev6 styled caveat list and see if the given 
 * 		event id is in the prohibited_event field of any of the restrictions.
 *		at the specified address.
 *	INPUT PARAMETERS:
 *	        event_id    An event_id that we will search for occurences of.
 *	        cav_list:   A list of restrictions.
 *		address: Found prohibited event must also be at this address.
 *		The reason none of  the variable naming makes sense is that they are artifacts of
 *		EV4/EV5 related routines and structures.
 *
 *	OUTPUT PARAMETERS:
 *	        This routine returns a pointer to a structure containing:
 *			probs->errnum:      palcode address
 *			probs->explanation: address of decoded instruction such as HW_MTPR r1,ASN0.

 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96
 ********************************************************************************
 */

{
      for_L(cav_list, Restriction, r) 
	{
	      
	      if (r->prohibited_event == event_id && r->cycles == 0  &&  r->event == address ) 
		{
		      /*	 
		       *  Found a possible problem. Send  back palcode address and instruction decode address
		       */	 
		      ERR_MSG *probs = (ERR_MSG *) malloc (sizeof (ERR_MSG));
	       	      if (!probs) perror("search_caveat: malloc failed");

		      probs->errnum = r->event;
		      probs->explanation = r->msg;
		      return (probs);
		}
	} 
      end_L(cav_list, Restriction, r);	
      return (NULL);
}


ERR_MSG *search_caveat(int event, List *cav_list, INSTR *inst)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Look through our caveat list and see if the given event in the the
 *	        prohibited_event field of any of the restrictions.
 *	INPUT PARAMETERS:
 *	        An event that we will search for occurences of.
 *	        A list of restrictions.
 *	OUTPUT PARAMETERS:
 *	        This routine returns a pointer to a structure containing an event number
 *	        and an explanation if the event is found in the list; NULL if it isn't.
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991
 ********************************************************************************
 */

{
      for_L(cav_list, Restriction, r) 
	{
	      
	      if (r->prohibited_event == event && r->cycles == 0 /* &&
		  not_ignored_and_new_error(inst->address,r->event) */) 
		{
		      /*	 
		       *  Found a problem. Send an ERR_MSG back to check_for_problems
		       */	 
		      ERR_MSG *probs = (ERR_MSG *) malloc (sizeof (ERR_MSG));
	       	      if (!probs) perror("search_caveat: malloc failed");

		      probs->errnum = r->event;
		      probs->explanation = r->msg;
		      return (probs);
		}
	} 
      end_L(cav_list, Restriction, r);	
      return (NULL);
}



void add_restriction (int event1, int event2, char *cyc, char *desc)
/*
 ********************************************************************************
 *	PURPOSE: 
 *	        To make the init_restrictions function smaller.
 *	INPUT PARAMETERS:
 *	        event1: The event setting the stage for a violation.
 *	        event2: the event causing the violation
 *	        cyc:    the number of cycles apart that these two events cannot happen
 *	        desc:   a description of the restriction
 *
 *     EV6 uses the array for different purposes as follows:
 *
 *			           EV4/EV5		   EV6
 *	event1:	aka event:      trigger event id	address of event in palcode
 *	event2: aka prohibited: prohib. event id	event id
 *	cyc:	aka cycles:	cycle count		NOT USED
 *	desc:	aka msg:	error msg		NOT USED
 *
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	        The restrictions array.
 *	IMPLICIT OUTPUTS:
 *	        The restrictions array with a new entry.
 *	AUTHOR/DATE:
 *	        Greg Coladonato, December 1991  
 ********************************************************************************
 */

{
      while (*cyc)
	{
	      Restriction *r;
	      
	      r = (Restriction *) malloc (sizeof(Restriction));
	      if (!r) perror("add_restriction: malloc failed");

	      r->event = event1;
	      r->prohibited_event = event2;
	      if (!isdigit(*cyc))
		{
		      fprintf (stderr, "Bad arg to add_restriction\n");
		      exit (0);
		}
	      r->cycles = (int) (*(cyc++) - '0');    
	      r->msg = (char *) malloc (strlen (desc) + 1);
	      if (!r->msg) perror("add_restriction: malloc failed");

	      strcpy (r->msg, desc);
	      if (restrictions[event1] == NULL) create_L(restrictions[event1]);
	      add_L(restrictions[event1], Restriction, r);
	}
}



void clean_restrictions (void)
/*
 ********************************************************************************
 *	PURPOSE:
 *		to be polite about memory use. When the user switches from one
 *		chip to another, deallocate the memory that used to be taken 
 *		up by all the restriction lists.
 *	INPUT PARAMETERS:
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	IMPLICIT OUTPUTS:
 *	AUTHOR/DATE:
 *	        Greg Coladonato, Aug 92
 ********************************************************************************
 */
{
      int i;
      
      for (i=0;i<MAX_RESTRICTIONS;i++)
	{
	      /* 
	       * This array should start out initialized to zero. Therefore, if there's
	       * anything in it, it must be from running a different implementation's
	       * init code. Clean out this array, being sure to free all malloc'ed memory
	       */
	      if (restrictions[i])
		{
		      for_L (restrictions[i], Restriction, r)
			{
			      free (r->msg);
			      mark_for_death (r);
			}
		      end_L (restrictions[i], Restriction, r);
		      
		      destroy_L (restrictions[i], Restriction);
		}
	}
}

void add_one_caveat (int address, int prohibited_event, int cycle, char *msg,  List *cav_list)
/*
 ********************************************************************************
 *	PURPOSE:
 *	        Puts the caller's caveat_list entry into the cav_list.
 *	INPUT PARAMETERS:
 *		event: id of event to add to the caveat list.
 *	        cav_list: A pointer to a list of events to watch out for.
 *
 *     EV6 uses the array for different purposes as follows:
 *
 *			           EV4/EV5		   EV6
 *	event1:	aka event:      trigger event id	palcode address of event instruction
 *	event2: aka prohibited: prohib. event id	event id
 *	cyc:	aka cycle:	cycle count		0
 *	desc:	aka msg:	error msg		instruction ascii decode at palcode address
 *
 *	OUTPUT PARAMETERS:
 *	IMPLICIT INPUTS:
 *	        the restrictions[] array.
 *	IMPLICIT OUTPUTS:
 *	FUNCTIONS CALLED DIRECTLY:
 *		mem_alloc_restriction
 *		wr
 *	AUTHOR/DATE:
 *	V3.24 FLS Sep-96 EV6
 ********************************************************************************
 */
{
	      /*	 
	       *  Copy the information from the caller structure to our caveat entry
	       */	 
	      Restriction *new_caveat;
	      new_caveat = Mem_Alloc_Restriction(address, prohibited_event, cycle, msg);
	      add_L(cav_list, Restriction, new_caveat);
	      if (cav_list->_head < (void *) 5000) printf("Add_L failed in rest.c add_one_caveat routine\n");
}	
