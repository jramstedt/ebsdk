#ifndef DLL_INCLUDED
#define DLL_INCLUDED
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
/*****************************************************************************/
/* FILE: DLL.H                                                               */
/* This file contains the definitions for a specialized Doubly Linked List   */
/*****************************************************************************/

typedef struct dll_node_struct dll_node;
struct dll_node_struct {
    dll_node *prev;
    dll_node *next;
    struct psectable *value;
};


typedef struct dll_struct dll;
struct dll_struct {
    dll_node *head;
    dll_node *tail;
};

#define DLL_FORWARD 	0
#define DLL_BACKWARD 	1

extern dll *dll_init();
extern void dll_insert();
extern dll_node* dll_lookup();
extern void dll_sort();
extern void dll_foreach();

#endif

