/* memory.h */

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
 *			(rest_chunk now 64000 was 32000.)
 *	
 */

#ifndef MEMORY_H
#define MEMORY_H 1

#include "types.h"

/* #define NODE_CHUNK	(1000000)    */
#define NODE_CHUNK	 (300000)
				    /*  This defines how many nodes we need */
				    /*  in our permutation tree */


#define CYCLE_CHUNK	(60000)	    /*  This is how long we expect a flow to run */
#define Rest_CHUNK	(64000)	    /*  This is the number of restrictions */

void Mem_GrabBlock_NODE (void);
void Mem_GrabBlock_CYCLE (void);
void Mem_GrabBlock_Restriction (void);

void Mem_FreeBlock_NODE (void);
void Mem_FreeBlock_CYCLE (void);
void Mem_FreeBlock_Restriction (void);

void Mem_ResetCount_NODE (void);
void Mem_ResetCount_CYCLE (void);
void Mem_ResetCount_Restriction (void);

int Mem_GetMax_NODE (void);
int Mem_GetMax_CYCLE (void);

Restriction *Mem_Alloc_Restriction (int, int, int, char*);
CYCLE *Mem_Alloc_CYCLE (INSTR *, INSTR *);
NODE *Mem_Alloc_NODE (NODE *, NODE *, NODE *, List *, INSTR *);

#endif   /*  MEMORY_H defined */
