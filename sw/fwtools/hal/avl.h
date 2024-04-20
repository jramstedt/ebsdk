#ifndef AVL_INCLUDED
#define AVL_INCLUDED
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
/* FILE: AVL.H                                                               */
/* This file contains the definitions for AVL balance tree	             */
/*****************************************************************************/


typedef struct avl_node_struct avl_node;
struct avl_node_struct {
    avl_node *left, *right;
    char *key;
    char *value;
    int height;
};


typedef struct avl_tree_struct avl_tree;
struct avl_tree_struct {
    avl_node *root;
    int (*compar)();
    int num_entries;
    int modified;
};


typedef struct avl_generator_struct avl_generator;
struct avl_generator_struct {
    avl_tree *tree;
    avl_node **nodelist;
    int count;
};


#define AVL_FORWARD 	0
#define AVL_BACKWARD 	1


extern avl_tree *avl_init_table();
extern int avl_delete(), avl_insert(), avl_lookup();
extern int avl_first(), avl_last(), avl_find_or_add();
extern int avl_count(), avl_numcmp(), avl_gen();
extern void avl_foreach(), avl_free_table(), avl_free_gen();
extern avl_generator *avl_init_gen();

#define avl_is_member(tree, key)	avl_lookup(tree, key, (char **) 0)

#define avl_foreach_item(table, gen, dir, key_p, value_p) 	\
    for(gen = avl_init_gen(table, dir); 			\
	    avl_gen(gen, key_p, value_p) || (avl_free_gen(gen),0);)

#endif
