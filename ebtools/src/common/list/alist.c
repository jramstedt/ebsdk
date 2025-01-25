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
/*
**
** FACILITY:	
**
**	ED64 Software Tools - alist
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Generate a list file from a a.out image file.  This
**      module also acts as an image post processor for input
**      into the Palcode Violation Checker.
** 
** CALLING ENVIRONMENT: 
**
**	user mode
** 
** AUTHOR: David A Rusling
**
** CREATION-DATE: 05-NOV-1992
** 
** MODIFIED BY: 
** 
**      Who	When		What
**	-----	-----------	---------------------------------------------
** 	DAR	   JUL-1993	Modified for 32 bit use.
**	DAR	   Nov-1992	First version.
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "stdc.h"
#include "aout.h"

/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000

#define _debug(name,string)\
     {if (_OPTION('d')) \
	printf("debug: %s %s\n", name, string);	\
     }

#define _ddebug(name,string,number)\
     {if (_OPTION('d')) \
	printf("debug: %s %s %d\n", name, string,number);	\
     }

#define _error(string1,string2)\
     {	\
	printf("error: %s %s\n", string1, string2);	\
	exit(1);    \
     }

#define _SEEN(o) seen[o-'a']
#define _OPTION(o) (_SEEN(o) == TRUE)


/*
 *  Global data (all names preceded by 'coff_g'.
 */
FILE *coff_g_infile;
char *coff_g_buffer = NULL;
unsigned int coff_g_buffer_size = 0;
#define SEEN_SIZE 100
char seen[SEEN_SIZE];			/* upper and lower case */
/*
 *  Forward routine descriptions.
 */
int main(int argc, char **argv);
void usage();
void read_file(char *filename);
void analyse_aout_file();
void dump_symbols(struct nlist *n, vm_size_t size);
void dump_text(vm_offset_t physical, int32 *p, vm_size_t size);
void dump_data(vm_size_t physical, int32 *p, vm_size_t size);
void dump_relocation(char *type, char *r, vm_size_t size);
char *symbol_name_from_index(unsigned int m_index);
char *symbol_name_from_va(vm_offset_t va);
char *symbol_name_from_value(vm_offset_t value);
void dump_entry_points(struct nlist *n, vm_size_t size);
void print_symbols_at_address(vm_offset_t address);
void dump_pvc_map(struct nlist *n, vm_size_t size);
void print_reloc_type(char type);
unsigned int filter_entry_points(char *name);
void print_64bit_value(char *start, integer_t value, int hex, char *end);
/*
 *  External routines.
 */
void print_instruction(int instruction, FILE *stream);

int main(int argc, char **argv)
{
    char *in_file_name = "a.out";	/* default .o file */
    char *arg, option;
    int i;

    for (i = 0; i < SEEN_SIZE; i++)
	seen[i] = FALSE;
/*
 * Parse arguments, but we are only interested in flags.
 * Skip argv[0].
 */
    for (i = 1; i < argc; i++) {
	_debug("main()", "argument loop");
	arg = argv[i];
	if (*arg == '-') {
/*
 * This is a -xyz style options list.  Work out the options specified.
 */
	    arg++;			/* skip the '-' */
	    while (option = *arg++) {	/* until we reach the '0' string
					 * terminator */
		switch (option) {
		    case 'h': 
		    case 'H': 
			usage();
			exit(1);
		    case 'd': 		/* debug, allow upper and lower case */
		    case 'D': 
		    case 'v': 		/* verbose, allow upper and lower case
					 */
		    case 'V': 
		    case 'e': 		/* print *only* entry point information
					 */
		    case 'E': 
		    case 'm': 		/* print *only* PVC information */
		    case 'M': 
		    case 'f': 		/* print full information (no skipping 
					 * zero locations) */
		    case 'F': 
			_SEEN(tolower(option)) = TRUE;
			break;
		    default: 
			usage();
			exit(0);
			break;
		}
	    }
	} else {
/*
 *  This is a filename, ignore multiple filenames, just take the 
 *  last one specified.  This is not a rolls-royce interface.
 */
	    in_file_name = arg;
	    _debug("main(), filename is ", in_file_name);
	}
    }

    if (!_OPTION('e'))
	printf("a.out file dumper [V2.0]\n");
/*
 *  Read in the coff file and save it in memory before formatting.
 */
    read_file(in_file_name);
    if (!_OPTION('e'))
	printf("Dump of a.out object file %s of size %d bytes\n", 
	  in_file_name, 
	  coff_g_buffer_size);

/*
 *  Now go and analyse the file contents.
 */
    analyse_aout_file();

    return EXIT_SUCCESS;
}					/* end of main() */

void usage()
{
    printf("alist - description\n");
    printf("\tDump the contents of an a.out format object file\n");
    printf("alist - usage : alist -dDvVeEmMhH <filename>\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\td,D = turn on debug mode\n");
    printf("\tv,V = turn on verbose mode\n");
    printf("\te,E = print entry points (for PVC)\n");
    printf("\tm,M = print map information (for PVC)\n");
    printf("\tf,F = print full information (no skipping zero locations)\n");
    printf("\th,H = print this help file\n");
}
void read_file(char *filename)
{
    char *newbuffer;
    unsigned int size;

    if ((coff_g_infile = fopen(filename, "rb")) == NULL)
	_error("cannot open file ", filename);
/*
 *  Read the file into memory, block by block, allocating as we
 *  go along.
 */
    while (TRUE) {
	newbuffer = (char *) malloc(coff_g_buffer_size + BLOCK_SIZE);
	if (coff_g_buffer_size != 0) {
	    memcpy(newbuffer, coff_g_buffer, coff_g_buffer_size);
	    free(coff_g_buffer);
	}
	size = 
	  fread(newbuffer + coff_g_buffer_size, sizeof(char) , BLOCK_SIZE, 
	  coff_g_infile);
	coff_g_buffer = newbuffer;
	if (size == 0)
	    return;			/* we've finished */
	else {
	    coff_g_buffer_size = coff_g_buffer_size + size;
	}
    }
}
void analyse_aout_file()
{
    struct exec *head;

    head = (struct exec *) coff_g_buffer;

    if (_OPTION('v')) {

	print_64bit_value("\ta_magic\t\t= 0x", head->a_magic, TRUE, "\n");
	print_64bit_value("\ta_text\t\t= ", 
	  head->a_text, FALSE, " (bytes of text in the file)\n");
	print_64bit_value("\ta_data\t\t= ", 
	  head->a_data, FALSE, " (bytes of data in the file)\n");
	print_64bit_value("\ta_bss\t\t= 0x", 
	  head->a_bss, FALSE, " (bytes of auto-zeroed data in the file)\n");
	print_64bit_value("\ta_syms\t\t= ", 
	  head->a_syms, FALSE, " (bytes of symbol table data in the file)\n");
	print_64bit_value("\ta_entry\t\t= 0x", 
	  head->a_entry, TRUE, " (start PC)\n");
	print_64bit_value("\ta_tstart\t= ", 
	  head->a_tstart, FALSE, " (text start, in memory)\n");
	print_64bit_value("\ta_dstart\t= ", 
	  head->a_dstart, FALSE, " (data start, in memory)\n");
	print_64bit_value("\ta_trsize\t= ", 
	  head->a_trsize, FALSE, 
	  " (bytes of text-relocation information in file)\n");
	print_64bit_value("\ta_drsize\t= ", 
	  head->a_drsize, FALSE, 
	  " (bytes of data-relocation information in file)\n");
    }

    if (!_OPTION('e') && !_OPTION('m')) {
	printf("\ttext offset = %d\n", N_TXTOFF(*head));
	printf("\tdata offset = %d\n", N_DATOFF(*head));
	printf("\ttext relocation offset = %d\n", N_TRELOFF(*head));
	printf("\tdata relocation offset = %d\n", N_DRELOFF(*head));
	printf("\tsymbols offset = %d\n", N_SYMOFF(*head));
    }

    if (!_OPTION('e') && !_OPTION('m')) {
	if neq64(head->a_text, zero)
	    dump_text(head->a_tstart, (int32 *) (coff_g_buffer + 
	      N_TXTOFF(*head)), head->a_text);
	if neq64(head->a_data, zero)
	    dump_data(head->a_text, (int32 *) (coff_g_buffer + 
	      N_DATOFF(*head)), head->a_data);
	if neq64(head->a_trsize, zero)
	    dump_relocation("text", 
	      (coff_g_buffer + 
	      N_TRELOFF(*head)), head->a_trsize);
	if neq64(head->a_drsize, zero)
	    dump_relocation("data", 
	      (coff_g_buffer + 
	      N_DRELOFF(*head)), head->a_trsize);
	if neq64(head->a_syms, zero)
	    dump_symbols((struct nlist *) (coff_g_buffer + N_SYMOFF(*head)), 
	      head->a_syms);
    }
/*
 *  Don't dump entry points if we only want the map (PVC) stuff.
 */
    if (_OPTION('e'))
	dump_entry_points((struct nlist *) (coff_g_buffer + N_SYMOFF(*head)), 
	  head->a_syms);
/*
 *  Dump only the PVC symbols and their values (used by PVC as the map file).
 */
    if (_OPTION('m'))
	dump_pvc_map((struct nlist *) (coff_g_buffer + N_SYMOFF(*head)), 
	  head->a_syms);

}
void dump_symbols(struct nlist *n, vm_size_t size)
{
    struct nlist *end;
/*
 *  Print out the symbols.
 */

    printf("Symbols\n");
    end = (struct nlist *) ((char *) n + low32(size));
    for (; n < end; n++) {
	printf("\t");
	switch (n->n_type & (0xFE)) {
	    case N_UNDF: 
		printf("UNDF");
		break;
	    case N_ABS: 
		printf("ABS");
		break;
	    case N_TEXT: 
		printf("TEXT");
		break;
	    case N_DATA: 
		printf("DATA");
		break;
	    case N_BSS: 
		printf("BSS");
		break;
	    case N_COMM: 
		printf("COMM");
		break;
	    case N_FN: 
		printf("FN");
		break;
	    case N_EXT: 
		printf("EXT");
		break;
	    case N_TYPE: 
		printf("TYPE");
		break;
	    case N_STAB: 
		printf("STAB");
		break;
	    default: 
		printf("%d", n->n_type);
	}
	if (n->n_type & N_EXT)
	    printf(",EXT");
	else
	    printf("\t");
	printf("\tother = 0x%08X  ", n->n_other);
	printf("desc = 0x%08X  ", n->n_desc);
	print_64bit_value("value = 0x", n->n_value, TRUE, "  ");
	if (n->n_un.n_name != NULL)
	    printf("%s", (char *) end + (unsigned int) n->n_un.n_name);
	printf("\n");
    }
}
void dump_text(vm_offset_t physical, int32 *p, vm_size_t size)
{
    unsigned int i, zeros = 0, s;
    char *ref, *symbol;

    printf("Executable Code\n");
    s = low32(size) / sizeof(int32);
    for (i = 0; i < s; i++) {
/*
 *  Print out the symbols at this address.  Make sure we code with no
 *  instructions at that address.
 */
	symbol = symbol_name_from_value(physical);
	if (symbol != NULL) {
	    if (p[i] == 0) {
		print_64bit_value("\t[", physical, TRUE, "] ");
		printf("0x%08X\n", p[i]);
	    }
	    print_symbols_at_address(physical);
	}

	if (p[i] == 0) {
	    if (zeros == 0)
		printf("\t\t:\n\tzero locations skipped\n\t\t:\n");
	    zeros++;
	} else {
	    print_64bit_value("\t[", physical, TRUE, "] ");
	    printf("0x%08X ", p[i]);
	    print_instruction((int) p[i], stdout);
	    ref = symbol_name_from_va(physical);
	    if (ref != NULL)
		printf("\t(%s)", ref);
	    printf("\n");
	    zeros = 0;
	}
	physical = plus_a_32(physical, 4);
    }
}
/* 
 *  Dump the data out in 32 bit quantities, adding in any 
 *  appropriate symbolic information.
 */
void dump_data(vm_size_t physical, int32 *p, vm_size_t size)
{
    unsigned int i, zeros = 0, s;
    char *symbol;
/* 
 *  Cope with cross compilation.
 */
    printf("Data\n");
    s = low32(size) / sizeof(int32);
    for (i = 0; i < s; i++) {
/*
 *  Print out the symbols at this address.  Make sure we cope with null
 *  data at that address.
 */
	symbol = symbol_name_from_value(physical);
	if (symbol != NULL) {
	    if (!_OPTION('f')) {
		if (p[i] == 0) {
		    print_64bit_value("\t[", physical, TRUE, "] ");
		    printf("0x%08X\n", p[i]);
		}
	    }
	    print_symbols_at_address(physical);
	}

	if (_OPTION('f')) {
	    printf("\t[0x%08X] 0x%08X\n", physical, p[i]);
	} else {
	    if (p[i] == 0) {
		if (zeros == 0)
		    printf("\t\t:\n\tzero locations skipped\n\t\t:\n");
		zeros++;
	    } else {
		printf("\t[0x%08X] 0x%08X\n", physical, p[i]);
		zeros = 0;
	    }
	}
	physical = plus_a_32(physical, 4);
    }
}

void dump_relocation(char *type, char *r, vm_size_t size)
{
    char *symbol_name;
    char *end;

    printf("Relocation Information (%s)\n", type);
    end =  r + low32(size);
    for (; r < end; r+= RELOCATION_INFO_SIZE) {
	symbol_name = symbol_name_from_index(*(unsigned int *) (r+R_INDEX_OFFSET));
	print_64bit_value("\tr_address = 0x",*(vm_offset_t *)(r+R_ADDRESS_OFFSET), TRUE, ", ");
	printf("r_index = %d (%s), r_extern = %d, ", 
	  *(unsigned int *) (r+R_INDEX_OFFSET), 
	  symbol_name == NULL ? "UNKNOWN" : symbol_name, r[R_EXTERN_OFFSET]);
	printf("r_type = %d ( ", (enum reloc_type) (r[R_TYPE_OFFSET]));
	print_reloc_type(r[R_TYPE_OFFSET]);
	print_64bit_value(" ),\n\t r_addend = ", *(integer_t *) (r+R_ADDEND_OFFSET), FALSE, "\n");
    }
}

char *symbol_name_from_index(unsigned int m_index)
{
    struct exec *exec;
    struct nlist *n, *end;

/*
 *  first, figure out where the symbols start and where the
 *  strings start also.
 */

    exec = (struct exec *) coff_g_buffer;
    n = (struct nlist *) (coff_g_buffer + N_SYMOFF(*exec));
    end = (struct nlist *) ((char *) n + low32(exec->a_syms));
    n = n + m_index;
    if (n->n_un.n_name != NULL)
	return ((char *) end + (unsigned int) n->n_un.n_name);
    else
	return (NULL);
}

char *symbol_name_from_va(vm_offset_t va)
{
    struct exec *exec;
    char *r, *end;

    exec = (struct exec *) coff_g_buffer;
    if neq64(exec->a_trsize, zero) {
	r = (char *) (coff_g_buffer + N_TRELOFF(*exec));
	end = (char *) ((char *) r + low32(exec->a_trsize));
	for (; r < end; r+=RELOCATION_INFO_SIZE ) {
	    if eq64(*(vm_offset_t *)(r+R_ADDRESS_OFFSET), va)
		return symbol_name_from_index(*(unsigned int *) (r+R_INDEX_OFFSET));
	}
    }
    return (NULL);
}
char *symbol_name_from_value(vm_offset_t value)
{
    struct exec *exec;
    struct nlist *n, *end;

/*
 *  first, figure out where the symbols start and where the
 *  strings start also.
 */

    exec = (struct exec *) coff_g_buffer;
    n = (struct nlist *) (coff_g_buffer + N_SYMOFF(*exec));
    end = (struct nlist *) ((char *) n + low32(exec->a_syms));
    for (; n < end; n++) {
	if eq64(n->n_value, value) {
	    if (n->n_un.n_name != NULL)
		return ((char *) end + (unsigned int) n->n_un.n_name);
	    else
		return (NULL);
	}
    }
    return (NULL);
}

void dump_entry_points(struct nlist *n, vm_size_t size)
{
    struct nlist *end;
    char *name;
    unsigned int type;
/*
 *  Print out the entry points (use the symbol entrys
 *  to find them.
 */

    if (!_OPTION('e'))
	printf("Entry Points\n");
    end = (struct nlist *) ((char *) n + low32(size));
    for (; n < end; n++) {
	if (n->n_un.n_name != NULL) {
	    name = (char *) end + (unsigned int) n->n_un.n_name;
	    if (filter_entry_points(name) != TRUE) {
		type = n->n_type & 0xFE;
		if (type == N_TEXT) {
		    if (!_OPTION('e'))
			printf("\t");
		    print_64bit_value("", n->n_value, TRUE, "\t");
		    printf("%s\n", name);
		}
	    }
	}
    }
}

void dump_pvc_map(struct nlist *n, vm_size_t size)
{
    struct nlist *end;
    char *name;
/*
 *  Print out the PVC entry points (use the symbol entrys
 *  to find them).
 */

    end = (struct nlist *) ((char *) n + low32(size));
    for (; n < end; n++) {
	if (n->n_un.n_name != NULL) {
	    name = (char *) end + (unsigned int) n->n_un.n_name;
	    if ((strstr(name, "PVC$") != NULL) || 
	      (strstr(name, "pvc$") != NULL)) {
		printf("%s ", name);
		print_64bit_value("", n->n_value, TRUE, "\n");
	    }
	}
    }
}

void print_symbols_at_address(vm_offset_t address)
{
    struct exec *exec;
    struct nlist *n, *end;
/*
 *  first, figure out where the symbols start and where the
 *  strings start also.
 */

    exec = (struct exec *) coff_g_buffer;
    n = (struct nlist *) (coff_g_buffer + N_SYMOFF(*exec));
    end = (struct nlist *) ((char *) n + low32(exec->a_syms));
/*
 *  Now print out all of the symbols at this address (there may
 *  be more than one).
 */
    for (; n < end; n++) {
	if eq64(n->n_value, address)
	    if (n->n_un.n_name != NULL)
		printf("%s:\n",
		  (char *) end + (unsigned int) n->n_un.n_name);
    }
}

void print_reloc_type(char type)
{
    switch (type) {
	case RELOC_0_7: 
	    printf("RELOC_0_7");
	    break;
	case RELOC_0_15: 
	    printf("RELOC_0_15");
	    break;
	case RELOC_0_31: 
	    printf("RELOC_0_31");
	    break;
	case RELOC_0_63: 
	    printf("RELOC_0_63");
	    break;
	case RELOC_0_25: 
	    printf("RELOC_0_25");
	    break;
	case RELOC_16_31: 
	    printf("RELOC_16_31");
	    break;
	case RELOC_WDISP21: 
	    printf("RELOC_WDISP21");
	    break;
	case RELOC_BASE21: 
	    printf("RELOC_BASE21");
	    break;
	case RELOC_WDISP14: 
	    printf("RELOC_WDISP14");
	    break;
	case RELOC_BASE14: 
	    printf("RELOC_BASE14");
	    break;
	case RELOC_32_47: 
	    printf("RELOC_32_47");
	    break;
	case RELOC_48_63: 
	    printf("RELOC_48_63");
	    break;
	case RELOC_U_16_31: 
	    printf("RELOC_U_16_31");
	    break;
	case RELOC_U_32_47: 
	    printf("RELOC_U_32_47");
	    break;
	case RELOC_U_48_63: 
	    printf("RELOC_U_48_63");
	    break;
	case RELOC_0_12: 
	    printf("RELOC_0_12");
	    break;
	case RELOC_0_8: 
	    printf("RELOC_0_8");
	    break;
	case NO_RELOC: 
	    printf("NO_RELOC");
	    break;
	case BRELOC_0_15: 
	    printf("BRELOC_0_15");
	    break;
	case BRELOC_0_31: 
	    printf("BRELOC_0_31");
	    break;
	case BRELOC_16_31: 
	    printf("BRELOC_16_31");
	    break;
	case BRSET_0_15: 
	    printf("BRSET_0_15");
	    break;
	case BRSET_16_31: 
	    printf("BRSET_16_31");
	    break;
    }
}

/*
 * Filter out unwanted entry points (generated by the
 * -e option.  Ignore PVC$ symbols and a few peculiar
 * symbols.  Return TRUE if the symbol should be
 * filtered.
 */
unsigned int filter_entry_points(char *name)
{
    if (strstr(name, "PVC$") != NULL)
	return (TRUE);
    if (strstr(name, "pvc$") != NULL)
	return (TRUE);
    if (strstr(name, "vmunixArg") != NULL)
	return (TRUE);
    if (strstr(name, "_etext") != NULL)
	return (TRUE);
    return (FALSE);
}

/*
 *  Print out the value of the 64 bit thing passed.
 *  This does different things on different architecture
 *  types.
 */
void print_64bit_value(char *start, integer_t value, int hex, char *end)
{
/*
 *  Always print out the start string
 */
    printf("%s", start);
/*
 *  If this is 32 bit land, then print out two 32 bit
 *  values.
 */
#ifdef CROSS_COMPILE_32_TO_64_BITS
    if (hex) {
	printf("%04X", high32(value));
	printf("%04X", low32(value));
    } else {
	printf("%d", low32(value));
    }
#else
    if (hex)
	printf("%08X", value);
    else
	printf("%d", value);
#endif
/*
 *  Always print out the end string
 */
    printf("%s", end);
}
