#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#endif

#include "alpha_opcode.h"
#include "hal.h"
#include "operator.h"

/* hash_table */
#define OPER_HASH_SIZE	(1109)	/* should be prime... is this? */
#define CHIP_OPER_HASH_SIZE (101)	/* should be prime... is this? */
static OPID* operator_hash;
static OPID* ev4_operator_hash;
static OPID* ev5_operator_hash;
static OPID* ev6_operator_hash;

#define NUM_OPER	(1024)
OPERATOR* operator;		/* array of defined operators... includes macros */
static int opnum,numoperalloc;
OPERATOR* ev4_operator;        /* create seperate arrays for chip specific instructions */
OPERATOR* ev5_operator;        /* create seperate arrays for chip specific instructions */
OPERATOR* ev6_operator;        /* create seperate arrays for chip specific instructions */

#define NUM_MACRO_DEFNS	(1024)
MACRODEF** macro_defn;		/* array of macro definitions */
static int nummacroalloc;
int macronum;

typedef struct defoper_ DEFOPER;
struct defoper_ {
    char* name;
    int opcode;
    int type;
};

/*
 * Load default opcodes.  Only in an include file so this file is easier
 * to read.
 */
#include "defoper.h"

void operator_init()
{
    numoperalloc = 0;
    nummacroalloc = 0;

    return;
}

void operator_uninit()
{
    int i;
    for (i = 0; i < macronum; ++i)
        free_macrodef(macro_defn[i]);

    FREE(operator_hash);
    FREE(ev4_operator_hash);
    FREE(ev5_operator_hash);
    FREE(ev6_operator_hash);
    FREE(operator);
    FREE(ev4_operator);
    FREE(ev5_operator);
    FREE(ev6_operator);
    FREE(macro_defn);
    
    return;
}

int ascii_hash(char *s, int m)
{
  unsigned int junk = 0;
  
  while (*s)
      junk += *(s++);

  return junk % m;
}

void insert_operator(OPID nop)
{
    int hashval = ascii_hash(operator[nop].name, OPER_HASH_SIZE);
    OPID nid,*lid;

    if (operator_hash[hashval] == ID_NULL) {
        /* insert operator as head of this hash element */
        operator_hash[hashval] = nop;
        return;
    }

    
    /* head of this element is occupied, search for end of list. */
    lid = NULL;
    nid = operator_hash[hashval];
    while (nid != ID_NULL) {
        lid = &operator[nid].next;
        nid = operator[nid].next;
    }

    *lid = nop;

    return;
}

void insert_chip_operator(int impl, OPID nop)
{
    int hashval;
    OPID nid,*lid, *chip_hash;
    OPERATOR *chip_op;

    switch (impl) {
    case EV4:
      chip_hash = ev4_operator_hash;
      chip_op = ev4_operator;
      break;
    case EV5:
      chip_hash = ev5_operator_hash;
      chip_op = ev5_operator;
      break;
    case EV6:
      chip_hash = ev6_operator_hash;
      chip_op = ev6_operator;
    }    

    hashval = ascii_hash(chip_op[nop].name, CHIP_OPER_HASH_SIZE);
    
    if (chip_hash[hashval] == ID_NULL) {
        /* insert operator as head of this hash element */
        chip_hash[hashval] = nop;
        return;
    }

    /* head of this element is occupied, search for end of list. */
    lid = NULL;
    nid = chip_hash[hashval];
    while (nid != ID_NULL) {
        lid = &chip_op[nid].next;
        nid = chip_op[nid].next;
    }

    *lid = nop;

    return;
}

void delete_operator(OPID oid)
{
    int hashval = ascii_hash(operator[oid].name, OPER_HASH_SIZE);
    OPID nid,*lid;

    if (operator_hash[hashval] == ID_NULL)
        /* operator doesn't exist, simply return. */
        return;

    /* head of this element is occupied, search for matching name. */
    lid = &operator_hash[hashval];
    nid = operator_hash[hashval];
    while ((nid != ID_NULL) && (strcmp(operator[oid].name, operator[nid].name) != 0)) {
        lid = &operator[nid].next;
        nid = operator[nid].next;
    }

    if (nid == ID_NULL)
        /* operator doesn't exist, simply return. */
        return;

    /* remove element from list */
    *lid = operator[oid].next;

    /* clear out next link in operator that was just deleted to allow for reuse. */
    operator[oid].next = ID_NULL;

    return;
}

OPID find_operator(char* opname_in, int return_name)
{
    char *orig_opname, *opname;
    char *tstr;
    int hashval; 
    int oid, curr_oid;

    if (return_name)
        opname = opname_in;
    else
        opname = strcpy(alloca(strlen(opname_in)+1), opname_in);
    
    orig_opname = opname;
    
    for (tstr=opname; *opname != '\0'; ++opname)
        if (*opname != '/')
            *(tstr++) = tolower(*opname);

    *tstr = '\0';
    
    /* Search for operator in default table */
    hashval = ascii_hash(orig_opname, OPER_HASH_SIZE);
    for (oid = operator_hash[hashval]; oid != ID_NULL; oid = operator[oid].next) 
        /* return value if the operator name matches */
      if (strcmp(operator[oid].name, orig_opname) == 0)
        return oid;

    /* If not found search for operator in chip specific operators*/
    hashval = ascii_hash(orig_opname, CHIP_OPER_HASH_SIZE);
    if (chip == EV4) {
      for (oid = ev4_operator_hash[hashval]; oid != ID_NULL; oid = ev4_operator[oid].next) 
        /* return value if the operator name matches */
        if (strcmp(ev4_operator[oid].name, orig_opname) == 0)
          return NUM_DEFOPER + oid;
    }
    else if (chip == EV5) {
      for (oid = ev5_operator_hash[hashval]; oid != ID_NULL; oid = ev5_operator[oid].next) 
        /* return value if the operator name matches */
        if (strcmp(ev5_operator[oid].name, orig_opname) == 0)
          return NUM_DEFOPER + NUM_EV4OPER + oid;
    }
    else { /* EV6 */
      for (oid = ev6_operator_hash[hashval]; oid != ID_NULL; oid = ev6_operator[oid].next) 
        /* return value if the operator name matches */
        if (strcmp(ev6_operator[oid].name, orig_opname) == 0)
          return NUM_DEFOPER + NUM_EV4OPER + NUM_EV5OPER + oid;
    }
    
    /* Not Found */
    return ID_NULL;
}
    
void
load_default_operators()
{
    int i,max;
    DEFOPER *opers;

    opnum = 0;
    operator_hash = (OPID*)malloc(sizeof(OPID)*OPER_HASH_SIZE);
    ev4_operator_hash = (OPID*)malloc(sizeof(OPID)*CHIP_OPER_HASH_SIZE);
    ev5_operator_hash = (OPID*)malloc(sizeof(OPID)*CHIP_OPER_HASH_SIZE);
    ev6_operator_hash = (OPID*)malloc(sizeof(OPID)*CHIP_OPER_HASH_SIZE);
    for (i=0; i<OPER_HASH_SIZE; ++i)  
        operator_hash[i] = ID_NULL;
    for (i=0; i<CHIP_OPER_HASH_SIZE; ++i) { 
        ev4_operator_hash[i] = ID_NULL;
        ev5_operator_hash[i] = ID_NULL;
        ev6_operator_hash[i] = ID_NULL;
    }

    /* allocate enough operators for the default ones plus 1024 user
     * defined ones */
    numoperalloc = NUM_DEFOPER + NUM_EV4OPER + NUM_EV5OPER + NUM_EV6OPER + NUM_OPER;
        
    operator = (OPERATOR*)malloc(sizeof(OPERATOR)*numoperalloc);
    for (i=0; i<numoperalloc; ++i) {
        operator[i].name = NULL;
        operator[i].next = ID_NULL;
    }

    macronum = 0;
    macro_defn = (MACRODEF**)calloc(NUM_MACRO_DEFNS, sizeof(MACRODEF*));

    /* insert the default operators */
    for (i=0; i<NUM_DEFOPER; ++i) {
        int hashval;
        
        /* setup the operator.
         *
         * we know that we have enough operator entries, so we don't need to
         * check.
         *
         * just copy the pointer to the name string, because the default
         * opcode table should not change during a run */
        operator[opnum].name = def_opcodes[i].name;
        operator[opnum].type = def_opcodes[i].type;
        operator[opnum].value = def_opcodes[i].opcode;

        /* put this operator in the hash table */
        insert_operator(opnum);

        /* increment operator table pointer */
        ++opnum;
    }

    /* and the chip specific ones */
    /* Note: THese are placed in two locations.  The default table and a special
       Chip Specific Table.  The latter table is hashed.  The former is only used
       when the ID is known. */
    ev4_operator = (OPERATOR*)malloc(sizeof(OPERATOR)*NUM_EV4OPER);
    for (i=0; i<NUM_EV4OPER; ++i)
        ev4_operator[i].next = ID_NULL;
    for (i=0; i<NUM_EV4OPER; ++i) {
        int hashval;
        
        operator[opnum].name = ev4_opcodes[i].name;
        operator[opnum].type = ev4_opcodes[i].type;
        operator[opnum].value = ev4_opcodes[i].opcode;
        ++opnum;
        
        /* setup the operator.
         *
         * we know that we have enough operator entries, so we don't need to
         * check.
         *
         * just copy the pointer to the name string, because the default
         * opcode table should not change during a run */
        ev4_operator[i].name = ev4_opcodes[i].name;
        ev4_operator[i].type = ev4_opcodes[i].type;
        ev4_operator[i].value = ev4_opcodes[i].opcode;

        /* put this operator in the hash table */
        insert_chip_operator(EV4,i);
    }

    ev5_operator = (OPERATOR*)malloc(sizeof(OPERATOR)*NUM_EV5OPER);
    for (i=0; i<NUM_EV5OPER; ++i)
        ev5_operator[i].next = ID_NULL;
    for (i=0; i<NUM_EV5OPER; ++i) {
        int hashval;
        
        operator[opnum].name = ev4_opcodes[i].name;
        operator[opnum].type = ev4_opcodes[i].type;
        operator[opnum].value = ev4_opcodes[i].opcode;
        ++opnum;
        
        /* setup the operator.
         *
         * we know that we have enough operator entries, so we don't need to
         * check.
         *
         * just copy the pointer to the name string, because the default
         * opcode table should not change during a run */
        ev5_operator[i].name = ev5_opcodes[i].name;
        ev5_operator[i].type = ev5_opcodes[i].type;
        ev5_operator[i].value = ev5_opcodes[i].opcode;

        /* put this operator in the hash table */
        insert_chip_operator(EV5,i);
    } 

    ev6_operator = (OPERATOR*)malloc(sizeof(OPERATOR)*NUM_EV6OPER);
    for (i=0; i<NUM_EV6OPER; ++i)
        ev6_operator[i].next = ID_NULL;
    for (i=0; i<NUM_EV6OPER; ++i) {
        int hashval;
        
        operator[opnum].name = ev6_opcodes[i].name;
        operator[opnum].type = ev6_opcodes[i].type;
        operator[opnum].value = ev6_opcodes[i].opcode;
        ++opnum;
        
        /* setup the operator.
         *
         * we know that we have enough operator entries, so we don't need to
         * check.
         *
         * just copy the pointer to the name string, because the default
         * opcode table should not change during a run */
        ev6_operator[i].name = ev6_opcodes[i].name;
        ev6_operator[i].type = ev6_opcodes[i].type;
        ev6_operator[i].value = ev6_opcodes[i].opcode;

        /* put this operator in the hash table */
        insert_chip_operator(EV6,i);
    }

    return;
}

OPID create_operator(char* name, int type, int value)
{
    /* check that we have a free operator */
    if (opnum == numoperalloc) {
        /* no free operator, realloc the array to be larger */
        numoperalloc += NUM_OPER;
        operator = (OPERATOR*)realloc(operator, sizeof(OPERATOR)*numoperalloc);
    }
    
    operator[opnum].name = name;
    operator[opnum].type = type;
    operator[opnum].value = value;
    operator[opnum].next = ID_NULL;

    return opnum++;    
}

OPID create_macro_defn(MACRODEF* mtmp)
{
    /* check that we have a free macro_defn */
    if (macronum == nummacroalloc) {
        /* no free macro_defn, realloc the array to be larger */
        nummacroalloc += NUM_MACRO_DEFNS;
        macro_defn = (MACRODEF**)realloc(macro_defn, sizeof(MACRODEF*)*nummacroalloc);
    }
    
    macro_defn[macronum] = mtmp;
    
    return macronum++;    
}

#ifdef __STDC__
int is_macro(OPID oid)
#else
int is_macro(oid)
OPID oid;
#endif
{
  return (operator[oid].type == MACRO_CALL);
}

void free_operators()
{
    free(operator_hash);
    free(ev4_operator_hash);
    free(ev5_operator_hash);
    free(ev6_operator_hash);
    free(operator);
    free(ev4_operator);
    free(ev5_operator);
    free(ev6_operator);
    free(macro_defn);
}
