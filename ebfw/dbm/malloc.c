/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: malloc.c,v 1.1.1.1 1998/12/29 21:36:15 paradis Exp $";
#endif

/*
 * $Log: malloc.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:15  paradis
 * Initial CVS checkin
 *
 * Revision 1.16  1997/10/03  14:36:42  fdh
 * Modified to position the heap based on the __start label
 * rather then the start of memory.
 * Allow a max of 16MB rather than 8MB below the heap.
 *
 * Revision 1.15  1997/04/30  16:46:06  pbell
 * Changed mempool boundary varriables to ul type.
 *
 * Revision 1.14  1997/04/24  20:50:57  pbell
 * Changed mempool boundary varriables to sl type for NT
 *
 * Revision 1.13  1997/03/17  19:15:34  fdh
 * Modified to dynamically compute the resources available to
 * the memory storage pool at runtime.
 *
 * Revision 1.12  1997/03/12  21:49:17  fdh
 * Increment malloc counter after call to init_pool().
 *
 * Revision 1.11  1997/03/12  19:30:49  fdh
 * Added a debug statement to report when the memory allocation fails.
 *
 * Revision 1.10  1997/03/12  15:52:01  fdh
 * Upgraded the very simple malloc to a hmm... simple malloc.
 * At least this version has a working free and it tries to
 * allocate the last freed memory before going off searching
 * for a block to allocate.  Simple but small.
 *
 * Revision 1.9  1995/11/22  21:22:00  cruz
 * Removed static initialization of variables.
 *
 * Revision 1.8  1995/10/26  21:40:49  cruz
 * Fixed function prototype.
 *
 * Revision 1.7  1995/10/05  01:10:58  fdh
 * Added copyright header and log info.
 * Corrected some static data types.
 *
 * revision 1.6  1995/10/04 23:39:11  fdh
 * Updated internal function prototypes.
 * 
 * revision 1.5  1995/08/29 12:59:13  fdh
 * Allow external inits of calls to malloc library.
 * 
 * revision 1.4  1995/08/25 20:56:54  fdh
 * Removed malloc.h
 * 
 * revision 1.3  1995/08/25 20:13:23  fdh
 * Changed initialization around a little.
 * This malloc library needs many enhancements to be
 * useful.  This is a simple implementation only intended to
 * support the BIOS emulation during VGA initialization.
 * 
 * revision 1.2  1994/06/22 19:52:50  fdh
 * removed references to mymalloc.
 * 
 * revision 1.1  1994/06/13 19:20:37  fdh
 * Initial revision
 */

#include "system.h"
#include "lib.h"
#include "mon.h"

#define MINBLOCK	512
#define BOUNDARY	((unsigned long)(sizeof(unsigned long) - 1))
#define TAG_SIZE	((sizeof(tag_t) + BOUNDARY) & ~BOUNDARY)

#define _MEG		(1024 * 1024)
#define POOL_BOUNDARY	((unsigned long)(0x100000 - 1))
#define DBMSTART	(((unsigned long)(&__start)) & ~POOL_BOUNDARY)
#define POOLST_MIN	(DBMSTART + (2*_MEG))
#define POOLST_MAX	(DBMSTART + (16*_MEG))
#define MEM_SIZE	(*sysdata.mem_size)

#ifdef DEBUG
#define CHECK_BLOCK	check_block
#else
#define CHECK_BLOCK(block, c) ((void)0)
#endif

int storage_initialized;
ul storage_pool_start;
ul storage_pool_end;

extern char __start;

static struct {
  int malloc;
  int realloc;
  int free;
} calls;

typedef struct _tag {
  size_t size;
  int free;
  struct _tag *next;
  struct _tag *previous;
} tag_t;

static tag_t *alloc_head;
static tag_t *last_free;

static tag_t *find_block(size_t size);
static void bogus_block(int error);
static void dump_block(tag_t *block, int c);

#ifdef DEBUG
static void check_block(tag_t *block, int c);
#endif

static tag_t *find_block(size_t size)
{
  tag_t *tag;

  /*
  ** Try to find a free block of minimum size
  */

  /* Start at last freed block */
  for(tag = last_free; tag != NULL; tag = tag->next) {
    if ((tag->size >= _MAX(size, MINBLOCK)) && tag->free) {
      return (tag);
    }
  }

  /* Restart at the head of the list */
  for(tag = alloc_head; tag < last_free; tag = tag->next) {
    if ((tag->size >= _MAX(size, MINBLOCK)) && tag->free) {
      return (tag);
    }
  }

#ifdef DEBUG
  printf("Malloc could not find a free block to fit %d bytes\n", size);
#endif

  return (NULL);
}

void * malloc(size_t size)
{
  tag_t *block;
  tag_t *newblock;

  if (!storage_initialized) init_storage_pool();

  ++calls.malloc;

  size = (size_t) (((unsigned long) size + BOUNDARY) & ~BOUNDARY);

  if ((block = find_block(size)) != NULL) {
    size_t newsize = block->size - size - TAG_SIZE;

    block->free = FALSE;

    if (newsize > MINBLOCK) {	/* Reduces fragmentation */
      newblock = (tag_t *) ((char *) block + size + TAG_SIZE);

      newblock->size = newsize;
      newblock->next = block->next;
      if (newblock->next != NULL)
      newblock->next->previous = newblock;
      newblock->previous = block;
      newblock->free = TRUE;

      block->size = size;
      block->next = newblock;

      last_free = newblock;
      CHECK_BLOCK(newblock, 'n');
    }

    CHECK_BLOCK(block, 'a');

    return ((char *) block + TAG_SIZE);
  }

  return (NULL);
}

void free(void *pointer)
{
  tag_t *block, *next, *previous;

  ++calls.free;

  if (pointer == NULL)		/* Bad pointer */
    return;
  block = (tag_t *) ((unsigned long) pointer - TAG_SIZE);
  if (block->size & BOUNDARY)	/* Bad pointer */
    return;

  last_free = block;
  block->free = TRUE;
  CHECK_BLOCK(block, 'f');

  /*
  ** Merge freed block into adjacent free blocks
  **/
  if ((next = block->next) != NULL) {
    if ((unsigned long) next & BOUNDARY) /* Bad pointer */
      return;

    if (next->free == TRUE) {
      CHECK_BLOCK(next, '+');
      block->size = block->size + next->size + TAG_SIZE;
      block->next = next->next;
      if (block->next != NULL)
	block->next->previous = block;
      CHECK_BLOCK(block, '=');
      CHECK_BLOCK(next->next, '+');
    }
  }

  if ((previous = block->previous) != NULL) {
    if ((unsigned long) previous & BOUNDARY) /* Bad pointer */
      return;

    if (previous->free == TRUE) {
      CHECK_BLOCK(previous, '-');
      previous->size = block->size + previous->size + TAG_SIZE;
      previous->next = block->next;
      if (previous->next != NULL)
	previous->next->previous = previous;
      last_free = previous;
      CHECK_BLOCK(previous, '=');
      CHECK_BLOCK(previous->next, 'x');
    }
  }
}

void *realloc(void *pointer, size_t size)
{
  void *newp;

  ++calls.realloc;
  if ((newp = malloc(size)) == NULL)
    return (NULL);

  memmove(newp, pointer, size);
  free(pointer);

  return newp;
}

void malloc_summary(char *string)
{
  ul heapsize;
  printf("\nMalloc Summary:%s\n", string);

  if (!storage_initialized)
    init_storage_pool();

  heapsize = storage_pool_end - storage_pool_start;
  if (heapsize > 0) {
    printf("Memory Heap: %d%s, start=0x%04x, end=0x%04x\n",
	   (heapsize>(1024*1024)) ? heapsize/(1024*1024) : heapsize/1024,
	   (heapsize>(1024*1024)) ? "MB" : "KB",
	   storage_pool_start, storage_pool_end);
  }
  printf("malloc %d, realloc %d, free %d\n\n",
	 calls.malloc, calls.realloc, calls.free);
}

void init_storage_pool(void)
{
  storage_initialized = TRUE;
  calls.malloc = calls.realloc = calls.free = 0;

  if (sysdata.valid) {
    /*
     * Start pool at halfway point between the start of this code and the
     * top of total memory leaving at least 2Meg but not more than 16Meg
     * below the pool.
     */
    storage_pool_start = DBMSTART + ((MEM_SIZE-DBMSTART)>>1);
    if	   (storage_pool_start < POOLST_MIN) storage_pool_start = POOLST_MIN;
    else if(storage_pool_start > POOLST_MAX) storage_pool_start = POOLST_MAX;

    /*
     * Leave the top 1/4 of the memory between the start of this code
     * and the top of total memory for stack use above heap.
     */
    storage_pool_end = MEM_SIZE - ((MEM_SIZE-DBMSTART)>>2);
    /* If there is no space left then set the end to the start */
    if( storage_pool_end < storage_pool_start )
	storage_pool_end = storage_pool_start;
  }
  else {
    storage_pool_start = storage_pool_end = MINIMUM_SYSTEM_MEMORY;
  }

  alloc_head = (tag_t *) storage_pool_start;
  alloc_head->size = (size_t) (storage_pool_end - storage_pool_start - TAG_SIZE);
  alloc_head->free = TRUE;
  alloc_head->next = NULL;
  alloc_head->previous = NULL;

  last_free = alloc_head;
#undef POOLST_MIN
#undef POOLST_MAX
}

__attribute__((unused))
static void bogus_block(int error)
{
  printf("Bogus block error: %d\n", error);
}

__attribute__((unused))
static void dump_block(tag_t *block, int c)
{
  if (block == NULL) {
    printf("0x%x: %c\n", block, c);
    return;
  }

  printf("0x%x: %c\tsize = %d;\n\t\tfree = %d;\n\t\tnext = 0x%x;\n\t\tprevious = 0x%x\n\t\tendpoint = 0x%x\n",
	 block, c, block->size, block->free, block->next, block->previous,
	 (unsigned long) block + block->size + TAG_SIZE);
}

#ifdef DEBUG
static void check_block(tag_t *block, int c)
{
#ifdef PRTRACE
  dump_block(block, c);
#endif

  if (block == NULL)
    return;

  if (block->previous != NULL) {
    if (block->previous->next != block) {
      dump_block(block->previous, '1');
      bogus_block(1);
    }
  }

  if (block->next == NULL) {
    if (((unsigned long) block + block->size + TAG_SIZE) != storage_pool_end)
      bogus_block(2);
  }

  if (block->next != NULL) {
    if (((unsigned long) block + block->size + TAG_SIZE) != ((unsigned long) block->next))
      bogus_block(3);
  }

  if ((block->size+TAG_SIZE) > (storage_pool_end-storage_pool_start))
    bogus_block(4);

}
#endif /* DEBUG */
