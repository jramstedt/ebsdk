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
/* FILE: AVL.C								     */
/* This file contains the implementation of AVL balance tree		     */
/*****************************************************************************/
#include <stdio.h>
#include "hal.h"
#include "avl.h"

#ifdef __STDC__
void *my_alloc(unsigned int size);
#else
void *my_alloc();
#endif

#undef ALLOC
#undef FREE
#ifndef USE_MALLOC_FREE
#define ALLOC(type, num)	\
    ((type *) my_alloc(sizeof(type) * (num)))
#define FREE(obj)		
#else
#define ALLOC(type, num)	\
    ((type *) malloc(sizeof(type) * (num)))
#define FREE(obj)	free(obj)
#endif

#define MAX(a,b)	((a) > (b) ? (a) : (b))

#define NIL(type)		\
    ((type *) 0)

#define HEIGHT(node) (node == NIL(avl_node) ? -1 : (node)->height)
#define BALANCE(node) (HEIGHT((node)->right) - HEIGHT((node)->left))

#define compute_height(node) {				\
    int x=HEIGHT(node->left), y=HEIGHT(node->right);	\
    (node)->height = MAX(x,y) + 1;			\
}

#ifdef __alpha
typedef long ptrint;
#else
typedef int ptrint;
#endif

#define COMPARE(key, nodekey, compare)	 		\
    ((compare == avl_numcmp) ? 				\
	(ptrint) key - (ptrint) nodekey : 			\
	(*compare)(key, nodekey))

int avl_numcmp(char*, char*);

#define STACK_SIZE	50

static avl_node *new_node();
static avl_node *find_rightmost();
static void do_rebalance(); 
static int rotate_left();
static int rotate_right();
static int do_check_tree();

avl_tree *
avl_init_table(compar)
int (*compar)();
{
    avl_tree *tree;

    tree = ALLOC(avl_tree, 1);
    tree->root = NIL(avl_node);
    tree->compar = compar;
    tree->num_entries = 0;
    return tree;
}


int
avl_lookup(tree, key, value_p)
avl_tree *tree;
register char *key;
char **value_p;
{
    register avl_node *node;
    register int (*compare)() = tree->compar, diff;

    node = tree->root;
    while (node != NIL(avl_node)) {
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) {
	    /* got a match */
	    if (value_p != NIL(char *)) *value_p = node->value;
	    return 1;
	}
	node = (diff < 0) ? node->left : node->right;
    }
    return 0;
}

int
avl_first(tree, key_p, value_p)
avl_tree *tree;
char **key_p;
char **value_p;
{
    register avl_node *node;

    if (tree->root == 0) {
	return 0;		/* no entries */
    } else {
	/* walk down the tree; stop at leftmost leaf */
	for(node = tree->root; node->left != 0; node = node->left) {
	}
	if (key_p != NIL(char *)) *key_p = node->key;
	if (value_p != NIL(char *)) *value_p = node->value;
	return 1;
    }
}

int
avl_last(tree, key_p, value_p)
avl_tree *tree;
char **key_p;
char **value_p;
{
    register avl_node *node;

    if (tree->root == 0) {
	return 0;		/* no entries */
    } else {
	/* walk down the tree; stop at rightmost leaf */
	for(node = tree->root; node->right != 0; node = node->right) {
	}
	if (key_p != NIL(char *)) *key_p = node->key;
	if (value_p != NIL(char *)) *value_p = node->value;
	return 1;
    }
}

int
avl_insert(tree, key, value)
avl_tree *tree;
char *key;
char *value;
{
    register avl_node **node_p, *node;
    register int stack_n = 0;
    register int (*compare)() = tree->compar;
    avl_node **stack_nodep[STACK_SIZE];
    int diff, status;

    node_p = &tree->root;

    /* walk down the tree (saving the path); stop at insertion point */
    status = 0;
    while ((node = *node_p) != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) status = 1;
	node_p = (diff < 0) ? &node->left : &node->right;
    }

    /* insert the item and re-balance the tree */
    *node_p = new_node(key, value);
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries++;
    tree->modified = 1;
    return status;
}


int
avl_find_or_add(tree, key, slot_p)
avl_tree *tree;
char *key;
char ***slot_p;
{
    register avl_node **node_p, *node;
    register int stack_n = 0;
    register int (*compare)() = tree->compar;
    avl_node **stack_nodep[STACK_SIZE];
    int diff;

    node_p = &tree->root;

    /* walk down the tree (saving the path); stop at insertion point */
    while ((node = *node_p) != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) {
	    if (slot_p != 0) *slot_p = &node->value;
	    return 1;		/* found */
	}
	node_p = (diff < 0) ? &node->left : &node->right;
    }

    /* insert the item and re-balance the tree */
    *node_p = new_node(key, NIL(char));
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries++;
    tree->modified = 1;
    if (slot_p != 0) *slot_p = &node->value;
    return 0;			/* not already in tree */
}

int
avl_delete(tree, key_p, value_p)
avl_tree *tree;
char **key_p;
char **value_p;
{
    register avl_node **node_p, *node, *rightmost;
    register int stack_n = 0;
    char *key = *key_p;
    int (*compare)() = tree->compar, diff;
    avl_node **stack_nodep[STACK_SIZE];
    
    node_p = &tree->root;

    /* Walk down the tree saving the path; return if not found */
    while ((node = *node_p) != NIL(avl_node)) {
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) goto delete_item;
	stack_nodep[stack_n++] = node_p;
	node_p = (diff < 0) ? &node->left : &node->right;
    }
    return 0;		/* not found */

    /* prepare to delete node and replace it with rightmost of left tree */
delete_item:
    *key_p = node->key;
    if (value_p != 0) *value_p = node->value;
    if (node->left == NIL(avl_node)) {
	*node_p = node->right;
    } else {
	rightmost = find_rightmost(&node->left);
	rightmost->left = node->left;
	rightmost->right = node->right;
	rightmost->height = -2; 	/* mark bogus height for do_rebal */
	*node_p = rightmost;
	stack_nodep[stack_n++] = node_p;
    }
    FREE(node);

    /* work our way back up, re-balancing the tree */
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries--;
    tree->modified = 1;
    return 1;
}

static void 
avl_record_gen_forward(node, gen)
avl_node *node;
avl_generator *gen;
{
    if (node != NIL(avl_node)) {
	avl_record_gen_forward(node->left, gen);
	gen->nodelist[gen->count++] = node;
	avl_record_gen_forward(node->right, gen);
    }
}


static void 
avl_record_gen_backward(node, gen)
avl_node *node;
avl_generator *gen;
{
    if (node != NIL(avl_node)) {
	avl_record_gen_backward(node->right, gen);
	gen->nodelist[gen->count++] = node;
	avl_record_gen_backward(node->left, gen);
    }
}


avl_generator *
avl_init_gen(tree, dir)
avl_tree *tree;
int dir;
{
    avl_generator *gen;

    /* what a hack */
    gen = ALLOC(avl_generator, 1);
    gen->tree = tree;
    gen->nodelist = ALLOC(avl_node *, avl_count(tree));
    gen->count = 0;
    if (dir == AVL_FORWARD) {
	avl_record_gen_forward(tree->root, gen);
    } else {
	avl_record_gen_backward(tree->root, gen);
    }
    gen->count = 0;

    /* catch any attempt to modify the tree while we generate */
    tree->modified = 0;
    return gen;
}

int
avl_gen(gen, key_p, value_p)
avl_generator *gen;
char **key_p;
char **value_p;
{
    avl_node *node;

    if (gen->count == gen->tree->num_entries) {
	return 0;
    } else {
	node = gen->nodelist[gen->count++];
	if (key_p != NIL(char *)) *key_p = node->key;
	if (value_p != NIL(char *)) *value_p = node->value;
	return 1;
    }
}


void
avl_free_gen(gen)
avl_generator *gen;
{
    FREE(gen->nodelist);
    FREE(gen);
}

static avl_node *
find_rightmost(node_p)
register avl_node **node_p;
{
    register avl_node *node;
    register int stack_n = 0;
    avl_node **stack_nodep[STACK_SIZE];

    node = *node_p;
    while (node->right != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	node_p = &node->right;
	node = *node_p;
    }
    *node_p = node->left;

    do_rebalance(stack_nodep, stack_n);
    return node;
}


static void
do_rebalance(stack_nodep, stack_n)
register avl_node ***stack_nodep;
register int stack_n;
{
    register avl_node **node_p, *node;
    register int hl, hr;
    int height;

    /* work our way back up, re-balancing the tree */
    while (--stack_n >= 0) {
	node_p = stack_nodep[stack_n];
	node = *node_p;
	hl = HEIGHT(node->left);		/* watch for NIL */
	hr = HEIGHT(node->right);		/* watch for NIL */
	if ((hr - hl) < -1) {
	    rotate_right(node_p);
	} else if ((hr - hl) > 1) {
	    rotate_left(node_p);
	} else {
	    height = MAX(hl, hr) + 1;
	    if (height == node->height) break;
	    node->height = height;
	}
    }
}

static int
rotate_left(node_p)
register avl_node **node_p;
{
    register avl_node *old_root = *node_p, *new_root, *new_right;

    if (BALANCE(old_root->right) >= 0) {
	*node_p = new_root = old_root->right;
	old_root->right = new_root->left;
	new_root->left = old_root;
    } else {
	new_right = old_root->right;
	*node_p = new_root = new_right->left;
	old_root->right = new_root->left;
	new_right->left = new_root->right;
	new_root->right = new_right;
	new_root->left = old_root;
	compute_height(new_right);
    }
    compute_height(old_root);
    compute_height(new_root);
}


static int
rotate_right(node_p)
avl_node **node_p;
{
    register avl_node *old_root = *node_p, *new_root, *new_left;

    if (BALANCE(old_root->left) <= 0) {
	*node_p = new_root = old_root->left;
	old_root->left = new_root->right;
	new_root->right = old_root;
    } else {
	new_left = old_root->left;
	*node_p = new_root = new_left->right;
	old_root->left = new_root->right;
	new_left->right = new_root->left;
	new_root->left = new_left;
	new_root->right = old_root;
	compute_height(new_left);
    }
    compute_height(old_root);
    compute_height(new_root);
}

static void 
avl_walk_forward(node, func)
avl_node *node;
void (*func)();
{
    if (node != NIL(avl_node)) {
	avl_walk_forward(node->left, func);
	(*func)(node->key, node->value);
	avl_walk_forward(node->right, func);
    }
}


static void 
avl_walk_backward(node, func)
avl_node *node;
void (*func)();
{
    if (node != NIL(avl_node)) {
	avl_walk_backward(node->right, func);
	(*func)(node->key, node->value);
	avl_walk_backward(node->left, func);
    }
}


void 
avl_foreach(tree, func, direction)
avl_tree *tree;
void (*func)();
int direction;
{
    if (direction == AVL_FORWARD) {
	avl_walk_forward(tree->root, func);
    } else {
	avl_walk_backward(tree->root, func);
    }
}


static void
free_entry(node, key_free, value_free)
avl_node *node;
void (*key_free)();
void (*value_free)();
{
    if (node != NIL(avl_node)) {
	free_entry(node->left, key_free, value_free);
	free_entry(node->right, key_free, value_free);
	if (key_free != 0) (*key_free)(node->key);
	if (value_free != 0) (*value_free)(node->value);
	FREE(node);
    }
}
    

void 
avl_free_table(tree, key_free, value_free)
avl_tree *tree;
void (*key_free)();
void (*value_free)();
{
    free_entry(tree->root, key_free, value_free);
    FREE(tree);
}


int 
avl_count(tree)
avl_tree *tree;
{
    return tree->num_entries;
}

static avl_node *
new_node(key, value)
char *key;
char *value;
{
    register avl_node *new;

    new = ALLOC(avl_node, 1);
    new->key = key;
    new->value = value;
    new->height = 0;
    new->left = new->right = NIL(avl_node);
    return new;
}


int 
avl_numcmp(x, y)
char *x, *y; 
{
    return (ptrint) x - (ptrint) y;
}

int
avl_check_tree(tree)
avl_tree *tree;
{
    int error = 0;
    (void) do_check_tree(tree->root, tree->compar, &error);
    return error;
}


static int
do_check_tree(node, compar, error)
avl_node *node;
int (*compar)();
int *error;
{
    int l_height, r_height, comp_height, bal;
    
    if (node == NIL(avl_node)) {
	return -1;
    }

    r_height = do_check_tree(node->right, compar, error);
    l_height = do_check_tree(node->left, compar, error);

    comp_height = MAX(l_height, r_height) + 1;
    bal = r_height - l_height;
    
    if (comp_height != node->height) {
	(void) printf("Bad height for 0x%08x: computed=%d stored=%d\n",
	    node, comp_height, node->height);
	++*error;
    }

    if (bal > 1 || bal < -1) {
	(void) printf("Out of balance at node 0x%08x, balance = %d\n", 
	    node, bal);
	++*error;
    }

    if (node->left != NIL(avl_node) && 
		    (*compar)(node->left->key, node->key) > 0) {
	(void) printf("Bad ordering between 0x%08x and 0x%08x", 
	    node, node->left);
	++*error;
    }
    
    if (node->right != NIL(avl_node) && 
		    (*compar)(node->key, node->right->key) > 0) {
	(void) printf("Bad ordering between 0x%08x and 0x%08x", 
	    node, node->right);
	++*error;
    }

    return comp_height;
}
