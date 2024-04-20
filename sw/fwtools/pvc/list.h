/* list.h */

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
 *	
 */

/* implementation of doubly-linked lists using a sentinel */

#ifndef LIST_H
#define LIST_H 1

#ifndef LIST_DEBUG
#define LIST_DEBUG 0
#endif

#include <stddef.h>
#include <stdlib.h>

/*
**  Base implementation
**
**    This code gives us access to the data structure fields
*/

/* list header */

typedef struct List {
  void *_head;
  void *_tail;
  int _length;
} List;

#define length_L(l) ((l)->_length)

#define head_L(l) ((l)->_head)

#define tail_L(l) ((l)->_tail)

#define create_L(l) {\
\
  l = (List *) malloc (sizeof (List));\
  if (LIST_DEBUG) printf("List head created @%x\n",l); \
\
  head_L(l) = l;\
  tail_L(l) = l;\
  length_L(l) = 0;\
}

/* list elements */

#define ListCommonFields  void *_next; void *_prev; int _killed

#define next_L(elt) ((elt)->_next)

#define prev_L(elt) ((elt)->_prev)

#define killed_L(elt) ((elt)->_killed)

/*
**  This code gives us added functionality and should be implemented
**  in terms of the above primitives- 
*/

#define mark_for_death(elt) killed_L(elt) = TRUE

#define add_L(l, type, pelt) {\
      if (LIST_DEBUG) printf("Entry %x added to list %x\n",pelt,l); \
      next_L(pelt) = next_L((type*)tail_L(l));\
      next_L((type*)tail_L(l)) = pelt;\
      prev_L(pelt) = tail_L(l);\
      tail_L(l) = pelt;\
      killed_L(pelt) = FALSE;\
      length_L(l)++;\
      /* scanfree(pelt); */\
}     
			    
#define for_L(l, type, elt) {\
   type *elt, *_temp;\
\
  for (elt = head_L(l); elt != (type*)l;) {

#define end_L(l, type, elt) \
\
  _temp = next_L(elt);\
  if (killed_L(elt)) remove_L(l, type, elt);\
  elt = _temp;\
  }\
}
   
#define end_nofree_L(l, type, elt) \
\
  _temp = next_L(elt);\
  if (killed_L(elt)) remove_nofree_L(l, type, elt);\
  elt = _temp;\
  }\
}

#define remove_L(l, type, pelt) {\
     if (LIST_DEBUG) printf("Entry %x removed/freed from list %x\n",pelt,l); \
     /* scanfree(pelt); */\
     next_L((type *)prev_L(pelt)) = next_L(pelt);\
     prev_L((type *)next_L(pelt)) = prev_L(pelt);\
     next_L(pelt)= (void *) -1;\
     prev_L(pelt)= (void *) -1;\
     free(pelt); /* shirron, remove this line, if you need to! */\
     length_L(l)--;  }

#define remove_nofree_L(l, type, pelt) {\
     if (LIST_DEBUG) printf("Entry %x removed from list %x\n",pelt,l); \
     /* scanfree(pelt); */\
     next_L((type *)prev_L(pelt)) = next_L(pelt);\
     prev_L((type *)next_L(pelt)) = prev_L(pelt);\
     length_L(l)--;  }

#define destroy_L(l, type) {\
\
  for_L(l, type, pelt) {\
    /* scanfree(pelt); */\
    mark_for_death(pelt);\
  } end_L(l, type, pelt);\
\
  free (l);\
  l=(List *)NULL;\
}

#define empty_L(l, type) {\
\
  for_L(l, type, pelt) {\
    /* scanfree(pelt); */\
    mark_for_death(pelt);\
  } end_L(l, type, pelt);\
}

#define debug_L(l,type,stag) \
for_L(l, type, dbgelt) \
 { \
  if (next_L(dbgelt) == dbgelt) printf("list is bogus %s %x\n",stag,dbgelt); \
  if (prev_L(dbgelt) == dbgelt) printf("list is bogus %s %x\n",stag,dbgelt); \
  if (next_L(dbgelt) == 0) 	printf("list is bogus %s %x\n",stag,dbgelt); \
  if (prev_L(dbgelt) == 0) 	printf("list is bogus %s %x\n",stag,dbgelt); \
  if (next_L(dbgelt) == -1) 	printf("list is bogus %s %x\n",stag,dbgelt); \
  if (prev_L(dbgelt) == -1) 	printf("list is bogus %s %x\n",stag,dbgelt); \
 } end_run_L(l, type, dbgelt);


#define end_run_L(l, type, dbgelt) \
\
  dbgelt = next_L(dbgelt);\
  } \
}



#endif /* LIST_H */

