/* memory.c */

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
 *	  0     gpc     AUG-92          First Created
 *	  1     fls	Jul-96		PVC V3.23 Merged "VMS only" with EBSDK.
 *			(this module is EBSDK V3.22 version with no changes)
 *	
 */
/*
**++
**  FACILITY:  memory management routines for PVC.
**
**  MODULE DESCRIPTION:
**
**      This module exists in order to create a wall of abstraction
**	between the memory management functions necessary and their
**	implementation. I believe it is good programming style to make
**	the scope of variables as small as reasonable.
**
**  AUTHORS:
**
**      Greg Coladonato
**
**  CREATION DATE:  6/24/92
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include "memory.h"


static NODE	*NODE_head;
static int	NODE_count, NODE_max;         

static CYCLE	*CYCLE_head;
static int	CYCLE_count, CYCLE_max;         
static int	CHUNK_max = NODE_CHUNK;

static Restriction *Rest_head;
static int Rest_count;         




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Mem_GrabBlock_*
**
**--
*/

void Mem_GrabBlock_NODE (void)
{

char *cp;
int i,j;    

    cp = getenv("NODE_CHUNK");
    if (cp != NULL)
        {
	i = 0;
        j = sscanf(cp, "%d", &i);
	if (j = 1)
	    {
	    CHUNK_max = i;
	    printf("NODE_CHUNK reset to %d\n",CHUNK_max);
	    }
        }

    NODE_head = (NODE *) malloc (CHUNK_max  * sizeof (NODE));
    if (!NODE_head)
	perror("Mem_GrabBlock_NODE: Malloc failed");
    NODE_count = 0;
    NODE_max = 0;
}

void Mem_GrabBlock_CYCLE (void)
{
    CYCLE_head = (CYCLE *) calloc (1, CYCLE_CHUNK  * sizeof (CYCLE));
    CYCLE_count = 0;
    CYCLE_max = 0;
}

#if 0
void Mem_GrabBlock_Restriction (void)
{
    Rest_head = (Restriction *) malloc (Rest_CHUNK  * sizeof (Restriction));
    Rest_count = 0;
}
#else
void Mem_GrabBlock_Restriction (void)
{
    Rest_head = (Restriction *) malloc (Rest_CHUNK  * sizeof (Restriction));
    Rest_count = 0;
}
#endif



/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Mem_FreeBlock_*
**
**--
*/


void Mem_FreeBlock_NODE (void)
{
    free(NODE_head);
}

void Mem_FreeBlock_CYCLE (void)
{
    free(CYCLE_head);
}

void Mem_FreeBlock_Restriction (void)
{
    free(Rest_head);
}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Mem_ResetCount_*
**
**--
*/

void Mem_ResetCount_NODE (void)
{
    NODE_max = (NODE_count > NODE_max) ? NODE_count : NODE_max;
    NODE_count = 0;
}

void Mem_ResetCount_CYCLE (void)
{
    CYCLE_max = (CYCLE_count > CYCLE_max) ? CYCLE_count : CYCLE_max;
    CYCLE_count = 0;
}

void Mem_ResetCount_Restriction (void)
{
    Rest_count = 0;
}   




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Mem_GetMax_*
**
**  RETURN VALUE:
**
**	The number of memory locations that we used up
**
**--
*/

int Mem_GetMax_NODE (void)
{
    NODE_max = (NODE_count > NODE_max) ? NODE_count : NODE_max;
    return (NODE_max);
}

int Mem_GetMax_CYCLE (void)
{
    CYCLE_max = (CYCLE_count > CYCLE_max) ? CYCLE_count : CYCLE_max;
    return (CYCLE_max);
}




/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**      Mem_Alloc_*
**
**  FORMAL PARAMETERS:
**
**	One argument corresponding to each of the fields in the structure
**	that we'd like to allocate.
**
**  RETURN VALUE:
**
**	A pointer to that structure, once it has been allocated and its
**	fields have been initialized.
**
**--
*/

NODE *Mem_Alloc_NODE (NODE *prev, NODE *inline, NODE *branch, List *jsr, INSTR *this) 
{
  NODE *new;
	
  
  new = malloc(sizeof (NODE));
  if (new == NULL)
    {
      printf ("\n\nNODE allocation failed.  Cannot allocate more memory.\n\n");
      exit(EXIT_FAILURE);
    }

  new->previous = prev;
  new->inline = inline;
  new->branch = branch;
  new->jump_dests = jsr;
  new->instruction = this;
  return (new);
}

void Mem_Free_NODE (NODE *node)
{
  free (node);
}	


CYCLE *Mem_Alloc_CYCLE (INSTR *is1, INSTR *is2)
{
      if (CYCLE_count < CYCLE_CHUNK-2) 
	{
	      CYCLE *new;
	      
	      new = CYCLE_head + CYCLE_count++;
	      new->is1 = is1;
	      new->is2 = is2;
	      return (new);
	}
      else 
	{
	printf ("\n\nCYCLE Overflow. Double the value of CYCLE_CHUNK in 'memory.h'.\n\n");
	exit(EXIT_FAILURE);
	}
}

#if 0
Restriction *Mem_Alloc_Restriction (int e, int p, int c, char *m)
    {
    Restriction *new;
    
    if (Rest_count < Rest_CHUNK-2) 
	{
	Rest_count = (Rest_count + 1);
	new = Rest_head + Rest_count;
	new->event = e;
	new->prohibited_event = p;
	new->cycles = c;
	new->msg = m;
	return (new);
	}
    else 
	{
	printf ("\n\nRestriction Overflow. Double the value of Rest_CHUNK in 'memory.h'.\n\n");
	exit(EXIT_FAILURE);
	}
    }

#else
Restriction *Mem_Alloc_Restriction (int e, int p, int c, char *m)
    {
    Restriction *new;
    
    if ( (new = malloc(sizeof(Restriction))) != NULL) 
	{
	new->event = e;
	new->prohibited_event = p;
	new->cycles = c;
	new->msg = m;
	return (new);
	}
    else 
	{
	printf ("\n\nFailed to allocate memory in Mem_Alloc_Restriction.\n\n");
	exit(EXIT_FAILURE);
	}
    }
#endif
