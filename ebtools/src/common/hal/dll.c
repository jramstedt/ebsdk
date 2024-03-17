/*
	The HAL Alpha Assembler is free software; you can redistribute 
	it and/or modify it with the sole restriction that this copyright 
	notice be attached.

	The information in this software is subject to change without notice 
	and should not be construed as a commitment by Digital Equipment
	Corporation.

	The author disclaims all warranties with regard to this software,
	including all implied warranties of merchantability and fitness, in
	no event shall he be liable for any special, indirect or consequential
	damages or any damages whatsoever resulting from loss of use, data, 
	or profits, whether in an action of contract, negligence or other
	tortious action, arising out of or in connection with the use or
	performance of this software.

*/
/***********************************************************************************/
/* FILE: DLL.C								           */
/* This file contains the implementation of specialized Dsect sorting functions.   */
/***********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dll.h"
#include "avl.h"
#include "hal.h"

#ifdef __STDC__
void *my_alloc(unsigned int size);
#else
void *my_alloc();
#endif

#undef ALLOC
#ifndef USE_MALLOC_FREE
#define ALLOC(type, num)	\
    ((type *) my_alloc(sizeof(type) * (num)))
#else
#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#endif

static dll *AllAddrUse, *AllAddrAvoid;

/*****************************************************************************/
/* DSECTFPC:								     */
/*  Calculate the FINAL address of the DSects				     */
/*****************************************************************************/
void dsectfpc(prev,item)
    PENTRY *prev;
    PENTRY *item;
{
  B64 al;

  if (item->link_address) {

    /* take care of offset */
    item->base.ulong[0] |= item->offset.ulong[0];
    item->base.ulong[1] |= item->offset.ulong[1];

    /* take care of alignment */
    al.ulong[1] = item->align>=32 ? 1<<(item->align-32) : 0;
    al.ulong[0] = item->align>=32 ? 0 : 1<<item->align;
    if (item->base.ulong[0] & (al.ulong[0] - 1)) {
      errout(E_WARN | E_NO_LNUM, "dsectfpc",
             "Dsect %s link address of %08x%08x was changed to obey its alignment argument of 2^%d\n",
             item->name,item->base.ulong[1],item->base.ulong[0],item->align);
      add64((int32*)item->base.ulong,(int32*)al.ulong,(int32*)item->base.ulong);
      item->base.ulong[0] &= ~(al.ulong[0] - 1);
    }

    /* write last pc in struct */
    if (item->link_size.ulong[1] || (item->link_size.ulong[0] > item->mpc.ulong[0]))
      add64((int32*)item->base.ulong, (int32*)item->link_size.ulong, (int32*)item->last.ulong);
    else
      add64((int32*)item->base.ulong, (int32*)item->mpc.ulong, (int32*)item->last.ulong);
  }
  else {

    /* figure out base address */
    if (!prev) {
      item->base.ulong[0] = Sa[0];
      item->base.ulong[1] = Sa[1];
    }
    else {
      item->base.ulong[1] = prev->last.ulong[1];
      item->base.ulong[0] = prev->last.ulong[0];
    }

    /* take care of offset */
    item->base.ulong[0] |= item->offset.ulong[0];
    item->base.ulong[1] |= item->offset.ulong[1];

    /* take care of alignment */
    al.ulong[1] = item->align>=32 ? 1<<(item->align-32) : 0;
    al.ulong[0] = item->align>=32 ? 0 : 1<<item->align;
    if (item->base.ulong[0] & (al.ulong[0] - 1)) {
      add64((int32*)item->base.ulong,(int32*)al.ulong,(int32*)item->base.ulong);
      item->base.ulong[0] &= ~(al.ulong[0] - 1);
    }
    
    /* write last pc in struct */
    if (item->link_size.ulong[1] || (item->link_size.ulong[0] > item->mpc.ulong[0]))
      add64((int32*)item->base.ulong, (int32*)item->link_size.ulong, (int32*)item->last.ulong);
    else
      add64((int32*)item->base.ulong, (int32*)item->mpc.ulong, (int32*)item->last.ulong);
    
  }

  dbgout("ReCalculating Dsect %s position ->Base(rgn:%d):[%08X %08X] End:[%08X %08X]\n",
         item->name,item->mem_region,item->base.ulong[1],item->base.ulong[0],item->last.ulong[1],item->last.ulong[0]);
}

void overlap_check(node)
     dll_node *node;
{
  PENTRY *prev = node->prev ? node->prev->value : NULL;
  PENTRY *next = node->next ? node->next->value : NULL;
  PENTRY *item = node->value;
  
  /* check for overlap errors with it's immediate surroundings */
  if (prev && !(item->link_attr & D_OVERLAP_OK) && item->mem_region==prev->mem_region &&
      (prev->last.ulong[1] > item->base.ulong[1] ||
       (prev->last.ulong[1] == item->base.ulong[1] && prev->last.ulong[0] > item->base.ulong[0]))) 
    errout(E_ERR | E_NO_LNUM, "OverlapCheck","Dsect '%s' overlaps previous Dsect '%s'\n",item->name,prev->name);
  
  if (next && !(item->link_attr & D_OVERLAP_OK) && item->mem_region==next->mem_region &&
      (item->last.ulong[1] > next->base.ulong[1] ||
       (item->last.ulong[1] == next->base.ulong[1] && item->last.ulong[0] > next->base.ulong[0]))) 
    errout(E_ERR | E_NO_LNUM, "OverlapCheck","Dsect '%s' overlaps next Dsect '%s'\n",item->name,next->name);

  /* Now check for conflicts with around "all region" sections */
  if (item->mem_region!=ALL_REGIONS && item->mem_region!=PHYSICAL_REGION) {
    dll_node *l;
    for (l = AllAddrUse->head; l && (l->value->base.ulong[1] < item->last.ulong[1] ||
                                     (l->value->base.ulong[1] == item->last.ulong[1] &&
                                      l->value->base.ulong[0] < item->last.ulong[0])); l = l->next)
      if (l->value->last.ulong[1] > item->base.ulong[1] ||
          (l->value->last.ulong[1] == item->base.ulong[1] && l->value->last.ulong[0] > item->base.ulong[0]))
        errout(E_ERR | E_NO_LNUM, "OverlapCheck","Dsect '%s' overlaps all region Dsect '%s'\n",item->name,l->value->name);
  }
  else if (item->mem_region==ALL_REGIONS) {
    dll_node *l;
    for (l = AllAddrAvoid->head; l && (l->value->base.ulong[1] < item->last.ulong[1] ||
                                       (l->value->base.ulong[1] == item->last.ulong[1] &&
                                        l->value->base.ulong[0] < item->last.ulong[0])); l = l->next)
      if (l->value->last.ulong[1] > item->base.ulong[1] ||
          (l->value->last.ulong[1] == item->base.ulong[1] && l->value->last.ulong[0] > item->base.ulong[0]))
        errout(E_ERR | E_NO_LNUM, "OverlapCheck","All region Dsect '%s' overlaps other Dsect(s). One being '%s'\n",
               item->name,l->value->name);
  }
}

/*
**  This function look thorugh a list of addresses (psects really ... it's ok if they overlap) to find
**  a free address range that's big enough to fit parameters provided (size and alignment).
**  The search will be bounded by the last address of the start section and
**  (optionally) the start address of the end section.
**  If none can be found, NULL is returned.
*/
dll_node* get_addr_bubble(list, start, end, upper_size, lower_size, align)
     dll *list;
     dll_node *start;
     dll_node *end;
     unsigned int upper_size;
     unsigned int lower_size;
     unsigned int align;
{
  dll_node *l;
  B64 bubble, adjusted_end_addr, adjusted_addr;
  
  /* First look to see if section can even fit between start and end */
  if (end) {
    adjusted_end_addr.ulong[1] = end->value->base.ulong[1];
    adjusted_end_addr.ulong[0] = end->value->base.ulong[0] & ~((1u<<page_size) - 1); /* bring down to next page boundary */
    sub64((int32*)adjusted_end_addr.ulong,(int32*)start->value->last.ulong, (int32*)bubble.ulong);
    if ((bubble.ulong[1] < upper_size) ||
        (bubble.ulong[1] == upper_size &&
         (bubble.ulong[0] < (lower_size + ((align+1) - (start->value->last.ulong[0] & align))))))
      return NULL; /* cannot fit */
  }

  /* Ok, has a chance to fit.
     Now go find 1st section in list that uses addresses after start's base */
  for(l=list->head; l && (l->value->last.ulong[1] < start->value->base.ulong[1] ||
                          (l->value->last.ulong[1] == start->value->base.ulong[1] &&
                           l->value->last.ulong[0] < start->value->base.ulong[0])); l = l->next)
    /* abort search if we go beyond end section */
    if (end && (l->value->base.ulong[1] > adjusted_end_addr.ulong[1] ||
                (l->value->base.ulong[1] == adjusted_end_addr.ulong[1] &&
                 l->value->base.ulong[0] >= adjusted_end_addr.ulong[0])))
      break;

  /* if such a section was not found, return start */
  if (!l || (end &&(l->value->base.ulong[1] > adjusted_end_addr.ulong[1] ||
                    (l->value->base.ulong[1] == adjusted_end_addr.ulong[1] &&
                     l->value->base.ulong[0] >= adjusted_end_addr.ulong[0]))))
    return start;

  /* Now look to see if we could fit the item between the start section and the one we just found in the list */
  adjusted_addr.ulong[1] = l->value->base.ulong[1];
  adjusted_addr.ulong[0] = l->value->base.ulong[0] & ~((1u<<page_size) - 1); /* bring down to next page boundary */
  sub64((int32*)adjusted_addr.ulong,(int32*)start->value->last.ulong, (int32*)bubble.ulong);
  if ((bubble.ulong[1] > upper_size) ||
      (bubble.ulong[1] == upper_size &&
       (bubble.ulong[0] >= (lower_size + ((align+1) - (start->value->last.ulong[0] & align))))))
    return start; 
  
  /* Found starting section ... look for big enough hole */
  for(; l->next && (end &&(l->value->base.ulong[1] < adjusted_end_addr.ulong[1] ||
                           (l->value->base.ulong[1] == adjusted_end_addr.ulong[1] &&
                            l->value->base.ulong[0] <= adjusted_end_addr.ulong[0]))); l = l->next) {
    /*check for overlap */
    adjusted_addr.ulong[1] = l->next->value->base.ulong[1];
    adjusted_addr.ulong[0] = l->next->value->base.ulong[0] & ~((1u<<page_size) - 1); /* bring down to next page boundary */
    if (adjusted_addr.ulong[1] < l->value->last.ulong[1] ||
        (adjusted_addr.ulong[1] == l->value->last.ulong[1] &&
         adjusted_addr.ulong[0] <= l->value->last.ulong[0])) 
      bubble.ulong[0] = bubble.ulong[1] = 0;
    else
      sub64((int32*)adjusted_addr.ulong,(int32*)l->value->last.ulong, (int32*)bubble.ulong);

    if ((bubble.ulong[1] > upper_size) ||
      (bubble.ulong[1] == upper_size &&
       (bubble.ulong[0] > (lower_size + ((align+1) - (l->value->last.ulong[0] & align))))))
    return l;
  }

  /* see if there room between the last l we had and the end section */ 
  if (!end) { /* no end, then just use highest VA */
    B64 last_va;
    last_va.ulong[1] = 0xffffffffu;
    last_va.ulong[0] = 0xffffffffu;
    sub64((int32*)last_va.ulong,(int32*)l->value->last.ulong, (int32*)bubble.ulong);
  }
  else { 
    /*check for overlap */
    if (l->next) {
      adjusted_addr.ulong[1] = l->next->value->base.ulong[1];
      adjusted_addr.ulong[0] = l->next->value->base.ulong[0] & ~((1u<<page_size) - 1); /* bring down to next page boundary */
      if (adjusted_addr.ulong[1] < l->value->last.ulong[1] ||
          (adjusted_addr.ulong[1] == l->value->last.ulong[1] &&
           adjusted_addr.ulong[0] <= l->value->last.ulong[0])) 
        bubble.ulong[0] = bubble.ulong[1] = 0;
      else
        sub64((int32*)adjusted_end_addr.ulong,(int32*)l->value->last.ulong, (int32*)bubble.ulong);
    }
    else
      sub64((int32*)adjusted_end_addr.ulong,(int32*)l->value->last.ulong, (int32*)bubble.ulong);
      
  }
  
  if ((bubble.ulong[1] > upper_size) ||
      (bubble.ulong[1] == upper_size &&
       (bubble.ulong[0] > (lower_size + ((align+1) - (l->value->last.ulong[0] & align))))))
    return l;

  return NULL;
}

/*
** Creates a dll and inits the structure
*/
dll *dll_init()
{
    dll *temp;

    temp = ALLOC(dll, 1);
    temp->head = NULL;
    temp->tail = NULL;
    
    return(temp);
}

/*
** Debug utility
*/
void dll_print(dll *list)
{
  dll_node *l;
  for (l=list->head; l!=NULL; l=l->next) 
    errout(E_INFO | E_NO_LNUM, "DLL", "%s [%0x %0x] - [%0x %0x]\n", l->value->name,
           l->value->base.ulong[1],l->value->base.ulong[0],l->value->last.ulong[1],l->value->last.ulong[0]);
}


/*
** Clean up dll's
*/
void dll_free_entry(node, value_free)
     dll_node *node;
     void (*value_free)();
{
  if (node == NULL)
    return;
    
  dll_free_entry(node->next, value_free);
  if (value_free != NULL)
    (*value_free)(node->value);
  FREE(node);
}
void dll_free(dead, value_free)
     dll* dead;
     void (*value_free)();
{
  dll_free_entry(dead->head, value_free);
  FREE(dead);
}

/*
** Inserts a node to the head of the list
*/
void dll_insert(list, value)
    dll *list;
    PENTRY *value;
{
    dll_node *temp;

    temp = ALLOC(dll_node, 1);
    temp->value = value;
    temp->prev = NULL;

    temp->next = list->head;
    if (list->head != NULL) {
        /* not first node */
        list->head->prev = temp;
    }
    else {
        /* first node */
        list->tail = temp;
    }

    list->head = temp;
}

/*
** Insert a node baseed on it's address.
** Always use superset section (i.e. remove subset sections) and overlaps don't matter
*/
void dll_addr_insert(list, item)
    dll *list;
    PENTRY *item;
{
    dll_node *temp, *after, *before;

    temp = ALLOC(dll_node, 1);
    temp->value = item;

    /* After = section with code above item 
       before = section with code below item
       Note that anything between before and after are subset sections */
    for (before = list->tail; before && (before->value->base.ulong[1] > item->base.ulong[1] ||
                                         (before->value->base.ulong[1] == item->base.ulong[1] &&
                                          before->value->base.ulong[0] > item->base.ulong[0])); before=before->prev)
      ;
    
    for (after = before; after && (after->value->last.ulong[1] < item->last.ulong[1] ||
                                   (after->value->last.ulong[1] == item->last.ulong[1] &&
                                    after->value->last.ulong[0] < item->last.ulong[0])); after=after->next)
      ;

    if (!before && !after) {  /* list is empty */
      list->head = temp;
      list->tail = temp;
      temp->prev = NULL;
      temp->next = NULL;
    }
    else if (!before) { /* Place at head */
      /* but first eliminate all subset sections */
      while(after->prev) {
        dll_node *l = after->prev;
        after->prev = l->prev;
        FREE(l);
      }
      list->head = temp;
      temp->next = after;
      temp->prev = NULL;
      after->prev = temp;
    }
    else if (!after) { /* place at tail */
      /* but first eliminate all subset sections */
      while(before->next) {
        dll_node *l = before->next;
        before->next = l->next;
        FREE(l);
      }
      list->tail = temp;
      temp->prev = before;
      temp->next = NULL;
      before->next = temp;
    }
    else { /* place inbetween before and after */
      /* but first eliminate all subset sections */
      while(after->prev != before) {
        dll_node *l = after->prev;
        after->prev = l->prev;
        FREE(l);
      }
      before->next = temp;
      after->prev = temp;
      temp->next = after;
      temp->prev = before;
    }
}
     
/*
** Move a node from one list to another
** Item will be placed after a particular node (note if prev_node is NULL then place at head)
*/
void dll_move(old_list, item, new_list, prev_node)
    dll *old_list;
    dll_node *item;
    dll *new_list;
    dll_node *prev_node;
{
  /* disconnect from old */
  if (old_list->head == item)
    old_list->head = item->prev;
  if (old_list->tail == item)
    old_list->tail = item->next;
  if (item->prev)
    item->prev->next = item->next;
  if (item->next)
    item->next->prev = item->prev;

  /* place in new */
  if (!prev_node) {
    item->prev = NULL;
    item->next = new_list->head;
    new_list->head = item;
  }
  else {
    item->prev = prev_node;
    item->next = prev_node->next;
    prev_node->next = item;
  }
  if (item->next)
    item->next->prev = item;
  if (new_list->tail == prev_node) 
    new_list->tail = item;
}

/*
** Go through list and find placement for after node (assumes item passed in is a link_after type)
** (i.e. return value is node that should preceed passed in item)
** proper place is (is precedence order):
**     - before node who wants to be after item
**     - directly after named node if link_directly_after
**     - after named node after any "directly after"'s
**     - after tail of list
*/
dll_node* dll_find_after_position(list, item)
    dll* list;
    PENTRY* item;
{
  dll_node* l;

  if (!list || !item) 
    return(NULL);

  for(l = list->head; l; l = l->next) {
    if (l->value->link_after && strcmp(l->value->link_after_name, item->name)==0) 
      return(l->prev);
    else if(strcmp(l->value->name, item->link_after_name)==0) {
      if (item->link_attr & D_DIRECTLY_AFTER)
        return(l);
      for (; l->next &&
             (l->next->value->link_attr & D_DIRECTLY_AFTER) &&
             strcmp(l->next->value->link_after_name, l->value->name)==0; l = l->next)
        ;
      return(l);
    }
  }

  return(list->tail);
}

/*
** Go through list and find placement for item 
** (i.e. return value is node that should preceed passed in item)
** proper place is (is precedence order):
**     - if item is link_after, first large enough bubble after named section
**     - first large enough bubble
**     - after tail of list
**
** NOTE: This is the place where illegal link_after's are detected (e.g. noexistent section name,
**       circular link_after's, and different memory regions).
**       NULL will be returned on errors.
** NOTE #2: It is absolutely key that the "base" value of the dsects to be a first guess at address location of that section
**          (The only reason why it's not the correct value is because of alignment and link_offset considerations)
*/
dll_node* dll_find_bubble_position(list, item)
    dll* list;
    PENTRY* item;
{
  dll_node* l = list->head;
  unsigned int align = (1<<item->align) - 1;
  unsigned int real_lower_size = (item->mpc.ulong[0] > item->link_size.ulong[0]) ? item->mpc.ulong[0] : item->link_size.ulong[0];

  if (!l) {
    dsectfpc(NULL, item); /* update item's addresses */
    return(NULL);
  }

  /* if it's a link_after, spin ahead to proper section */
  if (item->link_after) {
    while (l && strcmp(l->value->name, item->link_after_name)!=0)
      l = l->next;
    if (!l) { /* no before node could be found ... leave it up to caller if this is illegal. */
      return(NULL); 
    }
    if (l->value->mem_region!=item->mem_region) {
      errout(E_ERR | E_NO_LNUM, "FindBubble","Dsect '%s' says to link after Dsect '%s' which isn't in the same region '%d'.\n",
             item->name,l->value->name, item->mem_region);
      return(NULL); /* ERROR! */
    }
  }

  /* can't place item before any link_first's or between a node and any directly_after's */
  while (l->next && (l->next->value->link_first ||
                     (l->next->value->link_attr & D_DIRECTLY_AFTER)))
    l = l->next;

  /* if item has no code, then we're all done */
  if (item->mpc.ulong[0]==0 && item->link_size.ulong[0]==0 && item->link_size.ulong[0]==0) {
    dsectfpc(l->value, item); /* update item's addresses */
    return(l);
  }
  
  /* spin ahead to first section thats has code for same region */
  while (l && (l->value->mem_region<item->mem_region || (l->value->mpc.ulong[0]==0 && l->value->link_size.ulong[1]==0 &&
                                                         l->value->link_size.ulong[0]==0)))
    l = l->next;

  /* we we went off end of list ... but will still need to coordinate with AllAddrUse or AllAddrAvoid */
  if (!l ) {
    if (item->mem_region == PHYSICAL_REGION)
      dsectfpc(list->tail->value, item);
    else {
      PENTRY fake_section;
      dll_node *prev, fake;
      fake_section.base.ulong[1] = fake_section.base.ulong[0] = 0;
      fake_section.last.ulong[1] = fake_section.last.ulong[0] = 0;
      fake.value = &fake_section;
      prev = get_addr_bubble((item->mem_region == ALL_REGIONS) ? AllAddrAvoid : AllAddrUse,
                             &fake, NULL, item->link_size.ulong[1], real_lower_size, align);
      if (prev) 
        dsectfpc((prev == &fake) ? NULL: prev->value, item); /* update item's addresses */
      else
        errout(E_ERR | E_NO_LNUM, "FindBubble","Dsect '%s' couldn't find a big enough bubble in memory to be placed.\n",
               item->name);
    }
    return(list->tail);
  }

  /* now find appropiated sized bubble */
  for (;l->next && l->next->value->mem_region==item->mem_region; l=l->next) {

    /* Need to compute bubbles sizes differently for 3 types of link_regions.
       1) If PHYSICAL_REGION, bubble size is just the space between l and l->next
       2) IF ALL_REGIONS, bubble size is between l and either l->next or a section from AllAddrAvoid
          (brought down to the next page boundary, remember this is a virtual section)
          whoever is closer to l.  Also, need to check each space between section in AllAddrAvoid that
          falls between l and l->next.
       3) All other regions, bubble size is between l and either l->next or a section from AllAddrUse
          (brought down to the next page boundary, remember this is a virtual section)
          whoever is closer to l.  Also, need to check each space between section in AllAddrUse that
          falls between l and l->next.
    */
    if (item->mem_region == PHYSICAL_REGION) {
      B64 bubble;
      sub64((int32*)l->next->value->base.ulong,(int32*)l->value->last.ulong, (int32*)bubble.ulong);
      if ((bubble.ulong[1] > item->link_size.ulong[1]) ||
          (bubble.ulong[1] == item->link_size.ulong[1] &&
           (bubble.ulong[0] > (real_lower_size + ((align+1) - (l->value->last.ulong[0] & align)))))) {
        /* this bubble is big enough */
        dsectfpc(l->value, item); /* update item's addresses */
        break;
      }
    }
    else {
      dll_node *prev = get_addr_bubble((item->mem_region == ALL_REGIONS) ? AllAddrAvoid : AllAddrUse,
                                       l, l->next, item->link_size.ulong[1], real_lower_size, align);
      if (prev) {
        dsectfpc(prev->value, item); /* update item's addresses */
        break;
      }
    }
  }

  if (!l->next || l->next->value->mem_region!=item->mem_region) {
    /* we reached the end of the region ... but will still need to coordinate with AllAddrUse or AllAddrAvoid */
    if (item->mem_region == PHYSICAL_REGION)
      dsectfpc(l->value, item);
    else {
      dll_node *prev;
      if (l->value->mem_region!=item->mem_region) {
        PENTRY fake_section;
        dll_node fake;
        fake_section.base.ulong[1] = fake_section.base.ulong[0] = 0;
        fake_section.last.ulong[1] = fake_section.last.ulong[0] = 0;
        fake.value = &fake_section;
        prev = get_addr_bubble((item->mem_region == ALL_REGIONS) ? AllAddrAvoid : AllAddrUse,
                               &fake, NULL, item->link_size.ulong[1], real_lower_size, align);
        if (prev) 
          dsectfpc((prev == &fake) ? NULL: prev->value, item); /* update item's addresses */
        else
          errout(E_ERR | E_NO_LNUM, "FindBubble","Dsect '%s' couldn't find a big enough position in memory to be placed.\n",item->name);

        return(l->prev); // remember...l's region # is > then item 
      }
      else {
        prev = get_addr_bubble((item->mem_region == ALL_REGIONS) ? AllAddrAvoid : AllAddrUse,
                               l, NULL, item->link_size.ulong[1], real_lower_size, align);
        if (prev) 
          dsectfpc(prev->value, item); /* update item's addresses */
        else
          errout(E_ERR | E_NO_LNUM, "FindBubble","Dsect '%s' couldn't find a big enough position in memory to be placed.\n",item->name);
      }
    }
  }
    
  return(l);
}

/*
** Traverses entire list in direction defined and performs the input function
*/
void dll_foreach(list, func, direction)
    dll *list;
    void (*func)();
    int direction;
{
    dll_node *l;

    if ((list == NULL) ||
        (func == NULL)) {
        return;
    }
    
    if (direction == DLL_FORWARD) {
        l = list->head;
        while (l != NULL) {
            (*func)(l->value->name, l->value);
            l = l->next;
        }
    }
    else {
        l = list->tail;
        while (l != NULL) {
            (*func)(l->value->name, l->value);
            l = l->prev;
        }
    }
}

/*
** Sorts list with respect to .link_address and .link_after directives
** Also calculates all the final PC's for each section
*/
void dll_sort(list)
    dll *list;
{
  dll_node *l, *next_l;
  dll_node *last_first = NULL;
  dll *nldll = dll_init(); /*list of section that has no link requirements */
  dll *ladll = dll_init();  /*temp list to put link_after's */
  dll *fdll = dll_init();  /*the final dll to return back */
  dll *lstdll = dll_init();  /*temp list to put link_last's */
  AllAddrUse = dll_init();  /* global lists used in conjunction with "link_region all" sections */
  AllAddrAvoid = dll_init();
  
  if (list == NULL || list->head == NULL) return;

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********Before sorting ...\n");
  dll_print(list);
#endif
  
  /* 1st - place::
   *     link_first      -> at head in fdll
   *     link_addr       -> at tail in order in fdll
   *     link_after      -> in "after order" in ladll
   *     link_last       -> in lstdll
   *     no_code&no_link -> after link_first's in fdll
   *     code&no_link    -> at head of nldll */
  for (l=list->head; l!=NULL; l=next_l) {
    next_l = l->next;	/* record next pointer in case we move current to new list. */
    if (l->value->link_first) {
      if (l->value->mpc.ulong[0]!=0 || l->value->link_size.ulong[1]!=0 || l->value->link_size.ulong[0]!=0)
        errout(E_ERR | E_NO_LNUM, "dll_sort", "Link_first Dsect '%s' contains code.", l->value->name);
      if (!fdll->head || !fdll->head->value->link_first)
        last_first = l;
      dll_move(list, l, fdll, NULL);
      dsectfpc(NULL, l->value);

    }
    else if (l->value->link_address) {
      dll_node *p;
      if (l->value->mpc.ulong[0]==0 && l->value->link_size.ulong[1]==0 && l->value->link_size.ulong[0]==0)
        errout(E_ERR | E_NO_LNUM, "dll_sort", "Link_address Dsect '%s' doesn't contain code.", l->value->name);
      dsectfpc(NULL, l->value);

      for (p = fdll->tail; p && p->value->link_address && (l->value->mem_region < p->value->mem_region ||
                                                           (l->value->mem_region == p->value->mem_region &&
                                                            (l->value->base.ulong[1] < p->value->base.ulong[1] ||
                                                             l->value->base.ulong[0] < p->value->base.ulong[0]))); p = p->prev)
        ;
      dll_move(list, l, fdll, p);
      overlap_check(l);
      if (l->value->mem_region!=PHYSICAL_REGION)
        dll_addr_insert((l->value->mem_region==ALL_REGIONS ? AllAddrUse : AllAddrAvoid), l->value);
    }
    else if (l->value->link_after) 
      dll_move(list, l, ladll, dll_find_after_position(ladll, l->value));
    else if (l->value->link_last) {
      if (l->value->mpc.ulong[0]!=0 || l->value->link_size.ulong[1]!=0 || l->value->link_size.ulong[0]!=0)
        errout(E_ERR | E_NO_LNUM, "dll_sort", "Link_last Dsect '%s' contains code.", l->value->name);
      dll_move(list, l, lstdll, lstdll->tail);
    }
    else if (l->value->mpc.ulong[0]==0 && l->value->link_size.ulong[1]==0 && l->value->link_size.ulong[0]==0) { /* if this header has no code in it */
      dsectfpc(last_first ? last_first->value : NULL, l->value); 
      dll_move(list, l, fdll, last_first);
    }
    else
      dll_move(list, l, nldll, NULL); /* place in different dll so it doesn't degrade preformance of next step below */
  }

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********Contents of fdll ...\n");
  dll_print(fdll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of ladll ...\n");
  dll_print(ladll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of lstdll ...\n");
  dll_print(lstdll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of nldll ...\n");
  dll_print(nldll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrUse ...\n");
  dll_print(AllAddrUse);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrAvoid ...\n");
  dll_print(AllAddrAvoid);
#endif

  /* 2nd - now place link_after's in fdll in the address holes after named section*/
  for (l=ladll->head; l!=NULL; l=next_l) {
    dll_node *temp = dll_find_bubble_position(fdll, l->value);
    next_l = l->next;	/* record next pointer in case we move current */
    if (temp) { /* if before node couldn't be found (i.e. was in nldll) then do on 2nd pass below */
      dll_move(ladll, l, fdll, temp);
      if (l->value->mem_region!=PHYSICAL_REGION)
        dll_addr_insert((l->value->mem_region==ALL_REGIONS ? AllAddrUse : AllAddrAvoid), l->value);
    }
  }

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********Contents of fdll after placing 1st pass link_afters ...\n");
  dll_print(fdll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of ladll after 1st pass ...\n");
  dll_print(ladll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrUse ...\n");
  dll_print(AllAddrUse);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrAvoid ...\n");
  dll_print(AllAddrAvoid);
#endif

  /* 3rd - put nldll items into any appropiate bubbles of fdll */
  for (l=nldll->head; l!=NULL; l=next_l) {
    next_l = l->next;	/* record next pointer in case we move current */
    dll_move(nldll, l, fdll, dll_find_bubble_position(fdll, l->value));
    if (l->value->mem_region!=PHYSICAL_REGION)
      dll_addr_insert((l->value->mem_region==ALL_REGIONS ? AllAddrUse : AllAddrAvoid), l->value);
  }

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********Contents of fdll after placing non-linked ...\n");
  dll_print(fdll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrUse ...\n");
  dll_print(AllAddrUse);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrAvoid ...\n");
  dll_print(AllAddrAvoid);
#endif

  /* 4th - now that nldll sect are in, go through after's again */
  for (l=ladll->head; l!=NULL; l=next_l) {
    dll_node *temp = dll_find_bubble_position(fdll, l->value);
    next_l = l->next;	/* record next pointer in case we move current */
    if (temp) {
      dll_move(ladll, l, fdll, temp);
      if (l->value->mem_region!=PHYSICAL_REGION)
        dll_addr_insert((l->value->mem_region==ALL_REGIONS ? AllAddrUse : AllAddrAvoid), l->value);
    }
    else
      errout(E_ERR | E_NO_LNUM, "dll_sort","Dsect '%s' couldn't find it's link after Dsect '%s'.\n",
             l->value->name,l->value->link_after_name);
  }

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********Contents of fdll after placing 2nd pass link_afters ...\n");
  dll_print(fdll);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrUse ...\n");
  dll_print(AllAddrUse);
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\nContents of AllAddrAvoid ...\n");
  dll_print(AllAddrAvoid);
#endif

  /* 5th - finally place all link_last's at end of fdll */
  for (l=lstdll->head; l!=NULL; l=next_l) {
    next_l = l->next;	/* record next pointer in case we move current */
    dll_move(lstdll, l, fdll, fdll->tail);
    dsectfpc(l->prev ? l->prev->value : NULL, l->value); 
  }
  
  /* OK -- all done!! Switch list pointer to point to fdll items*/
  list->head = fdll->head;
  list->tail = fdll->tail;

#ifdef SORT_DEBUG
  errout(E_INFO | E_NO_LNUM, "DLL", "\n\n*********After sorting ...\n");
  dll_print(list);
#endif
  
  /* clean-up */
  dll_free(AllAddrUse, NULL);  /* don't delete nodes here! just links to real nodes for efficentcy */
  dll_free(AllAddrAvoid, NULL);
  FREE(fdll);
  FREE(ladll);
  FREE(nldll);
  FREE(lstdll);
}

