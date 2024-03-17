/*
	Copyright (c) 1992
	Digital Equipment Corporation, Maynard, Massachusetts 01754

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal.h"

#define NONE		0
#define LW		4
#define QW		8

#ifdef __alpha
#define MEM_ALIGN		QW
#else
#ifdef __mips__
#define MEM_ALIGN		LW
#else
#define MEM_ALIGN		NONE
#endif
#endif

#ifdef __MSDOS__
#define MAX_MEMORY_CHUNKS	(64*1024)
#define MEMORY_CHUNK_SIZE	(1024)
#else
#define MAX_MEMORY_CHUNKS	(512)
#define MEMORY_CHUNK_SIZE	(1024*1024)
#endif

int total_alloc;
static int mem_blk;
static unsigned int mem_use[MAX_MEMORY_CHUNKS];
static char* mem_tab[MAX_MEMORY_CHUNKS];

#define NUM_TOKENS_TO_ALLOC	(100)

int numtokens = 0;
static TOKEN_LIST* toklist = NULL;
static TOKEN_LIST* tokareas = NULL;

void init_memmgt()
{
    int i;
  
    for (i=0; i<MAX_MEMORY_CHUNKS; ++i) {
        mem_use[i] = 0;
        mem_tab[i] = NULL;
    }

    mem_blk = 0;
#ifndef USE_MALLOC_FREE
    mem_tab[0] = malloc(MEMORY_CHUNK_SIZE);
#endif

    total_alloc = 0;
  
    return;
}

void end_memmgt()
{
    int i;
    TOKEN_LIST *ttok, *ntok;
  
    for (i=0; i<mem_blk; ++i)
        if (mem_tab[i] != NULL)
            free(mem_tab[i]);

#ifdef BLOCK_TOKEN_ALLOC
    for (ttok = tokareas; ttok != NULL; ttok = ntok) {
        ntok = ttok->next;
        FREE(ttok->tok.s);
    }
#endif
    
    return;
}

TOKEN_LIST* alloc_tok()
{
#ifdef BLOCK_TOKEN_ALLOC
    TOKEN_LIST* rettok = toklist;
    
    if (rettok == NULL) {
        /* allocate a bunch of tokens */
        TOKEN_LIST* ttok = ALLOC0(NUM_TOKENS_TO_ALLOC, TOKEN_LIST);
        int i;

        numtokens += NUM_TOKENS_TO_ALLOC;
        
        for (i=2; i<(NUM_TOKENS_TO_ALLOC-1); ++i) {
            ttok[i].next = &ttok[i+1];
        }

        toklist = &ttok[2];

        (&ttok[1])->next = tokareas;
        tokareas = &ttok[1];
        tokareas->tok.s = (char*)ttok;
        
        return &ttok[0];
    }

    /* move head to next token in list */
    toklist = toklist->next;

    /* return one off the head of the toklist */
    /* zero out first, because that is almost always useful. */
    memset(rettok, 0, sizeof(TOKEN_LIST));
    
    return rettok;
#else
    return ALLOC0(1, TOKEN_LIST);
#endif
}

void free_tok(TOKEN_LIST* ttok)
{
    if (ttok == NULL)
        return;
    
#ifdef BLOCK_TOKEN_ALLOC
    ttok->next = toklist;
    toklist = ttok;
#else
    FREE(ttok);
#endif
    
    return;
}

void free_tok_s(TOKEN_LIST* ttok)
{
    if (ttok == NULL)
        return;

    switch (ttok->type) {
    case STRING:
    case BODYTEXT:
    case SYMBOL:
    case ARG_VALUE:
    case LOCAL_LABEL:
    case COMMENT:
    case DOT:
        if (ttok->tok.s != NULL) 
            FREE(ttok->tok.s);
        break;
    default:
        break;
    }
    free_tok(ttok);
    
    return;
}

#ifdef __STDC__
void *my_alloc(unsigned int size)
#else
void *my_alloc(size)
unsigned int size;
#endif
{
    void *ptr;

    if (MEM_ALIGN != NONE) {
        int temp = size % MEM_ALIGN;
        
        if (temp != 0)
            size += (MEM_ALIGN - temp);
    }

    /* record memory allocation */
    total_alloc += size;

    if (size > MEMORY_CHUNK_SIZE) {
        /* if it's that big, just malloc it straight... */
        ptr = malloc(size);
        if (ptr == NULL) {
            errout(E_ERR, "my_alloc", "insufficient virtual memory, allocating large (%d bytes) block.\n", size);
            uninit();
            exit(EXIT_FAILURE);
        }

        if ((mem_blk+1) >= MAX_MEMORY_CHUNKS) {
            errout(E_ERR, "my_alloc", "overran maximum memory chunks.\n");
            uninit();
            exit(EXIT_FAILURE);
        }
        
        /* record it as a mem_tab entry so it eventually gets freed. */
        /* first, move the current block to one higher.
        mem_use[mem_blk+1] = mem_use[mem_blk];
        mem_tab[mem_blk+1] = mem_tab[mem_blk];

        /* record this chunk in this block */
        mem_use[mem_blk] = size;
        mem_tab[mem_blk] = ptr;

        /* move to next block (actually same block, displaced by one). */
        mem_blk++;
    } else {
        unsigned int junk;

        if ((size + mem_use[mem_blk]) > MEMORY_CHUNK_SIZE) {
            if (++mem_blk >= MAX_MEMORY_CHUNKS) {
                errout(E_ERR, "my_alloc", "overran maximum memory chunks.\n");
                uninit();
                exit(EXIT_FAILURE);
            } else {
                mem_tab[mem_blk] = malloc(MEMORY_CHUNK_SIZE);
                if (mem_tab[mem_blk] == NULL) {
                    errout(E_ERR, "my_alloc", "insufficient virtual memory.\n");
                    uninit();
                    exit(EXIT_FAILURE);
                }
            }
        }
    
        junk = mem_use[mem_blk];
        mem_use[mem_blk] += size;
    
        ptr = mem_tab[mem_blk] + junk;
    }  

    return ptr;
}

#ifdef __STDC__
void *my_calloc(int n, int size)
#else
void *my_calloc(n, size)
int n, size;
#endif
{
    void *temp = my_alloc(n*size);
  
    memset(temp, 0, n*size);
  
    return temp;
}
