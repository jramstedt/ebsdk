/*-
 * This code is derived from software copyrighted by the Free Software
 * Foundation.
 *
 * Modified 1991 by Donn Seeley at UUNET Technologies, Inc.
 *
 * Heavily severed and modified by Alessandro Forin at CMU.
 * Now Alpha version only.
 */

#ifndef lint
static char sccsid[] = "@(#)ld.c	6.10 (Berkeley) 5/22/91";
#endif /* not lint */

/* Linker `ld' for GNU
   Copyright (C) 1988 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Richard Stallman with some help from Eric Albert.
   Set, indirect, and warning symbol features added by Randy Smith.  */
   
/* Define how to initialize system-dependent header fields.  */

#include <ar.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <string.h>

				/* COFF objects */
#include <machine/coff.h>
#include <machine/syms.h>

COFF_N_BADMAG(ex)
	struct exechdr *ex;
{
	return N_BADMAG(ex->a);
#undef	N_BADMAG
}

COFF_N_TXTOFF(ex)
	struct exechdr *ex;
{
  struct scnhdr *shdr;
  unsigned long int i;

  shdr = (struct scnhdr *)((char *)ex + sizeof(struct exechdr));
  for (i =0; i < ex->f.f_nscns; i++, shdr++) {
    if (strcmp(shdr->s_name, ".text") == 0)
	return(shdr->s_scnptr);
  }
#undef	N_TXTOFF
}


				/* AOUT objects */
#include <a.out.h>
#include <stab.h>

#define	BSD_N_BADMAG(x) \
 (((x)->a_magic)!=OMAGIC && ((x)->a_magic)!=NMAGIC && ((x)->a_magic)!=ZMAGIC)
#define BSD_TEXT_START(x) \
 N_TXTADDR(*x)
#define BSD_DATA_START(x) \
 N_DATADDR(*x)
#define	BSD_N_TXTOFF(x) \
 ((x)->a_magic == ZMAGIC ? 0 : sizeof(struct exec))
#define BSD_N_SYMOFF(x) \
 (BSD_N_TXTOFF(x) + (x)->a_text + (x)->a_data + (x)->a_trsize + (x)->a_drsize)
#define	BSD_N_STROFF(x) \
 (BSD_N_SYMOFF(x) + (x)->a_syms)

typedef union {
	struct exec	aout;
	struct exechdr	coff;
	char armag[SARMAG];
} object_header_t;

/* symseg.h defines the obsolete GNU debugging format; we should nuke it.  */
#define CORE_ADDR unsigned long	/* For symseg.h */
#include "symseg.h"

#define BSD_N_SET_MAGIC(exec, val)  ((exec)->a_magic = val)

/* If compiled with GNU C, use the built-in alloca */
#ifdef __GNUC__
#define alloca __builtin_alloca
#endif

#define min(a,b) ((a) < (b) ? (a) : (b))

/* Macro to control the number of undefined references printed */
#define MAX_UREFS_PRINTED	10

/* Size of a page; obtained from the operating system.  */

int page_size;

/* Name this program was invoked by.  */

char *progname;

/* System dependencies */

/* Define this to specify the default executable format.  */

#ifndef DEFAULT_MAGIC
#define DEFAULT_MAGIC ZMAGIC
#endif

/*
 * Ok.  Following are the relocation information macros.  If your
 * system should not be able to use the default set (below), you must
 * define the following:

 *   relocation_info: This must be typedef'd (or #define'd) to the type
 * of structure that is stored in the relocation info section of your
 * a.out files.  Often this is defined in the a.out.h for your system.
 *
 *   RELOC_ADDRESS (rval): Offset into the current section of the
 * <whatever> to be relocated.  *Must be an lvalue*.
 *
 *   RELOC_EXTERN_P (rval):  Is this relocation entry based on an
 * external symbol (1), or was it fully resolved upon entering the
 * loader (0) in which case some combination of the value in memory
 * (if RELOC_MEMORY_ADD_P) and the extra (if RELOC_ADD_EXTRA) contains
 * what the value of the relocation actually was.  *Must be an lvalue*.
 *
 *   RELOC_TYPE (rval): If this entry was fully resolved upon
 * entering the loader, what type should it be relocated as?
 *
 *   RELOC_SYMBOL (rval): If this entry was not fully resolved upon
 * entering the loader, what is the index of it's symbol in the symbol
 * table?  *Must be a lvalue*.
 *
 *   RELOC_MEMORY_ADD_P (rval): This should return true if the final
 * relocation value output here should be added to memory, or if the
 * section of memory described should simply be set to the relocation
 * value.
 *
 *   RELOC_ADD_EXTRA (rval): (Optional) This macro, if defined, gives
 * an extra value to be added to the relocation value based on the
 * individual relocation entry.  *Must be an lvalue if defined*.
 *
 *   RELOC_PCREL_P (rval): True if the relocation value described is
 * pc relative.
 *
 *   RELOC_BRREL_P (rval): True if the relocation value is an offset
 * of the base register [aka GP reg for MIPSco].
 *
 *   RELOC_BRSET_P (rval): True if the instruction loads the base register
 * with its value for the current file value (which must be relocated)
 *
 *   RELOC_MEMORY_BROFF(rval, rval): Current offset (from the base
 * register) in the object file, before relocation.
 *
 *   RELOC_VALUE_RIGHTSHIFT (rval): Number of bits right to shift the
 * final relocation value before putting it where it belongs.
 *
 *   RELOC_TARGET_SIZE (rval): log to the base 2 of the number of
 * bytes of size this relocation entry describes; 1 byte == 0; 2 bytes
 * == 1; 4 bytes == 2, and etc.  This is somewhat redundant (we could
 * do everything in terms of the bit operators below), but having this
 * macro could end up producing better code on machines without fancy
 * bit twiddling.  Also, it's easier to understand/code big/little
 * endian distinctions with this macro.
 *
 *   RELOC_TARGET_BITPOS (rval): The starting bit position within the
 * object described in RELOC_TARGET_SIZE in which the relocation value
 * will go.
 *
 *   RELOC_TARGET_BITSIZE (rval): How many bits are to be replaced
 * with the bits of the relocation value.  It may be assumed by the
 * code that the relocation value will fit into this many bits.  This
 * may be larger than RELOC_TARGET_SIZE if such be useful.
 *
 *
 *		Things I haven't implemented
 *		----------------------------
 *
 *    Values for RELOC_TARGET_SIZE other than 0, 1, 2 or 3.
 *
 *    Pc relative relocation for External references.
 *
 *
 */

#define RELOC_ADDRESS(r)		((r)->r_address)
#define RELOC_EXTERN_P(r)               ((r)->r_extern)
#define RELOC_TYPE(r)                   ((r)->r_index)
#define RELOC_SYMBOL(r)                 ((r)->r_index)
#define RELOC_MEMORY_ADD_P(r)           ((r)->r_type == RELOC_0_63)
#define RELOC_ADD_EXTRA(r)              ((r)->r_addend)
#define RELOC_PCREL_P(r)             \
        ((r)->r_type == RELOC_WDISP14 || (r)->r_type == RELOC_WDISP21)
#define RELOC_BRREL_P(r)             \
        (((r)->r_type >= BRELOC_0_15) && ((r)->r_type <= BRELOC_16_31))
#define RELOC_BRSET_P(r)             \
        ((r)->r_type >= BRSET_0_15)
#define	RELOC_MEMORY_BROFF(r,x)	     \
	( (((r)->r_type == BRSET_0_15)||((r)->r_type == BRELOC_0_15)) \
	  ? *(short*)(x) \
	  : ( ((r)->r_type == BRELOC_0_31) \
	    ? (*(int *)(x)) : ((*(short *)(x)) << 16) ) )
#define RELOC_VALUE_RIGHTSHIFT(r)       (reloc_target_rightshift[(r)->r_type])
#define RELOC_TARGET_SIZE(r)            (reloc_target_size[(r)->r_type])
#define RELOC_TARGET_BITPOS(r)          0
#define RELOC_TARGET_BITSIZE(r)         (reloc_target_bitsize[(r)->r_type])

/* Note that these are very dependent on the order of the enums in
   enum reloc_type (in a.out.h); if they change the following must be
   changed.  This is what is assumed here:
	    RELOC_0_7		0
	    RELOC_0_15		1
	    RELOC_0_31 		2
	    RELOC_0_63		3
	    RELOC_0_25		4
	    RELOC_16_31		5
	    RELOC_WDISP21	6
	    RELOC_BASE21	7
	    RELOC_WDISP14	8
	    RELOC_BASE14	9
	    RELOC_32_47		10
	    RELOC_48_63		11
	    RELOC_U_16_31	12
	    RELOC_U_32_47	13
	    RELOC_U_48_63	14
	    RELOC_0_12		15
	    RELOC_0_8		16
	    NO_RELOC		17
	    BRELOC_0_15		18
	    BRELOC_0_31		19
	    BRELOC_16_31	20
	    BRSET_0_15		21
	    BRSET_16_31		22
 */

static int reloc_target_rightshift[] = {
  0, 0, 0, 0, 0, 16, 2, 0, 2, 0, 32, 48, 16, 32, 48, 0, 0, 0, 0, 0, 16, 0, 16
};
static int reloc_target_size[] = {
  0, 1, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 1, 2, 1, 1, 1
};
static int reloc_target_bitsize[] = {
  8, 16, 32, 64, 26, 16, 21, 21, 14, 14, 16, 16, 16, 16, 16, 13, 9, 0, 16, 32, 16, 16, 16
};

#define	MAX_ALIGNMENT	(sizeof (double))

#ifdef nounderscore
#define LPREFIX '.'
#else
#define LPREFIX 'L'
#endif


/* Special global symbol types understood by GNU LD.  */

/* The following type indicates the definition of a symbol as being
   an indirect reference to another symbol.  The other symbol
   appears as an undefined reference, immediately following this symbol.

   Indirection is asymmetrical.  The other symbol's value will be used
   to satisfy requests for the indirect symbol, but not vice versa.
   If the other symbol does not have a definition, libraries will
   be searched to find a definition.

   So, for example, the following two lines placed in an assembler
   input file would result in an object file which would direct gnu ld
   to resolve all references to symbol "foo" as references to symbol
   "bar".

	.stabs "_foo",11,0,0,0
	.stabs "_bar",1,0,0,0

   Note that (11 == (N_INDR | N_EXT)) and (1 == (N_UNDF | N_EXT)).  */

#ifndef N_INDR
#define N_INDR 0xa
#endif

/* The following symbols refer to set elements.  These are expected
   only in input to the loader; they should not appear in loader
   output (unless relocatable output is requested).  To be recognized
   by the loader, the input symbols must have their N_EXT bit set.
   All the N_SET[ATDB] symbols with the same name form one set.  The
   loader collects all of these elements at load time and outputs a
   vector for each name.
   Space (an array of 32 bit words) is allocated for the set in the
   data section, and the n_value field of each set element value is
   stored into one word of the array.
   The first word of the array is the length of the set (number of
   elements).  The last word of the vector is set to zero for possible
   use by incremental loaders.  The array is ordered by the linkage
   order; the first symbols which the linker encounters will be first
   in the array.

   In C syntax this looks like:

	struct set_vector {
	  unsigned int length;
	  unsigned int vector[length];
	  unsigned int always_zero;
	};

   Before being placed into the array, each element is relocated
   according to its type.  This allows the loader to create an array
   of pointers to objects automatically.  N_SETA type symbols will not
   be relocated.

   The address of the set is made into an N_SETV symbol
   whose name is the same as the name of the set.
   This symbol acts like a N_DATA global symbol
   in that it can satisfy undefined external references.

   For the purposes of determining whether or not to load in a library
   file, set element definitions are not considered "real
   definitions"; they will not cause the loading of a library
   member.

   If relocatable output is requested, none of this processing is
   done.  The symbols are simply relocated and passed through to the
   output file.

   So, for example, the following three lines of assembler code
   (whether in one file or scattered between several different ones)
   will produce a three element vector (total length is five words;
   see above), referenced by the symbol "_xyzzy", which will have the
   addresses of the routines _init1, _init2, and _init3.

   *NOTE*: If symbolic addresses are used in the n_value field of the
   defining .stabs, those symbols must be defined in the same file as
   that containing the .stabs.

	.stabs "_xyzzy",23,0,0,_init1
	.stabs "_xyzzy",23,0,0,_init2
	.stabs "_xyzzy",23,0,0,_init3

   Note that (23 == (N_SETT | N_EXT)).  */

#ifndef N_SETA
#define	N_SETA	0x14		/* Absolute set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETT
#define	N_SETT	0x16		/* Text set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETD
#define	N_SETD	0x18		/* Data set element symbol */
#endif				/* This is input to LD, in a .o file.  */

#ifndef N_SETB
#define	N_SETB	0x1A		/* Bss set element symbol */
#endif				/* This is input to LD, in a .o file.  */

/* Macros dealing with the set element symbols defined in a.out.h */
#define	SET_ELEMENT_P(x)	((x)>=N_SETA&&(x)<=(N_SETB|N_EXT))
#define TYPE_OF_SET_ELEMENT(x)	((x)-N_SETA+N_ABS)

#ifndef N_SETV
#define N_SETV	0x1C		/* Pointer to set vector in data area.  */
#endif				/* This is output from LD.  */

/* If a this type of symbol is encountered, its name is a warning
   message to print each time the symbol referenced by the next symbol
   table entry is referenced.

   This feature may be used to allow backwards compatibility with
   certain functions (eg. gets) but to discourage programmers from
   their use.

   So if, for example, you wanted to have ld print a warning whenever
   the function "gets" was used in their C program, you would add the
   following to the assembler file in which gets is defined:

	.stabs "Obsolete function \"gets\" referenced",30,0,0,0
	.stabs "_gets",1,0,0,0

   These .stabs do not necessarily have to be in the same file as the
   gets function, they simply must exist somewhere in the compilation.  */

#ifndef N_WARNING
#define N_WARNING 0x1E		/* Warning message to print if symbol
				   included */
#endif				/* This is input to ld */

#ifndef __GNU_STAB__

/* Line number for the data section.  This is to be used to describe
   the source location of a variable declaration.  */
#ifndef N_DSLINE
#define N_DSLINE (N_SLINE+N_DATA-N_TEXT)
#endif

/* Line number for the bss section.  This is to be used to describe
   the source location of a variable declaration.  */
#ifndef N_BSLINE
#define N_BSLINE (N_SLINE+N_BSS-N_TEXT)
#endif

#endif /* not __GNU_STAB__ */

/* Symbol table */

/* Global symbol data is recorded in these structures,
   one for each global symbol.
   They are found via hashing in 'symtab', which points to a vector of buckets.
   Each bucket is a chain of these structures through the link field.  */

typedef
  struct glosym
    {
      /* Pointer to next symbol in this symbol's hash bucket.  */
      struct glosym *link;
      /* Name of this symbol.  */
      char *name;
      /* Value of this symbol as a global symbol.  */
      long value;
      /* Chain of external 'nlist's in files for this symbol, both defs
	 and refs.  */
      struct nlist *refs;
      /* Any warning message that might be associated with this symbol
         from an N_WARNING symbol encountered. */
      char *warning;
      /* Nonzero means definitions of this symbol as common have been seen,
	 and the value here is the largest size specified by any of them.  */
      int max_common_size;
      /* For relocatable_output, records the index of this global sym in the
	 symbol table to be written, with the first global sym given index 0.*/
      int def_count;
      /* Nonzero means a definition of this global symbol is known to exist.
	 Library members should not be loaded on its account.  */
      char defined;
      /* Nonzero means a reference to this global symbol has been seen
	 in a file that is surely being loaded.
	 A value higher than 1 is the n_type code for the symbol's
	 definition.  */
      char referenced;
      /* A count of the number of undefined references printed for a
	 specific symbol.  If a symbol is unresolved at the end of
	 digest_symbols (and the loading run is supposed to produce
	 relocatable output) do_file_warnings keeps track of how many
	 unresolved reference error messages have been printed for
	 each symbol here.  When the number hits MAX_UREFS_PRINTED,
	 messages stop. */
      unsigned char undef_refs;
      /* 1 means that this symbol has multiple definitions.  2 means
         that it has multiple definitions, and some of them are set
	 elements, one of which has been printed out already.  */
      unsigned char multiply_defined;
      /* Nonzero means print a message at all refs or defs of this symbol */
      char trace;
    }
  symbol;

/* Demangler for C++. */
extern char *cplus_demangle ();

/* Demangler function to use. */
char *(*demangler)() = NULL;

/* Number of buckets in symbol hash table */
#define	TABSIZE	1009

/* The symbol hash table: a vector of TABSIZE pointers to struct glosym. */
symbol *symtab[TABSIZE];

/* Number of symbols in symbol hash table. */
int num_hash_tab_syms = 0;

/* Count the number of nlist entries that are for local symbols.
   This count and the three following counts
   are incremented as as symbols are entered in the symbol table.  */
int local_sym_count;

/* Count number of nlist entries that are for local symbols
   whose names don't start with L. */
int non_L_local_sym_count;

/* Count the number of nlist entries for debugger info.  */
int debugger_sym_count;

/* Count the number of global symbols referenced and not defined.  */
int undefined_global_sym_count;

/* Count the number of global symbols multiply defined.  */
int multiple_def_count;

/* Count the number of defined global symbols.
   Each symbol is counted only once
   regardless of how many different nlist entries refer to it,
   since the output file will need only one nlist entry for it.
   This count is computed by `digest_symbols';
   it is undefined while symbols are being loaded. */
int defined_global_sym_count;

/* Count the number of symbols defined through common declarations.
   This count is kept in ranlib_library, linear_library, and
   enter_global_ref.  It is incremented when the defined flag is set
   in a symbol because of a common definition, and decremented when
   the symbol is defined "for real" (ie. by something besides a common
   definition).  */
int common_defined_global_count;

/* Count the number of set element type symbols and the number of
   separate vectors which these symbols will fit into.  See the
   GNU a.out.h for more info.
   This count is computed by 'enter_file_symbols' */
int set_symbol_count;
int set_vector_count;

/* Define a linked list of strings which define symbols which should
   be treated as set elements even though they aren't.  Any symbol
   with a prefix matching one of these should be treated as a set
   element.

   This is to make up for deficiencies in many assemblers which aren't
   willing to pass any stabs through to the loader which they don't
   understand.  */
struct string_list_element {
  char *str;
  struct string_list_element *next;
};

struct string_list_element *set_element_prefixes;

/* Count the number of definitions done indirectly (ie. done relative
   to the value of some other symbol. */
int global_indirect_count;

/* Count the number of warning symbols encountered. */
int warning_count;

/* Total number of symbols to be written in the output file.
   Computed by digest_symbols from the variables above.  */
int nsyms;


/* Nonzero means ptr to symbol entry for symbol to use as start addr.
   -e sets this.  */
symbol *entry_symbol;

symbol *edata_symbol;   /* the symbol _edata */
symbol *etext_symbol;   /* the symbol _etext */
symbol *end_symbol;	/* the symbol _end */


/* Each input file, and each library member ("subfile") being loaded,
   has a `file_entry' structure for it.

   For files specified by command args, these are contained in the vector
   which `file_table' points to.

   For library members, they are dynamically allocated,
   and chained through the `chain' field.
   The chain is found in the `subfiles' field of the `file_entry'.
   The `file_entry' objects for the members have `superfile' fields pointing
   to the one for the library.  */

struct file_entry {

  /* Name of this file.  */
  char *filename;
  /* Name to use for the symbol giving address of text start */
  /* Usually the same as filename, but for a file spec'd with -l
     this is the -l switch itself rather than the filename.  */
  char *local_sym_name;

  /* Describe the layout of the contents of the file */

  /* The relevant information from the file's a.out or coff header.
     The coff file read routine maps things onto the exec format */
  struct exec header;
  char coff_nscns;
  enum { NOTDEF, LIBR, AOUT, COFF } object_format;

  /* Offset in file of GDB symbol segment, or 0 if there is none.  */
  int symseg_offset;

  /* Describe data from the file loaded into core */

  /* Symbol table of the file.  */
  struct nlist *symbols;
  int n_symbols;

  /* Size in bytes of string table.  */
  int string_size;
  /* Pointer to the string table.
     The string table is not kept in core all the time,
     but when it is in core, its address is here.  */
  char *strings;

  /* Next two used only if `relocatable_output' or if needed for */
  /* output of undefined reference line numbers. */

  /* Text reloc info saved by `write_text' for `coptxtrel'.  */
  struct relocation_info *textrel;
  /* Data reloc info saved by `write_data' for `copdatrel'.  */
  struct relocation_info *datarel;

  /* Value of base register computed for this file */
  long base_register_value;

  /* Relation of this file's segments to the output file */

  /* Start of this file's text seg in the output file core image.  */
  long text_start_address;
  /* Start of this file's data seg in the output file core image.  */
  long data_start_address;
  /* Start of this file's bss seg in the output file core image.  */
  long bss_start_address;
  /* Offset in bytes in the output file symbol table
     of the first local symbol for this file.  Set by `write_file_symbols'.  */
  int local_syms_offset;

  /* For library members only */

  /* For a library, points to chain of entries for the library members.  */
  struct file_entry *subfiles;
  /* For a library member, offset of the member within the archive.
     Zero for files that are not library members.  */
  int starting_offset;
  /* Size of contents of this file, if library member.  */
  int total_size;
  /* For library member, points to the library's own entry.  */
  struct file_entry *superfile;
  /* For library member, points to next entry for next member.  */
  struct file_entry *chain;

  /* 1 means search a set of directories for this file.  */
  char search_dirs_flag;

  /* 1 means this is base file of incremental load.
     Do not load this file's text or data.
     Also default text_start to after this file's bss. */
  char just_syms_flag;
};

/* Vector of entries for input files specified by arguments.
   These are all the input files except for members of specified libraries.  */
struct file_entry *file_table;

/* Length of that vector.  */
int number_of_files;

/* When loading the text and data, we can avoid doing a close
   and another open between members of the same library.

   These two variables remember the file that is currently open.
   Both are zero if no file is open.

   See `each_file' and `file_close'.  */

struct file_entry *input_file;
int input_desc;

/* The name of the file to write; "a.out" by default.  */

char *output_filename;

/* Descriptor for writing that file with `mywrite'.  */

int outdesc;

/* Header for that file (filled in by `write_header').  */

struct exec outheader;

/* The following are computed by `digest_symbols'.  */

long text_size;		/* total size of text of all input files.  */
long data_size;		/* total size of data of all input files.  */
long bss_size;		/* total size of bss of all input files.  */
long text_reloc_size;	/* total size of text relocation of all input files.  */
long data_reloc_size;	/* total size of data relocation of all input */
			/* files.  */

/* Keep here the value of the base register for the current file,
   while writing output and relocating.  It basically sums up the
   data sections of all files, one at a time, so that each single
   file gets a full N bits of locally addressable data */
long cur_base_register;

/* Specifications of start and length of the area reserved at the end
   of the text segment for the set vectors.  Computed in 'digest_symbols' */
long set_sect_start;
int set_sect_size;

/* Pointer for in core storage for the above vectors, before they are
   written. */
unsigned long *set_vectors;

/* Amount of cleared space to leave between the text and data segments.  */

int text_pad;

/* Amount of bss segment to include as part of the data segment.  */

int data_pad;

/* Format of RANLIB_MEMBERNAME:
   First, a longword containing the size of the 'ranlib' data that follows.
   Second, zero or more 'ranlib' structures.
   Third, a word containing the length of symbol name strings.
   Fourth, zero or more symbol name strings (each followed by a null).
   See ranlib.h */

#include <ranlib.h>

/* Format of a MIPS_RANLIB_MEMBER:
   Same as above, but all in 32bit ints instead.  Beats me why. */

typedef int mipsranlib_nelm_t;
struct	mipsranlib {
	union {
		int	ran_strx;	/* string table index of.. */
/*nope		char	*ran_name;	/* ..symbol defined by.. */
	} ran_un;
	int	ran_off;		/* ..library member at this offset */
};

typedef int mipsranlib_ssiz_t;

#define	MIPS_RANLIB_MEMBERNAME	"________64E"
#define	MIPS_RANLIB_CHECKN	11



/* Record most of the command options.  */

/* Address we assume the text section will be loaded at.
   We relocate symbols and text and data for this, but we do not
   write any padding in the output file for it.  */
long text_start;

/* Offset of default entry-pc within the text section.  */
long entry_offset;

/* Address we decide the data section will be loaded at.  */
long data_start;

/* `text-start' address is normally this much plus a page boundary.
   This is not a user option; it is fixed for each system.  */
long text_start_alignment;

/* Nonzero if -T was specified in the command line.
   This prevents text_start from being set later to default values.  */
int T_flag_specified;

/* Same, but expect the a.out header to be eventually removed from
   the image (special kernel hack) */
int Tstrip_flag_specified;

/* Nonzero if -Tdata was specified in the command line.
   This prevents data_start from being set later to default values.  */
int Tdata_flag_specified;

/* Size to pad data section up to.
   We simply increase the size of the data section, padding with zeros,
   and reduce the size of the bss section to match.  */
int specified_data_size;

/* Magic number to use for the output file, set by switch.  */
int magic;

/* If we want a coff hat to our image */
int pseudo_coff_image = __osf__;
struct coff_hat {
  struct filehdr f;
  struct aouthdr a;
  struct scnhdr s0;
  struct scnhdr s1;
};

/* Nonzero means print names of input files as processed.  */
int trace_files;

/* Debugging */
int logorrea = 0 /* 3 */;

/* Which symbols should be stripped (omitted from the output):
   none, all, or debugger symbols.  */
enum { STRIP_NONE, STRIP_ALL, STRIP_DEBUGGER } strip_symbols;

/* Which local symbols should be omitted:
   none, all, or those starting with L.
   This is irrelevant if STRIP_NONE.  */
enum { DISCARD_NONE, DISCARD_ALL, DISCARD_L } discard_locals;

/* 1 => write load map.  */
int write_map;

/* 1 => write relocation into output file so can re-input it later.  */
int relocatable_output;

/* 1 => assign space to common symbols even if `relocatable_output'.  */
int force_common_definition;

/* Standard directories to search for files specified by -l.  */
char *standard_search_dirs[] =
#ifdef STANDARD_SEARCH_DIRS
  {STANDARD_SEARCH_DIRS};
#else
  {"/usr/lib", "/usr/local/lib"};
#endif

/* Actual vector of directories to search;
   this contains those specified with -L plus the standard ones.  */
char **search_dirs;

/* Length of the vector `search_dirs'.  */
int n_search_dirs;

/* Non zero means to create the output executable. */
/* Cleared by nonfatal errors.  */
int make_executable;

/* Force the executable to be output, even if there are non-fatal
   errors */
int force_executable;

/* Keep a list of any symbols referenced from the command line (so
   that error messages for these guys can be generated). This list is
   zero terminated. */
struct glosym **cmdline_references;
int cl_refs_allocated;

void bcopy (), bzero ();
char *alloc (), *realloc ();
#ifndef alloca
char *alloca ();
#endif
int free ();

char *malloc(), *realloc();
char *xmalloc (), *xrealloc ();
void fatal ();
void fatal_with_file ();
void perror_name ();
void perror_file ();
void error ();

void digest_symbols ();
void print_symbols ();
void load_symbols ();
void decode_command ();
void list_undefined_symbols ();
void list_unresolved_references ();
void write_output ();
void write_header ();
void write_text ();
void read_file_relocation ();
void write_data ();
void write_rel ();
void write_syms ();
void write_symsegs ();
void mywrite ();
void symtab_init ();
void padfile ();
char *concat ();
char *get_file_name ();
symbol *getsym (), *getsym_soft (), *reloc_to_sym();

int
main (argc, argv)
     char **argv;
     int argc;
{
/*   Added this to stop ld core-dumping on very large .o files.    */
#ifdef RLIMIT_STACK
  /* Get rid of any avoidable limit on stack size.  */
  {
    struct rlimit rlim;

    /* Set the stack limit huge so that alloca does not fail. */
    getrlimit (RLIMIT_STACK, &rlim);
    rlim.rlim_cur = rlim.rlim_max;
    setrlimit (RLIMIT_STACK, &rlim);
  }
#endif /* RLIMIT_STACK */

#ifdef	__LDPGSZ
  page_size = __LDPGSZ;	/* cuz if cross-compiling */
#else
  page_size = getpagesize ();
#endif
  progname = argv[0];

  /* Clear the cumulative info on the output file.  */

  text_size = 0;
  data_size = 0;
  bss_size = 0;
  text_reloc_size = 0;
  data_reloc_size = 0;

  data_pad = 0;
  text_pad = 0;

  /* Initialize the data about options.  */

  specified_data_size = 0;
  strip_symbols = STRIP_NONE;
  trace_files = 0;
  discard_locals = DISCARD_NONE;
  entry_symbol = 0;
  write_map = 0;
  relocatable_output = 0;
  force_common_definition = 0;
#if (defined(__alpha) && defined(__osf__))
  T_flag_specified = 1;
  text_start = 0x100000000;
  outheader.a_tstart = text_start;
#else
  T_flag_specified = 0;
#endif
  Tstrip_flag_specified = 0;
  Tdata_flag_specified = 0;
  magic = DEFAULT_MAGIC;
  make_executable = 1;
  force_executable = 0;
  set_element_prefixes = 0;

  /* Initialize the cumulative counts of symbols.  */

  local_sym_count = 0;
  non_L_local_sym_count = 0;
  debugger_sym_count = 0;
  undefined_global_sym_count = 0;
  set_symbol_count = 0;
  set_vector_count = 0;
  global_indirect_count = 0;
  warning_count = 0;
  multiple_def_count = 0;
  common_defined_global_count = 0;

  /* Keep a list of symbols referenced from the command line */
  cl_refs_allocated = 10;
  cmdline_references
    = (struct glosym **) xmalloc (cl_refs_allocated
				  * sizeof(struct glosym *));
  *cmdline_references = 0;

  /* Completely decode ARGV.  */

  decode_command (argc, argv);

  /* Create the symbols `etext', `edata' and `end'.  */

  if (!relocatable_output)
    symtab_init ();

  /* Determine whether to count the header as part of
     the text size, and initialize the text size accordingly.
     This depends on the kind of system and on the output format selected.  */

  BSD_N_SET_MAGIC (&outheader, magic);

  if (Tstrip_flag_specified)
    {
      text_size = 0;
      entry_offset = 0;
    }
  else
    {
      if (pseudo_coff_image) {
	text_size = sizeof (struct coff_hat);
      } else {
	text_size = sizeof (struct exec);
	text_size -= BSD_N_TXTOFF (&outheader);
      }

      if (text_size < 0)
        text_size = 0;
      entry_offset = text_size;
    }

  if (!T_flag_specified && !relocatable_output)
    text_start = BSD_TEXT_START (&outheader);	/* alpha: 0 */

  /* The text-start address is normally this far past a page boundary.  */
  text_start_alignment = text_start % page_size;

  /* Load symbols of all input files.
     Also search all libraries and decide which library members to load.  */

  load_symbols ();

  /* Compute where each file's sections go, and relocate symbols.  */

  digest_symbols ();

  /* Print error messages for any missing symbols, for any warning
     symbols, and possibly multiple definitions */

  do_warnings (stderr);

  /* Print a map, if requested.  */

  if (write_map) print_symbols (stdout);

  /* Write the output file.  */

  if (make_executable || force_executable)
    write_output ();

  exit (!make_executable);
}

void decode_option ();

/* Analyze a command line argument.
   Return 0 if the argument is a filename.
   Return 1 if the argument is a option complete in itself.
   Return 2 if the argument is a option which uses an argument.

   Thus, the value is the number of consecutive arguments
   that are part of options.  */

int
classify_arg (arg)
     register char *arg;
{
  if (*arg != '-') return 0;
  switch (arg[1])
    {
    case 'A':
    case 'g':
    case 'D':
    case 'e':
    case 'L':
    case 'l':
    case 'o':
    case 'u':
    case 'V':
    case 'y':
      if (arg[2])
	return 1;
      return 2;

    case 'B':
      if (! strcmp (&arg[2], "static"))
	return 1;

    case 'T':
      if (arg[2] == 0)
	return 2;
      if (! strcmp (&arg[2], "text"))
	return 2;
      if (! strcmp (&arg[2], "strip"))
	return 2;
      if (! strcmp (&arg[2], "data"))
	return 2;
      return 1;
    }

  return 1;
}

/* Process the command arguments,
   setting up file_table with an entry for each input file,
   and setting variables according to the options.  */

void
decode_command (argc, argv)
     char **argv;
     int argc;
{
  register int i;
  register struct file_entry *p;
  char *cp;

  number_of_files = 0;
  output_filename = "a.out";

  n_search_dirs = 0;
  search_dirs = (char **) xmalloc (sizeof (char *));

  /* First compute number_of_files so we know how long to make file_table.  */
  /* Also process most options completely.  */

  for (i = 1; i < argc; i++)
    {
      register int code = classify_arg (argv[i]);
      if (code)
	{
	  if (i + code > argc)
	    fatal ("no argument following %s\n", argv[i]);

	  decode_option (argv[i], argv[i+1]);

	  if (argv[i][1] == 'l' || argv[i][1] == 'A')
	    number_of_files++;

	  i += code - 1;
	}
      else
	number_of_files++;
    }

  if (!number_of_files)
    fatal ("no input files", 0);

  p = file_table
    = (struct file_entry *) xmalloc (number_of_files * sizeof (struct file_entry));
  bzero (p, number_of_files * sizeof (struct file_entry));

  /* Now scan again and fill in file_table.  */
  /* All options except -A and -l are ignored here.  */

  for (i = 1; i < argc; i++)
    {
      register int code = classify_arg (argv[i]);

      if (code)
	{
	  char *string;
	  if (code == 2)
	    string = argv[i+1];
	  else
	    string = &argv[i][2];

	  if (argv[i][1] == 'A')
	    {
	      if (p != file_table)
		fatal ("-A specified before an input file other than the first");

	      p->filename = string;
	      p->local_sym_name = string;
	      p->just_syms_flag = 1;
	      p++;
	    }
	  if (argv[i][1] == 'l')
	    {
	      if (cp = rindex(string, '/'))
		{
		  *cp++ = '\0';
		  cp = concat (string, "/lib", cp);
		  p->filename = concat (cp, ".a", "");
	        }
	      else
	        p->filename = concat ("lib", string, ".a");

	      p->local_sym_name = concat ("-l", string, "");
	      p->search_dirs_flag = 1;
	      p++;
	    }
	  i += code - 1;
	}
      else
	{
	  p->filename = argv[i];
	  p->local_sym_name = argv[i];
	  p++;
	}
    }

  /* Now check some option settings for consistency.  */

  if ((magic == ZMAGIC || magic == NMAGIC)
      && (text_start - text_start_alignment) & (page_size - 1))
    fatal ("-T argument not multiple of page size, with sharable output", 0);

  /* Append the standard search directories to the user-specified ones.  */
  {
    int n = sizeof standard_search_dirs / sizeof standard_search_dirs[0];
    n_search_dirs += n;
    search_dirs
      = (char **) xrealloc (search_dirs, n_search_dirs * sizeof (char *));
    bcopy (standard_search_dirs, &search_dirs[n_search_dirs - n],
	   n * sizeof (char *));
  }
}


void
add_cmdline_ref (sp)
     struct glosym *sp;
{
  struct glosym **ptr;

  for (ptr = cmdline_references;
       ptr < cmdline_references + cl_refs_allocated && *ptr;
       ptr++)
    ;

  if (ptr >= cmdline_references + cl_refs_allocated - 1)
    {
      int diff = ptr - cmdline_references;

      cl_refs_allocated *= 2;
      cmdline_references = (struct glosym **)
	xrealloc (cmdline_references,
		 cl_refs_allocated * sizeof (struct glosym *));
      ptr = cmdline_references + diff;
    }

  *ptr++ = sp;
  *ptr = (struct glosym *) 0;
}

int
set_element_prefixed_p (name)
     char *name;
{
  struct string_list_element *p;
  int i;

  for (p = set_element_prefixes; p; p = p->next)
    {
      for (i = 0; p->str[i] != '\0' && (p->str[i] == name[i]); i++)
	;

      if (p->str[i] == '\0')
	return 1;
    }
  return 0;
}

long parse ();

/* Record an option and arrange to act on it later.
   ARG should be the following command argument,
   which may or may not be used by this option.

   The `l' and `A' options are ignored here since they actually
   specify input files.  */

void
decode_option (swt, arg)
     register char *swt, *arg;
{
  /* We get Bstatic from gcc on suns.  */
  if (! strcmp (swt + 1, "Bstatic"))
    return;
  if (! strcmp (swt + 1, "Ttext"))
    {
      text_start = parse (arg, "%lx", "invalid argument to -Ttext");
      outheader.a_tstart = text_start;
      T_flag_specified = 1;
      return;
    }
  /* Same as above, but a.out header will be stripped, eventually */
  if (! strcmp (swt + 1, "Tstrip"))
    {
      Tstrip_flag_specified = 1;
      text_start = parse (arg, "%lx", "invalid argument to -Tstrip");
      outheader.a_tstart = text_start;
      T_flag_specified = 1;
      return;
    }
  if (! strcmp (swt + 1, "Tdata"))
    {
      data_start = parse (arg, "%lx", "invalid argument to -Tdata");
      outheader.a_dstart = data_start;
      Tdata_flag_specified = 1;
      return;
    }
  if (! strcmp (swt + 1, "Thdr"))
    {
      pseudo_coff_image = !pseudo_coff_image;
      return;
    }
  if (! strcmp (swt + 1, "noinhibit-exec"))
    {
      force_executable = 1;
      return;
    }

  if (swt[2] != 0)
    arg = &swt[2];

  switch (swt[1])
    {
    case 'A':
    case 'g':
      return;

    case 'D':
      specified_data_size = parse (arg, "%lx", "invalid argument to -D");
      return;

    case 'd':
      force_common_definition = 1;
      return;

    case 'e':
      entry_symbol = getsym (arg);
      if (!entry_symbol->defined && !entry_symbol->referenced)
	undefined_global_sym_count++;
      entry_symbol->referenced = 1;
      add_cmdline_ref (entry_symbol);
      return;

    case 'l':
      /* If linking with libg++, use the C++ demangler. */
      if (arg != NULL && strcmp (arg, "g++") == 0)
	demangler = cplus_demangle;
      return;

    case 'L':
      n_search_dirs++;
      search_dirs
	= (char **) xrealloc (search_dirs, n_search_dirs * sizeof (char *));
      search_dirs[n_search_dirs - 1] = arg;
      return;

    case 'M':
      write_map = 1;
      return;

    case 'N':
      magic = OMAGIC;
      return;

    case 'n':
      magic = NMAGIC;
      return;

    case 'o':
      output_filename = arg;
      return;

    case 'r':
      relocatable_output = 1;
      magic = OMAGIC;
      text_start = 0;
      return;

    case 'S':
      strip_symbols = STRIP_DEBUGGER;
      return;

    case 's':
      strip_symbols = STRIP_ALL;
      return;

    case 'T':
      text_start = parse (arg, "%lx", "invalid argument to -T");
      outheader.a_tstart = text_start;
      T_flag_specified = 1;
      return;

    case 't':
      trace_files = 1;
      return;

    case 'u':
      {
	register symbol *sp = getsym (arg);
	if (!sp->defined && !sp->referenced)
	  undefined_global_sym_count++;
	sp->referenced = 1;
	add_cmdline_ref (sp);
      }
      return;

    case 'V':
      {
	struct string_list_element *new
	  = (struct string_list_element *)
	    xmalloc (sizeof (struct string_list_element));

	new->str = arg;
	new->next = set_element_prefixes;
	set_element_prefixes = new;
	return;
      }

    case 'v':
      logorrea = 0;
      return;

    case 'X':
      discard_locals = DISCARD_L;
      return;

    case 'x':
      discard_locals = DISCARD_ALL;
      return;

    case 'y':
      {
	register symbol *sp = getsym (&swt[2]);
	sp->trace = 1;
      }
      return;

    case 'z':
      magic = ZMAGIC;
      return;

    default:
      fatal ("invalid command option `%s'", swt);
    }
}

/** Convenient functions for operating on one or all files being */
 /** loaded.  */
void print_file_name ();

/* Call FUNCTION on each input file entry.
   Do not call for entries for libraries;
   instead, call once for each library member that is being loaded.

   FUNCTION receives two arguments: the entry, and ARG.  */

void
each_file (function, arg)
     register void (*function)();
     register long arg;
{
  register int i;

  for (i = 0; i < number_of_files; i++)
    {
      register struct file_entry *entry = &file_table[i];
      if (entry->object_format == LIBR)
        {
	  register struct file_entry *subentry = entry->subfiles;
	  for (; subentry; subentry = subentry->chain)
	    (*function) (subentry, arg);
	}
      else
	(*function) (entry, arg);
    }
}

/* Call FUNCTION on each input file entry until it returns a non-zero
   value.  Return this value.
   Do not call for entries for libraries;
   instead, call once for each library member that is being loaded.

   FUNCTION receives two arguments: the entry, and ARG.  It must be a
   function returning unsigned long (though this can probably be fudged). */

unsigned long
check_each_file (function, arg)
     register unsigned long (*function)();
     register long arg;
{
  register int i;
  register unsigned long return_val;

  for (i = 0; i < number_of_files; i++)
    {
      register struct file_entry *entry = &file_table[i];
      if (entry->object_format == LIBR)
        {
	  register struct file_entry *subentry = entry->subfiles;
	  for (; subentry; subentry = subentry->chain)
	    if (return_val = (*function) (subentry, arg))
	      return return_val;
	}
      else
	if (return_val = (*function) (entry, arg))
	  return return_val;
    }
  return 0;
}

/* Like `each_file' but ignore files that were just for symbol definitions.  */

void
each_full_file (function, arg)
     register void (*function)();
     register long arg;
{
  register int i;

  for (i = 0; i < number_of_files; i++)
    {
      register struct file_entry *entry = &file_table[i];
      if (entry->just_syms_flag)
	continue;
      if (entry->object_format == LIBR)
        {
	  register struct file_entry *subentry = entry->subfiles;
	  for (; subentry; subentry = subentry->chain)
	    (*function) (subentry, arg);
	}
      else
	(*function) (entry, arg);
    }
}

/* Close the input file that is now open.  */

void
file_close ()
{
  close (input_desc);
  input_desc = 0;
  input_file = 0;
}

/* Open the input file specified by 'entry', and return a descriptor.
   The open file is remembered; if the same file is opened twice in a row,
   a new open is not actually done.  */

int
file_open (entry)
     register struct file_entry *entry;
{
  register int desc;

  if (entry->superfile)
    return file_open (entry->superfile);

  if (entry == input_file)
    return input_desc;

  if (input_file) file_close ();

  if (entry->search_dirs_flag)
    {
      int i;

      for (i = 0; i < n_search_dirs; i++)
	{
	  register char *string
	    = concat (search_dirs[i], "/", entry->filename);
	  desc = open (string, O_RDONLY, 0);
	  if (desc > 0)
	    {
	      entry->filename = string;
	      entry->search_dirs_flag = 0;
	      break;
	    }
	  free (string);
	}
#ifdef	CMUCS
      if (entry->search_dirs_flag)
	{
	  char *getenv();
	  char buf[256], *lpath = getenv("LPATH");
	  if (lpath)
	    desc = openp(lpath, entry->filename, buf, O_RDONLY, 0);
	  if (desc > 0)
	    {
	       entry->filename = concat(buf,"","");
	       entry->search_dirs_flag = 0;
	    }
	}
#endif
    }
  else
    desc = open (entry->filename, O_RDONLY, 0);

  if (desc > 0)
    {
      input_file = entry;
      input_desc = desc;
      return desc;
    }

  perror_file (entry);
  /* NOTREACHED */
}

/* Print the filename of ENTRY on OUTFILE (a stdio stream),
   and then a newline.  */

void
prline_file_name (entry, outfile)
     struct file_entry *entry;
     FILE *outfile;
{
  print_file_name (entry, outfile);
  fprintf (outfile, "\n");
}

/* Print the filename of ENTRY on OUTFILE (a stdio stream).  */

void
print_file_name (entry, outfile)
     struct file_entry *entry;
     FILE *outfile;
{
  if (entry->superfile)
    {
      print_file_name (entry->superfile, outfile);
      fprintf (outfile, "(%s)", entry->filename);
    }
  else
    fprintf (outfile, "%s", entry->filename);
}

/* Return the filename of entry as a string (malloc'd for the purpose) */

char *
get_file_name (entry)
     struct file_entry *entry;
{
  char *result, *supfile;
  if (entry->superfile)
    {
      supfile = get_file_name (entry->superfile);
      result = xmalloc (strlen (supfile)
				 + strlen (entry->filename) + 3);
      sprintf (result, "%s(%s)", supfile, entry->filename);
      free (supfile);
    }
  else
    {
      result = xmalloc (strlen (entry->filename) + 1);
      strcpy (result, entry->filename);
    }
  return result;
}

/* Medium-level input routines for rel files.  */

/* Read a file's header into the proper place in the file_entry.
   DESC is the descriptor on which the file is open.
   ENTRY is the file's entry.  */

void
read_header (desc, entry)
     int desc;
     register struct file_entry *entry;
{
  register int len;
  object_header_t loc;

  lseek (desc, entry->starting_offset, 0);

  len = read (desc, &loc, sizeof (loc));
  if (len != sizeof (loc))
    fatal_with_file ("failure reading header of ", entry, 0);

  if (! BSD_N_BADMAG (&loc.aout)) {
    entry->object_format = AOUT;
    entry->header = loc.aout;
    entry->base_register_value = 0;	/* 	add to exec struct ? XXXXXX  */

  } else if (! COFF_N_BADMAG(&loc.coff)) {
    entry->object_format = COFF;
    /* The proper thing to do here would be to treat the file
       just like a library, each section like a subfile. But. */
    xlate_coff_header(desc, entry, &loc.coff);

  } else if (strncmp(loc.armag, ARMAG, SARMAG) == 0) {
    entry->object_format = LIBR;
    bzero(&entry->header, sizeof(entry->header));

  } else
    fatal_with_file ("bad magic number in ", entry, 0);

}

/* Read the symbols of file ENTRY into core.
   Assume it is already open, on descriptor DESC.
   Also read the length of the string table, which follows the symbol table,
   but don't read the contents of the string table. (except: coff) */

void
read_entry_symbols (desc, entry)
     struct file_entry *entry;
     int desc;
{
  int str_size;

  if (entry->object_format == NOTDEF)
    read_header (desc, entry);

  switch (entry->object_format) {
  case AOUT:
    entry->symbols = (struct nlist *) xmalloc (entry->header.a_syms);
    entry->n_symbols = entry->header.a_syms / sizeof(struct nlist);

    lseek (desc, BSD_N_SYMOFF (&entry->header) + entry->starting_offset, 0);
    if (entry->header.a_syms != read (desc, entry->symbols, entry->header.a_syms))
      fatal_with_file ("premature end of file in symbols of ", entry, 0);

    lseek (desc, BSD_N_STROFF (&entry->header) + entry->starting_offset, 0);
    if (sizeof str_size != read (desc, &str_size, sizeof str_size))
      fatal_with_file ("bad string table size in ", entry, 0);

    entry->string_size = str_size;
    break;
  case COFF:
    xlate_coff_syms(desc, entry);
    break;
  default:
    fatal_with_file("read_entry_symbols: Bad header type in file ", entry, 0);
  }
}

/* Read the string table of file ENTRY into core.
   Assume it is already open, on descriptor DESC. */

void
read_entry_strings (desc, entry)
     struct file_entry *entry;
     int desc;
{
  if (entry->object_format == NOTDEF)
    read_header (desc, entry);

  switch (entry->object_format) {
  case AOUT:
    lseek (desc, BSD_N_STROFF (&entry->header) + entry->starting_offset, 0);
    if (entry->string_size != read (desc, entry->strings, entry->string_size))
      fatal_with_file ("premature end of file in strings of ", entry, 0);
    break;
  case COFF:
    if (entry->strings == 0)
      fatal_with_file("fix read_entry_strings for COFF, file ", entry, 0);
    break;
  default:
    fatal_with_file("read_entry_strings: Bad header type in file ", entry, 0);
  }
}

/* Read in the symbols of all input files.  */

void read_file_symbols (), read_entry_symbols (), read_entry_strings ();
void enter_file_symbols (), enter_global_ref (), search_library ();

void
load_symbols ()
{
  register int i;

  if (trace_files) fprintf (stderr, "Loading symbols:\n\n");

  for (i = 0; i < number_of_files; i++)
    {
      register struct file_entry *entry = &file_table[i];
      read_file_symbols (entry);
    }

  if (trace_files) fprintf (stderr, "\n");
}

/* If ENTRY is a rel file, read its symbol and string sections into core.
   If it is a library, search it and load the appropriate members
   (which means calling this function recursively on those members).  */

void
read_file_symbols (entry)
     register struct file_entry *entry;
{
  register int desc;
  register int len;

  desc = file_open (entry);

  read_header(desc, entry);

  switch (entry->object_format) {
  case AOUT:
    {
      read_entry_symbols (desc, entry);
      entry->strings = (char *) alloca (entry->string_size);
      read_entry_strings (desc, entry);
      enter_file_symbols (entry);
      entry->strings = 0;
      break;
    }
  case LIBR:
    {
      search_library (desc, entry);
      break;
    }
  case COFF:
    {
      read_entry_symbols (desc, entry);
      /* that reads the strings once and forall */
      enter_file_symbols (entry);
      break;
    }
  default:
	fatal_with_file ("malformed input file (not rel or archive) ", entry, 0);
  }


  file_close ();
}

/* Enter the external symbol defs and refs of ENTRY in the hash table.  */

void
enter_file_symbols (entry)
     struct file_entry *entry;
{
  register struct nlist
    *p,
    *end = entry->symbols + entry->n_symbols;

  if (trace_files) prline_file_name (entry, stderr);

  for (p = entry->symbols; p < end; p++)
    {
      if (p->n_type == (N_SETV | N_EXT)) continue;
      if (set_element_prefixes
	  && set_element_prefixed_p (p->n_un.n_strx + entry->strings))
	p->n_type += (N_SETA - N_ABS);

      if (SET_ELEMENT_P (p->n_type))
	{
	  set_symbol_count++;
	  if (!relocatable_output)
	    enter_global_ref (p, p->n_un.n_strx + entry->strings, entry);
	}
      else if (p->n_type == N_WARNING)
	{
	  char *name = p->n_un.n_strx + entry->strings;

	  /* Grab the next entry.  */
	  p++;
	  if (p->n_type != (N_UNDF | N_EXT))
	    {
	      fprintf (stderr, "%s: Warning symbol found in %s without external reference following.\n",
		       progname, entry->filename);
	      make_executable = 0;
	      p--;		/* Process normally.  */
	    }
	  else
	    {
	      symbol *sp;
	      char *sname = p->n_un.n_strx + entry->strings;
	      /* Deal with the warning symbol.  */
	      enter_global_ref (p, p->n_un.n_strx + entry->strings, entry);
	      sp = getsym (sname);
	      sp->warning = xmalloc (strlen(name) + 1);
	      strcpy (sp->warning, name);
	      warning_count++;
	    }
	}
      else if (p->n_type & N_EXT)
	enter_global_ref (p, p->n_un.n_strx + entry->strings, entry);
      else if (p->n_un.n_strx && !(p->n_type & (N_STAB | N_EXT)))
	{
	  if ((p->n_un.n_strx + entry->strings)[0] != LPREFIX)
	    non_L_local_sym_count++;
	  local_sym_count++;
	}
      else debugger_sym_count++;
    }

   /* Count one for the local symbol that we generate,
      whose name is the file's name (usually) and whose address
      is the start of the file's text.  */

  local_sym_count++;
  non_L_local_sym_count++;
}

/* Enter one global symbol in the hash table.
   NLIST_P points to the `struct nlist' read from the file
   that describes the global symbol.  NAME is the symbol's name.
   ENTRY is the file entry for the file the symbol comes from.

   The `struct nlist' is modified by placing it on a chain of
   all such structs that refer to the same global symbol.
   This chain starts in the `refs' field of the symbol table entry
   and is chained through the `n_name'.  */

void
enter_global_ref (nlist_p, name, entry)
     register struct nlist *nlist_p;
     char *name;
     struct file_entry *entry;
{
  register symbol *sp = getsym (name);
  register int type = nlist_p->n_type;
  int oldref = sp->referenced;
  int olddef = sp->defined;

  nlist_p->n_un.n_name = (char *) sp->refs;
  sp->refs = nlist_p;

  sp->referenced = 1;
  if (type != (N_UNDF | N_EXT) || nlist_p->n_value)
    {
      if (!sp->defined || sp->defined == (N_UNDF | N_EXT))
	sp->defined = type;

      if (oldref && !olddef)
	/* It used to be undefined and we're defining it.  */
	undefined_global_sym_count--;

      if (!olddef && type == (N_UNDF | N_EXT) && nlist_p->n_value)
	{
	  /* First definition and it's common.  */
	  common_defined_global_count++;
	  sp->max_common_size = nlist_p->n_value;
	}
      else if (olddef && sp->max_common_size && type != (N_UNDF | N_EXT))
	{
	  /* It used to be common and we're defining it as
	     something else.  */
	  common_defined_global_count--;
	  sp->max_common_size = 0;
	}
      else if (olddef && sp->max_common_size && type == (N_UNDF | N_EXT)
	  && sp->max_common_size < nlist_p->n_value)
	/* It used to be common and this is a new common entry to
	   which we need to pay attention.  */
	sp->max_common_size = nlist_p->n_value;

      /* Are we defining it as a set element?  */
      if (SET_ELEMENT_P (type)
	  && (!olddef || (olddef && sp->max_common_size)))
	set_vector_count++;
      /* As an indirection?  */
      else if (type == (N_INDR | N_EXT))
	{
	  /* Indirect symbols value should be modified to point
	     a symbol being equivalenced to. */
	  nlist_p->n_value
	    = (unsigned int) getsym ((nlist_p + 1)->n_un.n_strx
				     + entry->strings);
	  if ((symbol *) nlist_p->n_value == sp)
	    {
	      /* Somebody redefined a symbol to be itself.  */
	      fprintf (stderr, "%s: Symbol %s indirected to itself.\n",
		       entry->filename, name);
	      /* Rewrite this symbol as being a global text symbol
		 with value 0.  */
	      nlist_p->n_type = sp->defined = N_TEXT | N_EXT;
	      nlist_p->n_value = 0;
	      /* Don't make the output executable.  */
	      make_executable = 0;
	    }
	  else
	    global_indirect_count++;
	}
    }
  else
    if (!oldref)
      undefined_global_sym_count++;

  if (sp == end_symbol && entry->just_syms_flag && !T_flag_specified)
    text_start = nlist_p->n_value;

  if (sp->trace)
    {
      register char *reftype;
      switch (type & N_TYPE)
	{
	case N_UNDF:
	  if (nlist_p->n_value)
	    reftype = "defined as common";
	  else reftype = "referenced";
	  break;

	case N_ABS:
	  reftype = "defined as absolute";
	  break;

	case N_TEXT:
	  reftype = "defined in text section";
	  break;

	case N_DATA:
	  reftype = "defined in data section";
	  break;

	case N_BSS:
	  reftype = "defined in BSS section";
	  break;

	case N_SETT:
	  reftype = "is a text set element";
	  break;

	case N_SETD:
	  reftype = "is a data set element";
	  break;

	case N_SETB:
	  reftype = "is a BSS set element";
	  break;

	case N_SETA:
	  reftype = "is an absolute set element";
	  break;

	case N_SETV:
	  reftype = "defined in data section as vector";
	  break;

	case N_INDR:
	  reftype = (char *) alloca (23
				     + strlen ((nlist_p + 1)->n_un.n_strx
					       + entry->strings));
	  sprintf (reftype, "defined equivalent to %s",
		   (nlist_p + 1)->n_un.n_strx + entry->strings);
	  break;

	}

      fprintf (stderr, "symbol %s %s in ", sp->name, reftype);
      print_file_name (entry, stderr);
      fprintf (stderr, "\n");
    }
}

#if	DEBUG

/* This return 0 if the given file entry's symbol table does *not*
   contain the nlist point entry, and it returns the files entry
   pointer (cast to unsigned long) if it does. */

unsigned long
contains_symbol (entry, n_ptr)
     struct file_entry *entry;
     register struct nlist *n_ptr;
{
  if (n_ptr >= entry->symbols &&
      n_ptr < (entry->symbols + entry->n_symbols))
    return (unsigned long) entry;
  return 0;
}
#endif

/* Searching libraries */

struct file_entry *decode_library_subfile ();
void linear_library (), ranlib_library (), mipsranlib_library ();

/* Search the library ENTRY, already open on descriptor DESC.
   This means deciding which library members to load,
   making a chain of `struct file_entry' for those members,
   and entering their global symbols in the hash table.  */

void
search_library (desc, entry)
     int desc;
     struct file_entry *entry;
{
  int member_length;
  register char *name;
  register struct file_entry *subentry;

  if (!undefined_global_sym_count) return;

  /* Examine its first member, which starts SARMAG bytes in.  */
  subentry = decode_library_subfile (desc, entry, SARMAG, &member_length);
  if (!subentry) return;

  name = subentry->filename;
  free (subentry);

  /* Search via RANLIB_MEMBERNAME if that exists, else linearly.  */

  if (!strcmp (name, RANLIB_MEMBERNAME))
    ranlib_library (desc, entry, member_length);
  else if (!strncmp(name, MIPS_RANLIB_MEMBERNAME, MIPS_RANLIB_CHECKN))
    mipsranlib_library (desc, entry, member_length);
  else
    linear_library (desc, entry);
}

/* Construct and return a file_entry for a library member.
   The library's file_entry is library_entry, and the library is open on DESC.
   SUBFILE_OFFSET is the byte index in the library of this member's header.
   We store the length of the member into *LENGTH_LOC.  */

struct file_entry *
decode_library_subfile (desc, library_entry, subfile_offset, length_loc)
     int desc;
     struct file_entry *library_entry;
     int subfile_offset;
     int *length_loc;
{
  int bytes_read;
  register int namelen;
  int member_length;
  register char *name;
  struct ar_hdr hdr1;
  register struct file_entry *subentry;

  lseek (desc, subfile_offset, 0);

  bytes_read = read (desc, &hdr1, sizeof hdr1);
  if (!bytes_read)
    return 0;		/* end of archive */

  if (sizeof hdr1 != bytes_read)
    fatal_with_file ("malformed library archive ", library_entry, 0);

  if (sscanf (hdr1.ar_size, "%d", &member_length) != 1)
    fatal_with_file ("malformatted header of archive member in ", library_entry, 0);

  subentry = (struct file_entry *) xmalloc (sizeof (struct file_entry));
  bzero (subentry, sizeof (struct file_entry));

  for (namelen = 0;
       namelen < sizeof hdr1.ar_name
       && hdr1.ar_name[namelen] != 0 && hdr1.ar_name[namelen] != ' '
       && hdr1.ar_name[namelen] != '/';
       namelen++);

  name = xmalloc (namelen+1);
  strncpy (name, hdr1.ar_name, namelen);
  name[namelen] = 0;

  subentry->filename = name;
  subentry->local_sym_name = name;
  subentry->symbols = 0;
  subentry->strings = 0;
  subentry->subfiles = 0;
  subentry->starting_offset = subfile_offset + sizeof hdr1;
  subentry->superfile = library_entry;
  subentry->object_format = NOTDEF;
  subentry->just_syms_flag = 0;
  subentry->chain = 0;
  subentry->total_size = member_length;

  (*length_loc) = member_length;

  return subentry;
}

int subfile_wanted_p ();

/* Search a library that has a RANLIB_MEMBERNAME member.
   DESC is a descriptor on which the library is open.
     The file pointer is assumed to point at the RANLIB_MEMBERNAME data.
   ENTRY is the library's file_entry.
   MEMBER_LENGTH is the length of the RANLIB_MEMBERNAME data.  */

void
ranlib_library (desc, entry, member_length)
     int desc;
     struct file_entry *entry;
     int member_length;
{
  ranlib_nelm_t *ranlib_data = (ranlib_nelm_t *) xmalloc (member_length);
  register struct ranlib *ranlib_base;
  char *sym_name_base;
  int number_of_ranlibs;
  ranlib_ssiz_t length_of_strings;
  int not_finished;
  int bytes_read;
  register int i;
  struct file_entry *prev = 0;
  off_t prev_offset = 0;

  bytes_read = read (desc, ranlib_data, member_length);
  if (bytes_read != member_length) {
itsbad:
    fatal_with_file ("malformatted member (%s) in ",
    			entry, RANLIB_MEMBERNAME);
  }

  number_of_ranlibs = *ranlib_data / sizeof (struct ranlib);
  if (number_of_ranlibs < 0 ||
       number_of_ranlibs * sizeof (struct ranlib) + sizeof(ranlib_nelm_t) + sizeof(ranlib_ssiz_t) > member_length)
    goto itsbad;

  ranlib_base = (struct ranlib *) (ranlib_data + 1);
  length_of_strings = *(ranlib_ssiz_t *) (ranlib_base + number_of_ranlibs);

  if (length_of_strings < 0
      || number_of_ranlibs * sizeof (struct ranlib) + length_of_strings
	 + sizeof(ranlib_nelm_t) + sizeof(ranlib_ssiz_t) > member_length)
    goto itsbad;

  sym_name_base = sizeof (length_of_strings) + (char *) (ranlib_base + number_of_ranlibs);

  /* Check all the string indexes for validity.  */

  for (i = 0; i < number_of_ranlibs; i++)
    {
      register off_t index = ranlib_base[i].ran_un.ran_strx;
      if (index < 0 || index >= length_of_strings
	  || (index && *(sym_name_base + index - 1)))
        goto itsbad;
    }

  /* Search the ranlib data for members to load.
     Do this until one whole pass finds nothing to load.  */

  not_finished = 1;
  while (not_finished)
    {
      not_finished = 0;

      /* Scan all the symbols mentioned in the ranlib for ones that we need.
	 Load the library members that contain such symbols.  */

      for (i = 0;
	   (i < number_of_ranlibs
	    && (undefined_global_sym_count || common_defined_global_count));
	   i++)
	if (ranlib_base[i].ran_un.ran_strx >= 0)
	  {
	    register symbol *sp;

	    sp = getsym_soft (sym_name_base
			      + ranlib_base[i].ran_un.ran_strx);

	    /* If we find a symbol that appears to be needed, think carefully
	       about the archive member that the symbol is in.  */

	    if (sp && ((sp->referenced && !sp->defined)
		       || (sp->defined && sp->max_common_size)))
	      {
		int junk;
		register int j;
		register off_t offset = ranlib_base[i].ran_off;
		struct file_entry *subentry;

		/* Don't think carefully about any archive member
		   more than once in a given pass.  */

		if (prev_offset == offset)
		  continue;
		prev_offset = offset;

		/* Read the symbol table of the archive member.  */

		subentry = decode_library_subfile (desc, entry, offset, &junk);
		if (subentry == 0)
		  fatal_with_file ("invalid offset for %s in symbol table of",
			 entry,
			 sym_name_base + ranlib_base[i].ran_un.ran_strx);
		read_entry_symbols (desc, subentry);
		if (subentry->strings == 0) {
		    subentry->strings = malloc (subentry->string_size);
		    read_entry_strings (desc, subentry);
		}

		/* Now scan the symbol table and decide whether to load.  */

		if (!subfile_wanted_p (subentry))
		  {
		    free (subentry->symbols);
		    free (subentry);
		  }
		else
		  {
		    /* This member is needed; load it.
		       Since we are loading something on this pass,
		       we must make another pass through the ranlib data.  */

		    not_finished = 1;

		    enter_file_symbols (subentry);

		    if (prev)
		      prev->chain = subentry;
		    else entry->subfiles = subentry;
		    prev = subentry;

		    /* Clear out this member's symbols from the ranlib data
		       so that following passes won't waste time on them.  */

		    for (j = 0; j < number_of_ranlibs; j++)
		      {
			if (ranlib_base[j].ran_off == offset)
			  ranlib_base[j].ran_un.ran_strx = -1;
		      }
		  }

		if (subentry->object_format == AOUT) {
		  /* We'll read the strings again if we need them again.  */
		  free (subentry->strings);
		  subentry->strings = 0;
		}
	      }
	  }
    }

  free (ranlib_data);
}

/* Search a library that has a MIPS_RANLIB_MEMBERNAME member.
   DESC is a descriptor on which the library is open.
     The file pointer is assumed to point at the MIPS_RANLIB_MEMBERNAME data.
   ENTRY is the library's file_entry.
   MEMBER_LENGTH is the length of the MIPS_RANLIB_MEMBERNAME data.  */

void
mipsranlib_library (desc, entry, member_length)
     int desc;
     struct file_entry *entry;
     int member_length;
{
  mipsranlib_nelm_t *mipsranlib_data = (mipsranlib_nelm_t *) xmalloc (member_length);
  register struct mipsranlib *mipsranlib_base;
  char *sym_name_base;
  int number_of_mipsranlibs;
  mipsranlib_ssiz_t length_of_strings;
  int not_finished;
  int bytes_read;
  register int i;
  struct file_entry *prev = 0;
  off_t prev_offset = 0;

  bytes_read = read (desc, mipsranlib_data, member_length);
  if (bytes_read != member_length) {
itsbad:
    fatal_with_file ("malformatted member (%s) in ",
    			entry, MIPS_RANLIB_MEMBERNAME);
  }

  number_of_mipsranlibs = *mipsranlib_data;
  if (number_of_mipsranlibs < 0 ||
       number_of_mipsranlibs * sizeof (struct mipsranlib) + sizeof(mipsranlib_nelm_t) + sizeof(mipsranlib_ssiz_t) > member_length)
    goto itsbad;

  mipsranlib_base = (struct mipsranlib *) (mipsranlib_data + 1);
  length_of_strings = *(mipsranlib_ssiz_t *) (mipsranlib_base + number_of_mipsranlibs);

  if (length_of_strings < 0
      || number_of_mipsranlibs * sizeof (struct mipsranlib) + length_of_strings
	 + sizeof(mipsranlib_nelm_t) + sizeof(mipsranlib_ssiz_t) > member_length)
    goto itsbad;

  sym_name_base = sizeof (length_of_strings) + (char *) (mipsranlib_base + number_of_mipsranlibs);

  /* Check all the string indexes for validity.  */

  for (i = 0; i < number_of_mipsranlibs; i++)
    {
      register off_t index = mipsranlib_base[i].ran_un.ran_strx;
      if (index < 0 || index >= length_of_strings
	  || (index && *(sym_name_base + index - 1)))
        goto itsbad;
    }

  /* Search the mipsranlib data for members to load.
     Do this until one whole pass finds nothing to load.  */

  not_finished = 1;
  while (not_finished)
    {
      not_finished = 0;

      /* Scan all the symbols mentioned in the mipsranlib for ones that we need.
	 Load the library members that contain such symbols.  */

      for (i = 0;
	   (i < number_of_mipsranlibs
	    && (undefined_global_sym_count || common_defined_global_count));
	   i++)
	/* NOTE: if ran_off is negative this is a common and
	   0-ran_off is its size. We skip it */
	if (mipsranlib_base[i].ran_off > 0)
	  {
	    register symbol *sp;

	    sp = getsym_soft (sym_name_base
			      + mipsranlib_base[i].ran_un.ran_strx);

	    /* If we find a symbol that appears to be needed, think carefully
	       about the archive member that the symbol is in.  */

	    if (sp && ((sp->referenced && !sp->defined)
		       || (sp->defined && sp->max_common_size)))
	      {
		int junk;
		register int j;
		register off_t offset = mipsranlib_base[i].ran_off;
		struct file_entry *subentry;

		/* Don't think carefully about any archive member
		   more than once in a given pass.  */

		if (prev_offset == offset)
		  continue;
		prev_offset = offset;

		/* Read the symbol table of the archive member.  */

		subentry = decode_library_subfile (desc, entry, offset, &junk);
		if (subentry == 0)
		  fatal_with_file ("invalid offset for %s in symbol table of",
			 entry,
			 sym_name_base + mipsranlib_base[i].ran_un.ran_strx);
		read_entry_symbols (desc, subentry);
		if (subentry->strings == 0) {
			subentry->strings = malloc (subentry->string_size);
			read_entry_strings (desc, subentry);
		}

		/* Now scan the symbol table and decide whether to load.  */

		if (!subfile_wanted_p (subentry))
		  {
		    free (subentry->symbols);
		    free (subentry);
		  }
		else
		  {
		    /* This member is needed; load it.
		       Since we are loading something on this pass,
		       we must make another pass through the mipsranlib data.  */

		    not_finished = 1;

		    enter_file_symbols (subentry);

		    if (prev)
		      prev->chain = subentry;
		    else entry->subfiles = subentry;
		    prev = subentry;

		    /* Clear out this member's symbols from the mipsranlib data
		       so that following passes won't waste time on them.  */

		    for (j = 0; j < number_of_mipsranlibs; j++)
		      {
			if (mipsranlib_base[j].ran_off == offset)
			  mipsranlib_base[j].ran_un.ran_strx = -1;
		      }
		  }

		if (subentry->object_format == AOUT) {
		  /* We'll read the strings again if we need them again.  */
		  free (subentry->strings);
		  subentry->strings = 0;
		}
	      }
	  }
    }

  free (mipsranlib_data);
}

/* Search a library that has no RANLIB_MEMBERNAME.
   ENTRY is the library's file_entry.
   DESC is the descriptor it is open on.  */

void
linear_library (desc, entry)
     int desc;
     struct file_entry *entry;
{
  register struct file_entry *prev = 0;
  register int this_subfile_offset = SARMAG;

  while (undefined_global_sym_count || common_defined_global_count)
    {
      int member_length;
      register struct file_entry *subentry;

      subentry = decode_library_subfile (desc, entry, this_subfile_offset,
					 &member_length);

      if (!subentry) return;

      read_entry_symbols (desc, subentry);
      if (subentry->strings == 0) {
	  subentry->strings = (char *) alloca (subentry->string_size);
          read_entry_strings (desc, subentry);
      }

      if (!subfile_wanted_p (subentry))
	{
	  free (subentry->symbols);
	  free (subentry);
	}
      else
	{
	  enter_file_symbols (subentry);

	  if (prev)
	    prev->chain = subentry;
	  else entry->subfiles = subentry;
	  prev = subentry;
	  subentry->strings = 0; /* Since space will dissapear on return */
	}

      this_subfile_offset += member_length + sizeof (struct ar_hdr);
      if (this_subfile_offset & 1) this_subfile_offset++;
    }
}

/* ENTRY is an entry for a library member.
   Its symbols have been read into core, but not entered.
   Return nonzero if we ought to load this member.  */

int
subfile_wanted_p (entry)
     struct file_entry *entry;
{
  register struct nlist *p;
  register struct nlist *end = entry->symbols + entry->n_symbols;

  for (p = entry->symbols; p < end; p++)
    {
      register int type = p->n_type;
      register char *name = p->n_un.n_strx + entry->strings;

      /* If the symbol has an interesting definition, we could
	 potentially want it.  */
      if (type & N_EXT
	  && (type != (N_UNDF | N_EXT) || p->n_value)
	  && !SET_ELEMENT_P (type)
	  && !set_element_prefixed_p (name))
	{
	  register symbol *sp = getsym_soft (name);

	  /* If this symbol has not been hashed, we can't be looking for it. */

	  if (!sp) continue;

	  if ((sp->referenced && !sp->defined)
	      || (sp->defined && sp->max_common_size))
	    {
	      /* This is a symbol we are looking for.  It is either
	         not yet defined or defined as a common.  */

	      if (type == (N_UNDF | N_EXT))
		{
		  /* Symbol being defined as common.
		     Remember this, but don't load subfile just for this.  */

		  /* If it didn't used to be common, up the count of
		     common symbols.  */
		  if (!sp->max_common_size)
		    common_defined_global_count++;

		  if (sp->max_common_size < p->n_value)
		    sp->max_common_size = p->n_value;
		  if (!sp->defined)
		    undefined_global_sym_count--;
		  sp->defined = 1;
		  continue;
		}

	      if (write_map)
		{
		  print_file_name (entry, stdout);
		  fprintf (stdout, " needed due to %s\n", sp->name);
		}
	      return 1;
	    }
	}
    }

  return 0;
}

void consider_file_section_lengths (), relocate_file_addresses ();

/* Having entered all the global symbols and found the sizes of sections
   of all files to be linked, make all appropriate deductions from this data.

   We propagate global symbol values from definitions to references.
   We compute the layout of the output file and where each input file's
   contents fit into it.  */

void
digest_symbols ()
{
  register int i;
  int setv_fill_count;

  if (trace_files)
    fprintf (stderr, "Digesting symbol information:\n\n");

  /* Compute total size of sections */

  each_file (consider_file_section_lengths, 0);

  /* If necessary, pad text section to full page in the file.
     Include the padding in the text segment size.  */

  if (magic == ZMAGIC)
    {
      int text_end = text_size + BSD_N_TXTOFF (&outheader);
      text_pad = ((text_end + page_size - 1) & (- page_size)) - text_end;
      text_size += text_pad;
    }

  outheader.a_text = text_size;

  /* Make the data segment address start in memory on a suitable boundary.  */

  if (! Tdata_flag_specified)
    data_start = outheader.a_tstart + text_size;

  /* Set up the set element vector */

  if (!relocatable_output)
    {
      /* The set sector size is the number of set elements + a word
         for each symbol for the length word at the beginning of the
	 vector, plus a word for each symbol for a zero at the end of
	 the vector (for incremental linking).  */
      set_sect_size
	= (2 * set_symbol_count + set_vector_count) * sizeof (unsigned long);
      set_sect_start = data_start + data_size;
      data_size += set_sect_size;
      set_vectors = (unsigned long *) xmalloc (set_sect_size);
      setv_fill_count = 0;

    }

  /* Compute start addresses of each file's sections and symbols.  */

  each_full_file (relocate_file_addresses, 0);

  /* Now, for each symbol, verify that it is defined globally at most once.
     Put the global value into the symbol entry.
     Common symbols are allocated here, in the BSS section.
     Each defined symbol is given a '->defined' field
      which is the correct N_ code for its definition,
      except in the case of common symbols with -r.
     Then make all the references point at the symbol entry
     instead of being chained together. */

  defined_global_sym_count = 0;

  for (i = 0; i < TABSIZE; i++)
    {
      register symbol *sp;
      for (sp = symtab[i]; sp; sp = sp->link)
	{
	  /* For each symbol */
	  register struct nlist *p, *next;
	  int defs = 0, com = sp->max_common_size;
	  struct nlist *first_definition;
	  for (p = sp->refs; p; p = next)
	    {
	      register int type = p->n_type;

	      if (SET_ELEMENT_P (type))
		{
		  if (relocatable_output)
		    fatal ("internal: global ref to set element with -r");
		  if (!defs++)
		    {
		      sp->value = set_sect_start
			+ setv_fill_count++ * sizeof (unsigned long);
		      sp->defined = N_SETV | N_EXT;
		      first_definition = p;
		    }
		  else if ((sp->defined & ~N_EXT) != N_SETV)
		    {
		      sp->multiply_defined = 1;
		      multiple_def_count++;
		    }
		  set_vectors[setv_fill_count++] = p->n_value;
		}
	      else if ((type & N_EXT) && type != (N_UNDF | N_EXT))
		{
		  /* non-common definition */
		  if (defs++ && sp->value != p->n_value)
		    {
		      sp->multiply_defined = 1;
		      multiple_def_count++;
		    }
		  sp->value = p->n_value;
		  sp->defined = type;
		  first_definition = p;
		}
	      next = (struct nlist *) p->n_un.n_name;
	      p->n_un.n_name = (char *) sp;
	    }
	  /* Allocate as common if defined as common and not defined for real */
	  if (com && !defs)
	    {
	      if (!relocatable_output || force_common_definition)
		{
		  int align = sizeof (int);

		  /* Round up to nearest sizeof (int).  I don't know
		     whether this is necessary or not (given that
		     alignment is taken care of later), but it's
		     traditional, so I'll leave it in.  Note that if
		     this size alignment is ever removed, ALIGN above
		     will have to be initialized to 1 instead of
		     sizeof (int).  */

		  com = (com + sizeof (int) - 1) & (- sizeof (int));

		  while (!(com & align))
		    align <<= 1;

		  align = align > MAX_ALIGNMENT ? MAX_ALIGNMENT : align;

		  bss_size = ((((bss_size + data_size + data_start)
			      + (align - 1)) & (- align))
			      - data_size - data_start);

		  sp->value = data_start + data_size + bss_size;
		  sp->defined = N_BSS | N_EXT;
		  bss_size += com;
		  if (write_map)
		    printf ("Allocating common %s: %x at %x\n",
			    sp->name, com, sp->value);
		}
	      else
		{
		  sp->defined = 0;
		  undefined_global_sym_count++;
		}
	    }
	  /* Set length word at front of vector and zero byte at end.
	     Reverse the vector itself to put it in file order.  */
	  if ((sp->defined & ~N_EXT) == N_SETV)
	    {
	      unsigned long length_word_index
		= (sp->value - set_sect_start) / sizeof (unsigned long);
	      unsigned long i, tmp;

	      set_vectors[length_word_index]
		= setv_fill_count - 1 - length_word_index;

	      /* Reverse the vector.  */
	      for (i = 1;
		   i < (setv_fill_count - length_word_index - 1) / 2 + 1;
		   i++)
		{
		  tmp = set_vectors[length_word_index + i];
		  set_vectors[length_word_index + i]
		    = set_vectors[setv_fill_count - i];
		  set_vectors[setv_fill_count - i] = tmp;
		}

	      set_vectors[setv_fill_count++] = 0;
	    }
	  if (sp->defined)
	    defined_global_sym_count++;
	}
    }

  if (end_symbol)		/* These are null if -r.  */
    {
      etext_symbol->value = text_size + text_start;
      edata_symbol->value = data_start + data_size;
      end_symbol->value = data_start + data_size + bss_size;
    }

  /* Figure the data_pad now, so that it overlaps with the bss addresses.  */

  if (specified_data_size && specified_data_size > data_size)
    data_pad = specified_data_size - data_size;

  if (magic == ZMAGIC)
    data_pad = ((data_pad + data_size + page_size - 1) & (- page_size))
               - data_size;

  bss_size -= data_pad;
  if (bss_size < 0) bss_size = 0;

  data_size += data_pad;

}

/* Accumulate the section sizes of input file ENTRY
   into the section sizes of the output file.  */

void
consider_file_section_lengths (entry)
     register struct file_entry *entry;
{
  if (entry->just_syms_flag)
    return;

  entry->text_start_address = text_size;
  /* If there were any vectors, we need to chop them off */
  text_size += entry->header.a_text;

  entry->data_start_address = data_size;
  data_size += entry->header.a_data;

  entry->bss_start_address = bss_size;
  bss_size += entry->header.a_bss;

  text_reloc_size += entry->header.a_trsize;
  data_reloc_size += entry->header.a_drsize;

}

/* Determine where the sections of ENTRY go into the output file,
   whose total section sizes are already known.
   Also relocate the addresses of the file's local and debugger symbols.  */

void
relocate_file_addresses (entry)
     register struct file_entry *entry;
{
  entry->text_start_address += text_start;
  /* Note that `data_start' and `data_size' have not yet been
     adjusted for `data_pad'.  If they had been, we would get the wrong
     results here.  */
  entry->data_start_address += data_start;
  entry->bss_start_address += data_start + data_size;

  {
    register struct nlist *p;
    register struct nlist *end = entry->symbols + entry->n_symbols;

    for (p = entry->symbols; p < end; p++)
      {
	/* If this belongs to a section, update it by the section's start address */
	register int type = p->n_type & N_TYPE;

	switch (type)
	  {
	  case N_TEXT:
	  case N_SETT:
	    p->n_value += entry->text_start_address;
	    break;
	  case N_DATA:
	  case N_SETV:
	  case N_SETD:
	    /* A symbol whose value is in the data section
	       is present in the input file as if the data section
	       started at an address equal to the length of the file's text.  */
	    p->n_value += entry->data_start_address - entry->header.a_text;
	    break;
	  case N_BSS:
	  case N_SETB:
	    /* likewise for symbols with value in BSS.  */
	    p->n_value += entry->bss_start_address
	      - entry->header.a_text - entry->header.a_data;
	    break;
	  }
      }
  }
}

void describe_file_sections (), list_file_locals ();

/* Print a complete or partial map of the output file.  */

void
print_symbols (outfile)
     FILE *outfile;
{
  register int i;

  fprintf (outfile, "\nFiles:\n\n");

  each_file (describe_file_sections, outfile);

  fprintf (outfile, "\nGlobal symbols:\n\n");

  for (i = 0; i < TABSIZE; i++)
    {
      register symbol *sp;
      for (sp = symtab[i]; sp; sp = sp->link)
	{
	  if (sp->defined == 1)
	    fprintf (outfile, "  %s: common, length 0x%x\n", sp->name, sp->max_common_size);
	  if (sp->defined)
	    fprintf (outfile, "  %s: 0x%x\n", sp->name, sp->value);
	  else if (sp->referenced)
	    fprintf (outfile, "  %s: undefined\n", sp->name);
	}
    }

  each_file (list_file_locals, outfile);
}

void
describe_file_sections (entry, outfile)
     struct file_entry *entry;
     FILE *outfile;
{
  fprintf (outfile, "  ");
  print_file_name (entry, outfile);
  if (entry->just_syms_flag)
    fprintf (outfile, " symbols only\n", 0);
  else
    fprintf (outfile, " text %x(%x), data %x(%x), bss %x(%x) hex\n",
	     entry->text_start_address, entry->header.a_text,
	     entry->data_start_address, entry->header.a_data,
	     entry->bss_start_address, entry->header.a_bss);
}

void
list_file_locals (entry, outfile)
     struct file_entry *entry;
     FILE *outfile;
{
  register struct nlist
    *p,
    *end = entry->symbols + entry->n_symbols;

  if (entry->strings == 0) {
      entry->strings = (char *) alloca (entry->string_size);
      read_entry_strings (file_open (entry), entry);
  }

  fprintf (outfile, "\nLocal symbols of ");
  print_file_name (entry, outfile);
  fprintf (outfile, ":\n\n");

  for (p = entry->symbols; p < end; p++)
    /* If this is a definition,
       update it if necessary by this file's start address.  */
    if (!(p->n_type & (N_STAB | N_EXT)))
      fprintf (outfile, "  %s: 0x%x\n",
	       entry->strings + p->n_un.n_strx, p->n_value);

  entry->strings = 0;		/* All done with them.  */
}


/* Static vars for do_warnings and subroutines of it */
int list_unresolved_refs;	/* List unresolved refs */
int list_warning_symbols;	/* List warning syms */
int list_multiple_defs;		/* List multiple definitions */

/*
 * Structure for communication between do_file_warnings and it's
 * helper routines.  Will in practice be an array of three of these:
 * 0) Current line, 1) Next line, 2) Source file info.
 */
struct line_debug_entry
{
  int line;
  char *filename;
  struct nlist *sym;
};

void qsort ();
/*
 * Helper routines for do_file_warnings.
 */

/* Return an integer less than, equal to, or greater than 0 as per the
   relation between the two relocation entries.  Used by qsort.  */

int
relocation_entries_relation (rel1, rel2)
     struct relocation_info *rel1, *rel2;
{
  return RELOC_ADDRESS(rel1) - RELOC_ADDRESS(rel2);
}

/* Moves to the next debugging symbol in the file.  USE_DATA_SYMBOLS
   determines the type of the debugging symbol to look for (DSLINE or
   SLINE).  STATE_POINTER keeps track of the old and new locatiosn in
   the file.  It assumes that state_pointer[1] is valid; ie
   that it.sym points into some entry in the symbol table.  If
   state_pointer[1].sym == 0, this routine should not be called.  */

int
next_debug_entry (use_data_symbols, state_pointer)
     register int use_data_symbols;
     /* Next must be passed by reference! */
     struct line_debug_entry state_pointer[3];
{
  register struct line_debug_entry
    *current = state_pointer,
    *next = state_pointer + 1,
    /* Used to store source file */
    *source = state_pointer + 2;
  struct file_entry *entry = (struct file_entry *) source->sym;

  current->sym = next->sym;
  current->line = next->line;
  current->filename = next->filename;

  while (++(next->sym) < (entry->symbols + entry->n_symbols))
    {
      /* n_type is a char, and N_SOL, N_EINCL and N_BINCL are > 0x80, so
       * may look negative...therefore, must mask to low bits
       */
      switch (next->sym->n_type & 0xff)
	{
	case N_SLINE:
	  if (use_data_symbols) continue;
	  next->line = next->sym->n_desc;
	  return 1;
	case N_DSLINE:
	  if (!use_data_symbols) continue;
	  next->line = next->sym->n_desc;
	  return 1;
#ifdef HAVE_SUN_STABS
	case N_EINCL:
	  next->filename = source->filename;
	  continue;
#endif
	case N_SO:
	  source->filename = next->sym->n_un.n_strx + entry->strings;
	  source->line++;
#ifdef HAVE_SUN_STABS
	case N_BINCL:
#endif
	case N_SOL:
	  next->filename
	    = next->sym->n_un.n_strx + entry->strings;
	default:
	  continue;
	}
    }
  next->sym = (struct nlist *) 0;
  return 0;
}

/* Create a structure to save the state of a scan through the debug
   symbols.  USE_DATA_SYMBOLS is set if we should be scanning for
   DSLINE's instead of SLINE's.  entry is the file entry which points
   at the symbols to use.  */

struct line_debug_entry *
init_debug_scan (use_data_symbols, entry)
     int use_data_symbols;
     struct file_entry *entry;
{
  struct line_debug_entry
    *state_pointer
      = (struct line_debug_entry *)
	xmalloc (3 * sizeof (struct line_debug_entry));
  register struct line_debug_entry
    *current = state_pointer,
    *next = state_pointer + 1,
    *source = state_pointer + 2; /* Used to store source file */

  struct nlist *tmp;

  for (tmp = entry->symbols;
       tmp < (entry->symbols + entry->n_symbols);
       tmp++)
    if (tmp->n_type == (int) N_SO)
      break;

  if (tmp >= (entry->symbols + entry->n_symbols))
    {
      /* I believe this translates to "We lose" */
      current->filename = next->filename = entry->filename;
      current->line = next->line = -1;
      current->sym = next->sym = (struct nlist *) 0;
      return state_pointer;
    }

  next->line = source->line = 0;
  next->filename = source->filename
    = (tmp->n_un.n_strx + entry->strings);
  source->sym = (struct nlist *) entry;
  next->sym = tmp;

  next_debug_entry (use_data_symbols, state_pointer); /* To setup next */

  if (!next->sym)		/* No line numbers for this section; */
				/* setup output results as appropriate */
    {
      if (source->line)
	{
	  current->filename = source->filename = entry->filename;
	  current->line = -1;	/* Don't print lineno */
	}
      else
	{
	  current->filename = source->filename;
	  current->line = 0;
	}
      return state_pointer;
    }


  next_debug_entry (use_data_symbols, state_pointer); /* To setup current */

  return state_pointer;
}

/* Takes an ADDRESS (in either text or data space) and a STATE_POINTER
   which describes the current location in the implied scan through
   the debug symbols within the file which ADDRESS is within, and
   returns the source line number which corresponds to ADDRESS.  */

int
address_to_line (address, state_pointer)
     unsigned long address;
     /* Next must be passed by reference! */
     struct line_debug_entry state_pointer[3];
{
  struct line_debug_entry
    *current = state_pointer,
    *next = state_pointer + 1;
  struct line_debug_entry *tmp_pointer;

  int use_data_symbols;

  if (next->sym)
    use_data_symbols = (next->sym->n_type & N_TYPE) == N_DATA;
  else
    return current->line;

  /* Go back to the beginning if we've already passed it.  */
  if (current->sym->n_value > address)
    {
      tmp_pointer = init_debug_scan (use_data_symbols,
				     (struct file_entry *)
				     ((state_pointer + 2)->sym));
      state_pointer[0] = tmp_pointer[0];
      state_pointer[1] = tmp_pointer[1];
      state_pointer[2] = tmp_pointer[2];
      free (tmp_pointer);
    }

  /* If we're still in a bad way, return -1, meaning invalid line.  */
  if (current->sym->n_value > address)
    return -1;

  while (next->sym
	 && next->sym->n_value <= address
	 && next_debug_entry (use_data_symbols, state_pointer))
    ;
  return current->line;
}


/* Macros for manipulating bitvectors.  */
#define	BIT_SET_P(bv, index)	((bv)[(index) >> 3] & 1 << ((index) & 0x7))
#define	SET_BIT(bv, index)	((bv)[(index) >> 3] |= 1 << ((index) & 0x7))

/* This routine will scan through the relocation data of file ENTRY,
   printing out references to undefined symbols and references to
   symbols defined in files with N_WARNING symbols.  If DATA_SEGMENT
   is non-zero, it will scan the data relocation segment (and use
   N_DSLINE symbols to track line number); otherwise it will scan the
   text relocation segment.  Warnings will be printed on the output
   stream OUTFILE.  Eventually, every nlist symbol mapped through will
   be marked in the NLIST_BITVECTOR, so we don't repeat ourselves when
   we scan the nlists themselves.  */

do_relocation_warnings (entry, data_segment, outfile, nlist_bitvector)
     struct file_entry *entry;
     int data_segment;
     FILE *outfile;
     unsigned char *nlist_bitvector;
{
  struct relocation_info
    *reloc_start = data_segment ? entry->datarel : entry->textrel,
    *reloc;
  int reloc_size
    = ((data_segment ? entry->header.a_drsize : entry->header.a_trsize)
       / sizeof (struct relocation_info));
  int start_of_segment
    = (data_segment ? entry->data_start_address : entry->text_start_address);
  struct nlist *start_of_syms = entry->symbols;
  struct line_debug_entry *state_pointer
    = init_debug_scan (data_segment != 0, entry);
  register struct line_debug_entry
    *current = state_pointer;
  /* Assigned to generally static values; should not be written into.  */
  char *errfmt;
  /* Assigned to alloca'd values cand copied into; should be freed
     when done.  */
  char *errmsg;
  int invalidate_line_number;

  /* We need to sort the relocation info here.  Sheesh, so much effort
     for one lousy error optimization. */

  qsort (reloc_start, reloc_size, sizeof (struct relocation_info),
	 relocation_entries_relation);

  for (reloc = reloc_start;
       reloc < (reloc_start + reloc_size);
       reloc++)
    {
      register struct nlist *s;
      register symbol *g;

      /* If the relocation isn't resolved through a symbol, continue */
      if (!RELOC_EXTERN_P(reloc))
	continue;

      s = &(entry->symbols[RELOC_SYMBOL(reloc)]);

      /* Local symbols shouldn't ever be used by relocation info, so
	 the next should be safe.
	 This is, of course, wrong.  References to local BSS symbols can be
	 the targets of relocation info, and they can (must) be
	 resolved through symbols.  However, these must be defined properly,
	 (the assembler would have caught it otherwise), so we can
	 ignore these cases.  */
      if (!(s->n_type & N_EXT))
	continue;

      g = (symbol *) s->n_un.n_name;
      errmsg = 0;

      if (!g->defined && list_unresolved_refs) /* Reference */
	{
	  /* Mark as being noted by relocation warning pass.  */
	  SET_BIT (nlist_bitvector, s - start_of_syms);

	  if (g->undef_refs >= MAX_UREFS_PRINTED)    /* Listed too many */
	    continue;

	  /* Undefined symbol which we should mention */

	  if (++(g->undef_refs) == MAX_UREFS_PRINTED)
	    {
	      errfmt = "More undefined symbol %s refs follow";
	      invalidate_line_number = 1;
	    }
	  else
	    {
	      errfmt = "Undefined symbol %s referenced from %s segment";
	      invalidate_line_number = 0;
	    }
	}
      else					     /* Defined */
	{
	  /* Potential symbol warning here */
	  if (!g->warning) continue;

	  /* Mark as being noted by relocation warning pass.  */
	  SET_BIT (nlist_bitvector, s - start_of_syms);

	  errfmt = 0;
	  errmsg = g->warning;
	  invalidate_line_number = 0;
	}


      /* If errfmt == 0, errmsg has already been defined.  */
      if (errfmt != 0)
	{
	  char *nm;

	  if (demangler == NULL || (nm = (*demangler)(g->name)) == NULL)
	    nm = g->name;
	  errmsg = xmalloc (strlen (errfmt) + strlen (nm) + 1);
	  sprintf (errmsg, errfmt, nm, data_segment ? "data" : "text");
	  if (nm != g->name)
	    free (nm);
	}

      address_to_line (RELOC_ADDRESS (reloc) + start_of_segment,
		       state_pointer);

      if (current->line >=0)
	fprintf (outfile, "%s:%d: %s\n", current->filename,
		 invalidate_line_number ? 0 : current->line, errmsg);
      else
	fprintf (outfile, "%s: %s\n", current->filename, errmsg);

      if (errfmt != 0)
	free (errmsg);
    }

  free (state_pointer);
}

/* Print on OUTFILE a list of all warnings generated by references
   and/or definitions in the file ENTRY.  List source file and line
   number if possible, just the .o file if not. */

void
do_file_warnings (entry, outfile)
     struct file_entry *entry;
     FILE *outfile;
{
  int number_of_syms = entry->n_symbols;
  unsigned char *nlist_bitvector
    = (unsigned char *) alloca ((number_of_syms >> 3) + 1);
  struct line_debug_entry *text_scan, *data_scan;
  int i;
  char *errfmt, *file_name;
  int line_number;
  int dont_allow_symbol_name;

  bzero (nlist_bitvector, (number_of_syms >> 3) + 1);

  /* Read in the files strings if they aren't available */
  if (!entry->strings)
    {
      int desc;

      entry->strings = (char *) alloca (entry->string_size);
      desc = file_open (entry);
      read_entry_strings (desc, entry);
    }

  read_file_relocation (entry);

  /* Do text warnings based on a scan through the relocation info.  */
  do_relocation_warnings (entry, 0, outfile, nlist_bitvector);

  /* Do data warnings based on a scan through the relocation info.  */
  do_relocation_warnings (entry, 1, outfile, nlist_bitvector);

  /* Scan through all of the nlist entries in this file and pick up
     anything that the scan through the relocation stuff didn't.  */

  text_scan = init_debug_scan (0, entry);
  data_scan = init_debug_scan (1, entry);

  for (i = 0; i < number_of_syms; i++)
    {
      struct nlist *s;
      struct glosym *g;

      s = entry->symbols + i;

      if (!(s->n_type & N_EXT))
	continue;

      g = (symbol *) s->n_un.n_name;
      dont_allow_symbol_name = 0;

      if (list_multiple_defs && g->multiply_defined)
	{
	  errfmt = "Definition of symbol %s (multiply defined)";
	  switch (s->n_type)
	    {
	    case N_TEXT | N_EXT:
	      line_number = address_to_line (s->n_value, text_scan);
	      file_name = text_scan[0].filename;
	      break;
	    case N_DATA | N_EXT:
	      line_number = address_to_line (s->n_value, data_scan);
	      file_name = data_scan[0].filename;
	      break;
	    case N_SETA | N_EXT:
	    case N_SETT | N_EXT:
	    case N_SETD | N_EXT:
	    case N_SETB | N_EXT:
	      if (g->multiply_defined == 2)
		continue;
	      errfmt = "First set element definition of symbol %s (multiply defined)";
	      break;
	    default:
	      continue;		/* Don't print out multiple defs
				   at references.  */
	    }
	}
      else if (BIT_SET_P (nlist_bitvector, i))
	continue;
      else if (list_unresolved_refs && !g->defined)
	{
	  if (g->undef_refs >= MAX_UREFS_PRINTED)
	    continue;

	  if (++(g->undef_refs) == MAX_UREFS_PRINTED)
	    errfmt = "More undefined \"%s\" refs follow";
	  else
	    errfmt = "Undefined symbol \"%s\" referenced";
	  line_number = -1;
	}
      else if (g->warning)
	{
	  /* There are two cases in which we don't want to
	     do this.  The first is if this is a definition instead of
	     a reference.  The second is if it's the reference used by
	     the warning stabs itself.  */
	  if (s->n_type != (N_EXT | N_UNDF)
	      || (i && (s-1)->n_type == N_WARNING))
	    continue;

	  errfmt = g->warning;
	  line_number = -1;
	  dont_allow_symbol_name = 1;
	}
      else
	continue;

      if (line_number == -1)
	fprintf (outfile, "%s: ", entry->filename);
      else
	fprintf (outfile, "%s:%d: ", file_name, line_number);

      if (dont_allow_symbol_name)
	fprintf (outfile, "%s", errfmt);
      else
	{
	  char *nm;
	  if (demangler != NULL && (nm = (*demangler)(g->name)) != NULL)
	    {
	      fprintf (outfile, errfmt, nm);
	      free (nm);
	    }
	  else
	    fprintf (outfile, errfmt, g->name);
	}

      fputc ('\n', outfile);
    }
  free (text_scan);
  free (data_scan);
  entry->strings = 0;		/* Since it will dissapear anyway.  */
}

do_warnings (outfile)
     FILE *outfile;
{
  list_unresolved_refs = !relocatable_output && undefined_global_sym_count;
  list_warning_symbols = warning_count;
  list_multiple_defs = multiple_def_count != 0;

  if (!(list_unresolved_refs ||
	list_warning_symbols ||
	list_multiple_defs      ))
    /* No need to run this routine */
    return;

  each_file (do_file_warnings, outfile);

  if (list_unresolved_refs || list_multiple_defs)
    make_executable = 0;
}

/* Write the output file */

void
write_output ()
{
  struct stat statbuf;
  int filemode;

  (void) unlink (output_filename);
  outdesc = open (output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (outdesc < 0) perror_name (output_filename);

  if (fstat (outdesc, &statbuf) < 0)
    perror_name (output_filename);

  filemode = statbuf.st_mode;

  chmod (output_filename, filemode & ~0111);

  /* Output the a.out header.  */
  write_header ();

  /* Output the text and data segments, relocating as we go.  */
  cur_base_register = data_start;
  write_text ();
  cur_base_register = data_start;
  write_data ();

  /* Output the merged relocation info, if requested with `-r'.  */
  if (relocatable_output)
    write_rel ();

  /* Output the symbol table (both globals and locals).  */
  write_syms ();

  /* Copy any GDB symbol segments from input files.  */
  write_symsegs ();

  close (outdesc);

  if (chmod (output_filename, filemode | 0111) == -1)
    perror_name (output_filename);
}

void modify_location (), perform_relocation (), copy_text (), copy_data ();

void
write_header ()
{
  BSD_N_SET_MAGIC (&outheader, magic);
  outheader.a_text = text_size;
  outheader.a_data = data_size;
  outheader.a_bss = bss_size;
  outheader.a_entry = (entry_symbol ? entry_symbol->value
		       : text_start + entry_offset);
  outheader.a_tstart = text_start;
  outheader.a_dstart = data_start;

  if (strip_symbols == STRIP_ALL)
    nsyms = 0;
  else
    {
      nsyms = (defined_global_sym_count
	       + undefined_global_sym_count);
      if (discard_locals == DISCARD_L)
	nsyms += non_L_local_sym_count;
      else if (discard_locals == DISCARD_NONE)
	nsyms += local_sym_count;
      /* One extra for following reference on indirects */
      if (relocatable_output)
	nsyms += set_symbol_count + global_indirect_count;
    }

  if (strip_symbols == STRIP_NONE)
    nsyms += debugger_sym_count;

  outheader.a_syms = nsyms * sizeof (struct nlist);

  if (relocatable_output)
    {
      outheader.a_trsize = text_reloc_size;
      outheader.a_drsize = data_reloc_size;
    }
  else
    {
      outheader.a_trsize = 0;
      outheader.a_drsize = 0;
    }

  if (pseudo_coff_image)
    write_coff_header();
  else
    write_aout_header();
}

write_aout_header()
{
  mywrite (&outheader, sizeof (struct exec), 1, outdesc);

  /* Output whatever padding is required in the executable file
     between the header and the start of the text.  */

  padfile (BSD_N_TXTOFF (&outheader) - sizeof outheader, outdesc);
}

/* This is not serious.  Just to convince the OSF kernel to execute
   our images, which are really targeted at Mach 3.0 with the 4.3 BSD
   server [or our user-level A.OUT loader] */ 

write_coff_header() {
  struct coff_hat h;

  memset(&h, 0, sizeof(struct coff_hat));

  h.f.f_magic = 0x183;
  h.f.f_nscns = 2;
  h.f.f_timdat = time(0);
  relocatable_output = 0;    /* sorry, cant do that */
  h.f.f_symptr = 0;                          /* ???????????????? */
  h.f.f_nsyms = outheader.a_syms;
  h.f.f_opthdr = 0;
  h.f.f_flags = 0x1007;

  h.a.magic = outheader.a_magic;
  h.a.vstamp = 0x900;
  h.a.tsize = outheader.a_text;
  h.a.dsize = outheader.a_data;
  h.a.bsize = outheader.a_bss;
  h.a.entry = outheader.a_entry;
  h.a.text_start = outheader.a_tstart;
  h.a.data_start = outheader.a_dstart;
  h.a.bss_start = outheader.a_dstart + outheader.a_data;
  h.a.gprmask = 0;
  h.a.fprmask = 0;
  h.a.gp_value = 0;

  bcopy(".text\0\0", h.s0.s_name, 8);
  h.s0.s_paddr =
  h.s0.s_vaddr = outheader.a_tstart;
  h.s0.s_size = outheader.a_text;
  h.s0.s_scnptr = 0;                          /* ???????????????? */
  h.s0.s_relptr = 0;
  h.s0.s_lnnoptr = 0;
  h.s0.s_nreloc = 0;
  h.s0.s_nlnno = 0;
  h.s0.s_flags = 0x20;

  bcopy(".data\0\0", h.s1.s_name, 8);
  h.s1.s_paddr =
  h.s1.s_vaddr = outheader.a_dstart;
  h.s1.s_scnptr = h.s0.s_scnptr + h.s0.s_size + text_pad;
  h.s1.s_size = outheader.a_data;
  h.s1.s_relptr = 0;
  h.s1.s_lnnoptr = 0;
  h.s1.s_nreloc = 0;
  h.s1.s_nlnno = 0;
  h.s1.s_flags = 0x40;

  mywrite (&h, sizeof (h), 1, outdesc);

  /* Output whatever padding is required in the executable file
     between the header and the start of the text.  */

  padfile (COFF_N_TXTOFF (&h) - sizeof h, outdesc);
}

/* Relocate the text segment of each input file
   and write to the output file.  */

void
write_text ()
{
  if (trace_files)
    fprintf (stderr, "Copying and relocating text:\n\n");

  each_full_file (copy_text, 0);
  file_close ();

  if (trace_files)
    fprintf (stderr, "\n");

  padfile (text_pad, outdesc);
}

int
text_offset (entry)
     struct file_entry *entry;
{
  switch (entry->object_format) {
  case AOUT:
    return entry->starting_offset + BSD_N_TXTOFF (&entry->header);
  case COFF:
    return entry->starting_offset +
    	sizeof(struct exechdr) + entry->coff_nscns * sizeof(struct scnhdr);
  default:
    fatal_with_file("text_offset: bad format ", entry, 0);
  }
}

/* Read in all of the relocation information */

void
read_relocation ()
{
  each_full_file (read_file_relocation, 0);
}

/* Read in the relocation sections of ENTRY if necessary */

void
read_file_relocation (entry)
     struct file_entry *entry;
{
  register struct relocation_info *reloc;
  int desc;
  int read_return;

  desc = -1;

  if (entry->object_format == COFF) {
    desc = file_open (entry);
    xlate_coff_relocs(desc, entry);
    return;
  }

  if (entry->object_format != AOUT)
    fatal_with_file("read_file_relocation: bad header type in", entry, 0);

  if (!entry->textrel)
    {
      reloc = (struct relocation_info *) xmalloc (entry->header.a_trsize);
      desc = file_open (entry);
      lseek (desc,
	     text_offset (entry) + entry->header.a_text + entry->header.a_data,
	     L_SET);
      if (entry->header.a_trsize != (read_return = read (desc, reloc, entry->header.a_trsize)))
	{
	  fprintf (stderr, "Return from read: %d\n", read_return);
	  fatal_with_file ("premature eof in text relocation of ", entry, 0);
	}
      entry->textrel = reloc;
    }

  if (!entry->datarel)
    {
      reloc = (struct relocation_info *) xmalloc (entry->header.a_drsize);
      if (desc == -1) desc = file_open (entry);
      lseek (desc,
	     text_offset (entry) + entry->header.a_text
	     + entry->header.a_data + entry->header.a_trsize,
	     L_SET);
      if (entry->header.a_drsize != read (desc, reloc, entry->header.a_drsize))
	fatal_with_file ("premature eof in data relocation of ", entry, 0);
      entry->datarel = reloc;
    }
}

/* Read the text segment contents of ENTRY, relocate them,
   and write the result to the output file.
   If `-r', save the text relocation for later reuse.  */

void
copy_text (entry)
     struct file_entry *entry;
{
  register char *bytes;
  register int desc;
  register struct relocation_info *reloc;

  if (trace_files)
    prline_file_name (entry, stderr);

  desc = file_open (entry);

  /* Allocate space for the file's text section */

  bytes = (char *) alloca (entry->header.a_text);

  /* Deal with relocation information however is appropriate */

  if (entry->textrel)  reloc = entry->textrel;
  else if (relocatable_output)
    {
      read_file_relocation (entry);
      reloc = entry->textrel;
    }
  else
    {
      if (entry->object_format == COFF) {
        xlate_coff_relocs(desc, entry);
	reloc = entry->textrel;
      } else {
        reloc = (struct relocation_info *) alloca (entry->header.a_trsize);
        lseek (desc, text_offset (entry) + entry->header.a_text + entry->header.a_data, 0);
        if (entry->header.a_trsize != read (desc, reloc, entry->header.a_trsize))
	  fatal_with_file ("premature eof in text relocation of ", entry, 0);
      }
    }

  /* Read the text section into core.  */

  lseek (desc, text_offset (entry), 0);
  if (entry->header.a_text != read (desc, bytes, entry->header.a_text))
    fatal_with_file ("premature eof in text section of ", entry, 0);


  /* Relocate the text according to the text relocation.  */

  perform_relocation (bytes, entry->text_start_address, entry->header.a_text,
		      reloc, entry->header.a_trsize, entry);

  /* Write the relocated text to the output file.  */

  mywrite (bytes, 1, entry->header.a_text, outdesc);

  cur_base_register += entry->header.a_data;
}

/* Relocate the data segment of each input file
   and write to the output file.  */

void
write_data ()
{
  if (trace_files)
    fprintf (stderr, "Copying and relocating data:\n\n");

  each_full_file (copy_data, 0);
  file_close ();

  /* Write out the set element vectors.  See digest symbols for
     description of length of the set vector section.  */

  if (set_vector_count)
    mywrite (set_vectors, 2 * set_symbol_count + set_vector_count,
	     sizeof (unsigned long), outdesc);

  if (trace_files)
    fprintf (stderr, "\n");

  padfile (data_pad, outdesc);
}

/* Read the data segment contents of ENTRY, relocate them,
   and write the result to the output file.
   If `-r', save the data relocation for later reuse.
   See comments in `copy_text'.  */

void
copy_data (entry)
     struct file_entry *entry;
{
  register struct relocation_info *reloc;
  register char *bytes;
  register int desc;

  if (trace_files)
    prline_file_name (entry, stderr);

  desc = file_open (entry);

  bytes = (char *) alloca (entry->header.a_data);

  if (entry->datarel) reloc = entry->datarel;
  else if (relocatable_output)	/* Will need this again */
    {
      read_file_relocation (entry);
      reloc = entry->datarel;
    }
  else
    {
      if (entry->object_format == COFF) {
        xlate_coff_relocs(desc, entry);
	reloc = entry->datarel;
      } else {
        reloc = (struct relocation_info *) alloca (entry->header.a_drsize);
        lseek (desc, text_offset (entry) + entry->header.a_text
	       + entry->header.a_data + entry->header.a_trsize,
	       0);
        if (entry->header.a_drsize != read (desc, reloc, entry->header.a_drsize))
	  fatal_with_file ("premature eof in data relocation of ", entry, 0);
      }
    }

  lseek (desc, text_offset (entry) + entry->header.a_text, 0);
  if (entry->header.a_data != read (desc, bytes, entry->header.a_data))
    fatal_with_file ("premature eof in data section of ", entry, 0);

  perform_relocation (bytes, entry->data_start_address - entry->header.a_text,
		      entry->header.a_data, reloc, entry->header.a_drsize, entry);

  mywrite (bytes, 1, entry->header.a_data, outdesc);

  cur_base_register += entry->header.a_data;
}

/* Relocate ENTRY's text or data section contents.
   DATA is the address of the contents, in core.
   DATA_SIZE is the length of the contents.
   PC_RELOCATION is the difference between the address of the contents
     in the output file and its address in the input file.
   RELOC_INFO is the address of the relocation info, in core.
   RELOC_SIZE is its length in bytes.  */
/* This version is about to be severly hacked by Randy.  Hope it
   works afterwards. */

int dochecksign = 1;
#define	checksign(x)	{ if (dochecksign && ((x) & 0x8000)) (x) += 0x10000;}


void
perform_relocation (data, pc_relocation, data_size, reloc_info, reloc_size, entry)
     char *data;
     struct relocation_info *reloc_info;
     struct file_entry *entry;
     long pc_relocation;
     int data_size;
     int reloc_size;
{
  long prev_hi = 0;
  register struct relocation_info *p = reloc_info;
  struct relocation_info *end
    = reloc_info + reloc_size / sizeof (struct relocation_info);
  long text_relocation = entry->text_start_address;
  long data_relocation = entry->data_start_address - entry->header.a_text;
  long bss_relocation
    = entry->bss_start_address - entry->header.a_text - entry->header.a_data;

  if (logorrea) fprintf(stderr, "perform %s: %x [%x %x %x] [%x %x %x]\n",
  			entry->filename,
  			pc_relocation,
			entry->text_start_address, entry->data_start_address,
			entry->bss_start_address,
			text_relocation, data_relocation, bss_relocation);

  for (; p < end; p++)
    {
      register long relocation = 0;
      register long addr = RELOC_ADDRESS(p);
      register unsigned long mask = 0;

      if (logorrea) fprintf(stderr, "%s: reloc x%x %x %d %x",
      				entry->filename,
				p->r_address, p->r_index, p->r_extern, p->r_type);

      /* We expect addresses to be relative to the beginning of
         either the text or data segment */
      if (addr >= data_size)
	fatal_with_file ("relocation address out of range in ", entry, 0);

      if (RELOC_EXTERN_P(p))
	{
	  symbol *sp = reloc_to_sym(p, entry);

	  /* If the symbol is undefined, leave it at zero.  */
	  if (! sp->defined)
	    relocation = 0;
	  else
	    relocation = sp->value;
	}
      else switch (RELOC_TYPE(p))
	{
	case N_TEXT:
	case N_TEXT | N_EXT:
	  relocation = text_relocation;
	  break;

	case N_DATA:
	case N_DATA | N_EXT:
	  /* A word that points to beginning of the the data section
	     initially contains not 0 but rather the "address" of that section
	     in the input file, which is the length of the file's text.  */
	  relocation = data_relocation;
	  break;

	case N_BSS:
	case N_BSS | N_EXT:
	  /* Similarly, an input word pointing to the beginning of the bss
	     initially contains the length of text plus data of the file.  */
	  relocation = bss_relocation;
	  break;

	case N_ABS:
	case N_ABS | N_EXT:
	  /* Don't know why this code would occur, but apparently it does.  */
	  break;

	default:
	  fatal_with_file ("nonexternal relocation code x%x invalid in ",
	  		entry, RELOC_TYPE(p));
	}

/* check: gp > 16k & hi */
      if (RELOC_BRSET_P(p))
        {
	  /* AXIOM: GP == (Hi + addr) + lo  */
	  static long full_address;

if (logorrea) fprintf(stderr, "([%x %x %x %x]", relocation, 
		RELOC_MEMORY_BROFF(p, data+addr), cur_base_register,
		entry->base_register_value);

	  if (p->r_type == BRSET_16_31) {
	    /* find where we wanted it to point to, note
	       we must keep the offset of the "ldgp gp,offset(reg)" */
	    long off = 
	    	RELOC_MEMORY_BROFF(p, data+addr) + RELOC_MEMORY_BROFF(p+1, data+RELOC_ADDRESS(p+1))
		- entry->base_register_value;
	    
	    /* note that 'dot' was already factored into off (by as) */
	    relocation = cur_base_register - pc_relocation + off;
	    checksign(relocation);
	    full_address = relocation; /* save for next instr: */
	  } else {
	    relocation = full_address;
	  }
        }
      else if (RELOC_BRREL_P(p))
        {
/* check: TEXT */
	  /* relocate where we want it to point to */
	  relocation += entry->base_register_value + RELOC_MEMORY_BROFF(p,data+addr);
	  /* and put it in br-relative form */
	  relocation -= cur_base_register;
	      
	  relocation += RELOC_ADD_EXTRA(p);
	  if (p->r_type != BRELOC_0_31)
	    checksign(relocation);
	}
/* check more */
      else
        {
if (logorrea) fprintf(stderr, "(%x", relocation);
          relocation += RELOC_ADD_EXTRA(p);
	  if (p->r_type == RELOC_0_15 || p->r_type == RELOC_16_31)
	    checksign(relocation);
        }

if (logorrea) fprintf(stderr, ",%x)", relocation);

/* unchecked */
      if (relocatable_output)
	{
	  /* Non-PC relative relocations which are absolute
	     or which have become non-external now have fixed
	     relocations.  Set the ADD_EXTRA of this relocation
	     to be the relocation we have now determined.  */
	  if (! RELOC_PCREL_P (p))
	    {
	      if ((int)p->r_type <= RELOC_0_63
		  || RELOC_EXTERN_P (p) == 0)
		RELOC_ADD_EXTRA (p) = relocation;
	    }
	  /* External PC-relative relocations continue to move around;
	     update their relocations by the amount they have moved
	     so far.  */
	  else if (RELOC_EXTERN_P (p))
	    RELOC_ADD_EXTRA (p) -= pc_relocation;

          if (logorrea) fprintf(stderr, " now adds x%x\n", RELOC_ADD_EXTRA(p));

	  continue;
	}

      if (RELOC_PCREL_P(p))
	relocation -= pc_relocation;

      if (logorrea) fprintf(stderr, " by x%x >> %d : %d << %d",
      			relocation, RELOC_VALUE_RIGHTSHIFT(p),
			RELOC_TARGET_BITSIZE(p), RELOC_TARGET_BITPOS(p));

      relocation >>= RELOC_VALUE_RIGHTSHIFT(p);

      /* Unshifted mask for relocation */
      mask = 1 << RELOC_TARGET_BITSIZE(p) - 1;
      mask |= mask - 1;
      relocation &= mask;

      /* Shift everything up to where it's going to be used */
      relocation <<= RELOC_TARGET_BITPOS(p);
      mask <<= RELOC_TARGET_BITPOS(p);

      switch (RELOC_TARGET_SIZE(p))
	{
	case 0:
	  if (RELOC_MEMORY_ADD_P(p))
	    relocation += mask & *(char *) (data + addr);
	  *(char *) (data + addr) &= ~mask;
	  *(char *) (data + addr) |= relocation & mask;
	  break;

	case 1:
	      if (RELOC_MEMORY_ADD_P(p))
		relocation += mask & *(short *) (data + addr);
	      *(short *) (data + addr) &= ~mask;
	      *(short *) (data + addr) |= relocation & mask;
	  break;

	case 2:
	      if (RELOC_MEMORY_ADD_P(p))
		relocation += mask & *(int *) (data + addr);
	      *(int *) (data + addr) &= ~mask;
	      *(int *) (data + addr) |= relocation & mask;

	  break;

	case 3:
	      if (RELOC_MEMORY_ADD_P(p))
		relocation += mask & *(long *) (data + addr);
	      *(long *) (data + addr) &= ~mask;
	      *(long *) (data + addr) |= relocation & mask;

	  break;

	case -1:
              if (logorrea) fprintf(stderr, "\n");
	      continue;	/* NO_RELOC, ABS */

	default:
	  fatal_with_file ("Unimplemented relocation field length in ", entry, 0);
	}
        if (logorrea) fprintf(stderr, " gets x%x\n", relocation & mask);
    }
}

/* For relocatable_output only: write out the relocation,
   relocating the addresses-to-be-relocated.  */

void coptxtrel (), copdatrel ();

void
write_rel ()
{
  register int i;
  register int count = 0;

  if (trace_files)
    fprintf (stderr, "Writing text relocation:\n\n");

  /* Assign each global symbol a sequence number, giving the order
     in which `write_syms' will write it.
     This is so we can store the proper symbolnum fields
     in relocation entries we write.  */

  for (i = 0; i < TABSIZE; i++)
    {
      symbol *sp;
      for (sp = symtab[i]; sp; sp = sp->link)
	if (sp->referenced || sp->defined)
	  {
	    sp->def_count = count++;
	    /* Leave room for the reference required by N_INDR, if
	       necessary.  */
	    if ((sp->defined & ~N_EXT) == N_INDR)
	      count++;
	  }
    }
  /* Correct, because if (relocatable_output), we will also be writing
     whatever indirect blocks we have.  */
  if (count != defined_global_sym_count
      + undefined_global_sym_count + global_indirect_count)
    fatal ("internal error");

  /* Write out the relocations of all files, remembered from copy_text.  */

  each_full_file (coptxtrel, 0);

  if (trace_files)
    fprintf (stderr, "\nWriting data relocation:\n\n");

  each_full_file (copdatrel, 0);

  if (trace_files)
    fprintf (stderr, "\n");
}

void
coptxtrel (entry)
     struct file_entry *entry;
{
  register struct relocation_info *p, *end;
  register long reloc = entry->text_start_address;

  p = entry->textrel;
  end = (struct relocation_info *) (entry->header.a_trsize + (char *) p);
  while (p < end)
    {
      RELOC_ADDRESS(p) += reloc;
      if (RELOC_EXTERN_P(p))
	{
	  register symbol *sp = reloc_to_sym(p, entry);

	  /* If the symbol is now defined, change the external relocation
	     to an internal one.  */

	  if (sp->defined)
	    {
	      RELOC_EXTERN_P(p) = 0;
	      RELOC_SYMBOL(p) = (sp->defined & N_TYPE);
	      /* If we aren't going to be adding in the value in
	         memory on the next pass of the loader, then we need
		 to add it in from the relocation entry.  Otherwise
	         the work we did in this pass is lost.  */
	      if (!RELOC_MEMORY_ADD_P(p))
		RELOC_ADD_EXTRA (p) += sp->value;
	    }
	  else
	    /* Debugger symbols come first, so have to start this
	       after them.  */
	      RELOC_SYMBOL(p) = (sp->def_count + nsyms
				 - defined_global_sym_count
				 - undefined_global_sym_count
				 - global_indirect_count);
	}
      p++;
    }
  mywrite (entry->textrel, 1, entry->header.a_trsize, outdesc);
}

void
copdatrel (entry)
     struct file_entry *entry;
{
  register struct relocation_info *p, *end;
  /* Relocate the address of the relocation.
     Old address is relative to start of the input file's data section.
     New address is relative to start of the output file's data section.  */
  register int reloc = entry->data_start_address - text_size;

  p = entry->datarel;
  end = (struct relocation_info *) (entry->header.a_drsize + (char *) p);
  while (p < end)
    {
      RELOC_ADDRESS(p) += reloc;
      if (RELOC_EXTERN_P(p))
	{
	  symbol *sp = reloc_to_sym(p, entry);
	  int symtype;

	  symtype = sp->defined & N_TYPE;

	  if (force_common_definition
	      || symtype == N_DATA || symtype == N_TEXT || symtype == N_ABS)
	    {
	      RELOC_EXTERN_P(p) = 0;
	      RELOC_SYMBOL(p) = symtype;
	    }
	  else
	    /* Debugger symbols come first, so have to start this
	       after them.  */
	    RELOC_SYMBOL(p)
	      = sp->def_count
		 + nsyms - defined_global_sym_count
		 - undefined_global_sym_count
		 - global_indirect_count;
	}
      p++;
    }
  mywrite (entry->datarel, 1, entry->header.a_drsize, outdesc);
}

void write_file_syms ();
void write_string_table ();

/* Offsets and current lengths of symbol and string tables in output file. */

int symbol_table_offset;
int symbol_table_len;

/* Address in output file where string table starts.  */
int string_table_offset;

/* Offset within string table
   where the strings in `strtab_vector' should be written.  */
int string_table_len;

/* Total size of string table strings allocated so far,
   including strings in `strtab_vector'.  */
int strtab_size;

/* Vector whose elements are strings to be added to the string table.  */
char **strtab_vector;

/* Vector whose elements are the lengths of those strings.  */
int *strtab_lens;

/* Index in `strtab_vector' at which the next string will be stored.  */
int strtab_index;

/* Add the string NAME to the output file string table.
   Record it in `strtab_vector' to be output later.
   Return the index within the string table that this string will have.  */

int
assign_string_table_index (name)
     char *name;
{
  register int index = strtab_size;
  register int len = strlen (name) + 1;

  strtab_size += len;
  strtab_vector[strtab_index] = name;
  strtab_lens[strtab_index++] = len;

  return index;
}

FILE *outstream = (FILE *) 0;

/* Write the contents of `strtab_vector' into the string table.
   This is done once for each file's local&debugger symbols
   and once for the global symbols.  */

void
write_string_table ()
{
  register int i;

  lseek (outdesc, string_table_offset + string_table_len, 0);

  if (!outstream)
    outstream = fdopen (outdesc, "w");

  for (i = 0; i < strtab_index; i++)
    {
      fwrite (strtab_vector[i], 1, strtab_lens[i], outstream);
      string_table_len += strtab_lens[i];
    }

  fflush (outstream);

  /* Report I/O error such as disk full.  */
  if (ferror (outstream))
    perror_name (output_filename);
}

/* Write the symbol table and string table of the output file.  */

void
write_syms ()
{
  /* Number of symbols written so far.  */
  int syms_written = 0;
  register int i;
  register symbol *sp;

  /* Buffer big enough for all the global symbols.  One
     extra struct for each indirect symbol to hold the extra reference
     following. */
  struct nlist *buf
    = (struct nlist *) alloca ((defined_global_sym_count
				+ undefined_global_sym_count
				+ global_indirect_count)
			       * sizeof (struct nlist));
  /* Pointer for storing into BUF.  */
  register struct nlist *bufp = buf;

  /* Size of string table includes the bytes that store the size.  */
  strtab_size = sizeof strtab_size;

  symbol_table_offset = BSD_N_SYMOFF (&outheader);
  symbol_table_len = 0;
  string_table_offset = BSD_N_STROFF (&outheader);
  string_table_len = strtab_size;

  if (strip_symbols == STRIP_ALL)
    return;

  /* Write the local symbols defined by the various files.  */

  each_file (write_file_syms, &syms_written);
  file_close ();

  /* Now write out the global symbols.  */

  /* Allocate two vectors that record the data to generate the string
     table from the global symbols written so far.  This must include
     extra space for the references following indirect outputs. */

  strtab_vector = (char **) alloca ((num_hash_tab_syms
				     + global_indirect_count) * sizeof (char *));
  strtab_lens = (int *) alloca ((num_hash_tab_syms
				 + global_indirect_count) * sizeof (int));
  strtab_index = 0;

  /* Scan the symbol hash table, bucket by bucket.  */

  for (i = 0; i < TABSIZE; i++)
    for (sp = symtab[i]; sp; sp = sp->link)
      {
	struct nlist nl;

	nl.n_other = 0;
	nl.n_desc = 0;

	/* Compute a `struct nlist' for the symbol.  */

	if (sp->defined || sp->referenced)
	  {
	    /* common condition needs to be before undefined condition */
	    /* because unallocated commons are set undefined in */
	    /* digest_symbols */
	    if (sp->defined > 1) /* defined with known type */
	      {
		/* If the target of an indirect symbol has been
		   defined and we are outputting an executable,
		   resolve the indirection; it's no longer needed */
		if (!relocatable_output
		    && ((sp->defined & N_TYPE) == N_INDR)
		    && (((symbol *) sp->value)->defined > 1))
		  {
		    symbol *newsp = (symbol *) sp->value;
		    nl.n_type = newsp->defined;
		    nl.n_value = newsp->value;
		  }
		else
		  {
		    nl.n_type = sp->defined;
		    if (sp->defined != (N_INDR | N_EXT))
		      nl.n_value = sp->value;
		    else
		      nl.n_value = 0;
		  }
	      }
	    else if (sp->max_common_size) /* defined as common but not allocated. */
	      {
		/* happens only with -r and not -d */
		/* write out a common definition */
		nl.n_type = N_UNDF | N_EXT;
		nl.n_value = sp->max_common_size;
	      }
	    else if (!sp->defined)	      /* undefined -- legit only if -r */
	      {
		nl.n_type = N_UNDF | N_EXT;
		nl.n_value = 0;
	      }
	    else
	      fatal ("internal error: %s defined in mysterious way", sp->name);

	    /* Allocate string table space for the symbol name.  */

	    nl.n_un.n_strx = assign_string_table_index (sp->name);

	    /* Output to the buffer and count it.  */

	    *bufp++ = nl;
	    syms_written++;
	    if (nl.n_type == (N_INDR | N_EXT))
	      {
		struct nlist xtra_ref;
		xtra_ref.n_type = N_EXT | N_UNDF;
		xtra_ref.n_un.n_strx
		  = assign_string_table_index (((symbol *) sp->value)->name);
		xtra_ref.n_other = 0;
		xtra_ref.n_desc = 0;
		xtra_ref.n_value = 0;
		*bufp++ = xtra_ref;
		syms_written++;
	      }
	  }
      }

  /* Output the buffer full of `struct nlist's.  */

  lseek (outdesc, symbol_table_offset + symbol_table_len, 0);
  mywrite (buf, sizeof (struct nlist), bufp - buf, outdesc);
  symbol_table_len += sizeof (struct nlist) * (bufp - buf);

  if (syms_written != nsyms)
    fatal ("internal error: wrong number of symbols written into output file", 0);

  if (symbol_table_offset + symbol_table_len != string_table_offset)
    fatal ("internal error: inconsistent symbol table length", 0);

  /* Now the total string table size is known, so write it.
     We are already positioned at the right place in the file.  */

  mywrite (&strtab_size, sizeof (int), 1, outdesc);  /* we're at right place */

  /* Write the strings for the global symbols.  */

  write_string_table ();
}

/* Write the local and debugger symbols of file ENTRY.
   Increment *SYMS_WRITTEN_ADDR for each symbol that is written.  */

/* Note that we do not combine identical names of local symbols.
   dbx or gdb would be confused if we did that.  */

void
write_file_syms (entry, syms_written_addr)
     struct file_entry *entry;
     int *syms_written_addr;
{
  register struct nlist *p = entry->symbols;
  register struct nlist *end = p + entry->n_symbols;

  /* Upper bound on number of syms to be written here.  */
  int max_syms = entry->n_symbols + 1;

  /* Buffer to accumulate all the syms before writing them.
     It has one extra slot for the local symbol we generate here.  */
  struct nlist *buf
    = (struct nlist *) alloca ( max_syms * sizeof (struct nlist));
  register struct nlist *bufp = buf;


  /* Make tables that record, for each symbol, its name and its name's length.
     The elements are filled in by `assign_string_table_index'.  */

  strtab_vector = (char **) alloca (max_syms * sizeof (char *));
  strtab_lens = (int *) alloca (max_syms * sizeof (int));
  strtab_index = 0;

  /* Generate a local symbol for the start of this file's text.  */

  if (discard_locals != DISCARD_ALL)
    {
      struct nlist nl;

      nl.n_type = N_FN | N_EXT;
      nl.n_un.n_strx = assign_string_table_index (entry->local_sym_name);
      nl.n_value = entry->text_start_address;
      nl.n_desc = 0;
      nl.n_other = 0;
      *bufp++ = nl;
      (*syms_written_addr)++;
      entry->local_syms_offset = *syms_written_addr * sizeof (struct nlist);
    }

  /* Read the file's string table.  */

  if (entry->strings == 0) {
      entry->strings = (char *) alloca (entry->string_size);
      read_entry_strings (file_open (entry), entry);
  }

  for (; p < end; p++)
    {
      register int type = p->n_type;
      register int write = 0;

      /* WRITE gets 1 for a non-global symbol that should be written.  */


      if (SET_ELEMENT_P (type))	/* This occurs even if global.  These */
				/* types of symbols are never written */
				/* globally, though they are stored */
				/* globally.  */
        write = relocatable_output;
      else if (!(type & (N_STAB | N_EXT)))
        /* ordinary local symbol */
	write = ((discard_locals != DISCARD_ALL)
		 && !(discard_locals == DISCARD_L &&
		      (p->n_un.n_strx + entry->strings)[0] == LPREFIX)
		 && type != N_WARNING);
      else if (!(type & N_EXT))
	/* debugger symbol */
        write = (strip_symbols == STRIP_NONE);

      if (write)
	{
	  /* If this symbol has a name,
	     allocate space for it in the output string table.  */

	  if (p->n_un.n_strx)
	    p->n_un.n_strx = assign_string_table_index (p->n_un.n_strx
							+ entry->strings);

	  /* Output this symbol to the buffer and count it.  */

	  *bufp++ = *p;
	  (*syms_written_addr)++;
	}
    }

  /* All the symbols are now in BUF; write them.  */

  lseek (outdesc, symbol_table_offset + symbol_table_len, 0);
  mywrite (buf, sizeof (struct nlist), bufp - buf, outdesc);
  symbol_table_len += sizeof (struct nlist) * (bufp - buf);

  /* Write the string-table data for the symbols just written,
     using the data in vectors `strtab_vector' and `strtab_lens'.  */

  write_string_table ();
  entry->strings = 0;		/* Since it will dissapear anyway.  */
}

/* Copy any GDB symbol segments from the input files to the output file.
   The contents of the symbol segment is copied without change
   except that we store some information into the beginning of it.  */

void write_file_symseg ();

void
write_symsegs ()
{
  each_file (write_file_symseg, 0);
}

void
write_file_symseg (entry)
     struct file_entry *entry;
{
  char buffer[4096];
  struct symbol_root root;
  int indesc;
  int len;

  if (entry->symseg_offset == 0)
    return;

  /* This entry has a symbol segment.  Read the root of the segment.  */

  indesc = file_open (entry);
  lseek (indesc, entry->symseg_offset + entry->starting_offset, 0);
  if (sizeof root != read (indesc, &root, sizeof root))
    fatal_with_file ("premature end of file in symbol segment of ", entry, 0);

  /* Store some relocation info into the root.  */

  root.ldsymoff = entry->local_syms_offset;
  root.textrel = entry->text_start_address;
  root.datarel = entry->data_start_address - entry->header.a_text;
  root.bssrel = entry->bss_start_address
    - entry->header.a_text - entry->header.a_data;
  root.databeg = entry->data_start_address - root.datarel;
  root.bssbeg = entry->bss_start_address - root.bssrel;

  /* Write the modified root into the output file.  */

  mywrite (&root, sizeof root, 1, outdesc);

  /* Copy the rest of the symbol segment unchanged.  */

  if (entry->superfile)
    {
      /* Library member: number of bytes to copy is determined
	 from the member's total size.  */

      int total = entry->total_size - entry->symseg_offset - sizeof root;

      while (total > 0)
	{
	  len = read (indesc, buffer, min (sizeof buffer, total));

	  if (len != min (sizeof buffer, total))
	    fatal_with_file ("premature end of file in symbol segment of ", entry, 0);
	  total -= len;
	  mywrite (buffer, len, 1, outdesc);
	}
    }
  else
    {
      /* A separate file: copy until end of file.  */

      while (len = read (indesc, buffer, sizeof buffer))
	{
	  mywrite (buffer, len, 1, outdesc);
	  if (len < sizeof buffer)
	    break;
	}
    }

  file_close ();
}

/* Create the symbol table entries for `etext', `edata' and `end'.  */

void
symtab_init ()
{
#if ( !defined(nounderscore) || __osf__ )
  edata_symbol = getsym ("_edata");
  etext_symbol = getsym ("_etext");
  end_symbol = getsym ("_end");
#else
  edata_symbol = getsym ("edata");
  etext_symbol = getsym ("etext");
  end_symbol = getsym ("end");
#endif

  edata_symbol->defined = N_DATA | N_EXT;
  etext_symbol->defined = N_TEXT | N_EXT;
  end_symbol->defined = N_BSS | N_EXT;

  edata_symbol->referenced = 1;
  etext_symbol->referenced = 1;
  end_symbol->referenced = 1;
}

/* Compute the hash code for symbol name KEY.  */

int
hash_string (key)
     char *key;
{
  register char *cp;
  register int k;

  cp = key;
  k = 0;
  while (*cp)
    k = (((k << 1) + (k >> 14)) ^ (*cp++)) & 0x3fff;

  return k;
}

/* Get the symbol table entry for the global symbol named KEY.
   Create one if there is none.  */

symbol *
getsym (key)
     char *key;
{
  register int hashval;
  register symbol *bp;

  /* Determine the proper bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */

  for (bp = symtab[hashval]; bp; bp = bp->link)
    if (! strcmp (key, bp->name))
      return bp;

  /* Nothing was found; create a new symbol table entry.  */

  bp = (symbol *) xmalloc (sizeof (symbol));
  bp->refs = 0;
  bp->name = xmalloc (strlen (key) + 1);
  strcpy (bp->name, key);
  bp->defined = 0;
  bp->referenced = 0;
  bp->trace = 0;
  bp->value = 0;
  bp->max_common_size = 0;
  bp->warning = 0;
  bp->undef_refs = 0;
  bp->multiply_defined = 0;

  /* Add the entry to the bucket.  */

  bp->link = symtab[hashval];
  symtab[hashval] = bp;

  ++num_hash_tab_syms;

  return bp;
}

/* Like `getsym' but return 0 if the symbol is not already known.  */

symbol *
getsym_soft (key)
     char *key;
{
  register int hashval;
  register symbol *bp;

  /* Determine which bucket.  */

  hashval = hash_string (key) % TABSIZE;

  /* Search the bucket.  */

  for (bp = symtab[hashval]; bp; bp = bp->link)
    if (! strcmp (key, bp->name))
      return bp;

  return 0;
}

symbol *reloc_to_sym(p, entry)
  struct relocation_info *p;
  struct file_entry *entry;
{
  register int symindex = RELOC_SYMBOL (p);
  symbol *sp;

  if (symindex >= entry->n_symbols)
    fatal_with_file ("relocation symbolnum out of range in ", entry, 0);
  sp = (symbol *) (entry->symbols + symindex)->n_un.n_name;

#ifdef N_INDR
  /* Resolve indirection.  */
  if ((sp->defined & ~N_EXT) == N_INDR)
    sp = (symbol *) sp->value;
#endif
  return sp;
}

/* Report a fatal error.
   STRING is a printf format string and ARG is one arg for it.  */

void
fatal (string, arg)
     char *string, *arg;
{
  fprintf (stderr, "ld: ");
  fprintf (stderr, string, arg);
  fprintf (stderr, "\n");
  exit (1);
}

/* Report a fatal error.  The error message is STRING
   followed by the filename of ENTRY.  */

void
fatal_with_file (string, entry, arg0)
     char *string;
     struct file_entry *entry;
     char *arg0;
{
  fprintf (stderr, "ld: ");
  fprintf (stderr, string, arg0);
  print_file_name (entry, stderr);
  fprintf (stderr, "\n");
  exit (1);
}

/* Report a fatal error using the message for the last failed system call,
   followed by the string NAME.  */

void
perror_name (name)
     char *name;
{
  extern int errno, sys_nerr;
  extern char *sys_errlist[];
  char *s;

  if (errno < sys_nerr)
    s = concat ("", sys_errlist[errno], " for %s");
  else
    s = "cannot open %s";
  fatal (s, name);
}

/* Report a fatal error using the message for the last failed system call,
   followed by the name of file ENTRY.  */

void
perror_file (entry)
     struct file_entry *entry;
{
  extern int errno, sys_nerr;
  extern char *sys_errlist[];
  char *s;

  if (errno < sys_nerr)
    s = concat ("", sys_errlist[errno], " for ");
  else
    s = "cannot open ";
  fatal_with_file (s, entry, 0);
}

/* Report a nonfatal error.
   STRING is a format for printf, and ARG1 ... ARG3 are args for it.  */

void
error (string, arg1, arg2, arg3)
     char *string, *arg1, *arg2, *arg3;
{
  fprintf (stderr, "%s: ", progname);
  fprintf (stderr, string, arg1, arg2, arg3);
  fprintf (stderr, "\n");
}


/* Output COUNT*ELTSIZE bytes of data at BUF
   to the descriptor DESC.  */

void
mywrite (buf, count, eltsize, desc)
     char *buf;
     int count;
     int eltsize;
     int desc;
{
  register int val;
  register int bytes = count * eltsize;

  while (bytes > 0)
    {
      val = write (desc, buf, bytes);
      if (val <= 0)
	perror_name (output_filename);
      buf += val;
      bytes -= val;
    }
}

/* Output PADDING zero-bytes to descriptor OUTDESC.
   PADDING may be negative; in that case, do nothing.  */

void
padfile (padding, outdesc)
     int padding;
     int outdesc;
{
  register char *buf;
  if (padding <= 0)
    return;

  buf = (char *) alloca (padding);
  bzero (buf, padding);
  mywrite (buf, padding, 1, outdesc);
}

/* Return a newly-allocated string
   whose contents concatenate the strings S1, S2, S3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  register int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  register char *result = xmalloc (len1 + len2 + len3 + 1);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  result[len1 + len2 + len3] = 0;

  return result;
}

/* Parse the string ARG using scanf format FORMAT, and return the result.
   If it does not parse, report fatal error
   generating the error message using format string ERROR and ARG as arg.  */

long
parse (arg, format, error)
     char *arg, *format, *error;
{
  long x;
  if (1 != sscanf (arg, format, &x))
    fatal (error, arg);
  return x;
}

/* Like malloc but get fatal error if memory is exhausted.  */

char *xmalloc (size)
     int size;
{
  register char *result;

  if (size <= 0) return 0;
  result = malloc (size);
  if (result == 0)
    fatal ("virtual memory exhausted", 0);
  return result;
}

/* Like realloc but get fatal error if memory is exhausted.  */

char *xrealloc (ptr, size)
     char *ptr;
     int size;
{
  register char *result = realloc (ptr, size);
  if (result == 0)
    fatal ("virtual memory exhausted", 0);
  return result;
}

/*
 *	COFF support junkies
 */

#if	defined(alpha) || defined(mips)

#define	n_symbol_index_was	n_pad

#include "mips_reloc.h"

/* Build an a.out header from the info in a coff header.
   Does the minimum necessary, assuming e.g. that the ranlib
   info might make it unnecessary to look deeper into
   this file */

xlate_coff_header( desc, entry, ex )
  int desc;
  register struct file_entry *entry;
  struct exechdr *ex;
{
  struct scnhdr	*scnp;
  int i, ntrel, ndrel;
  int rsiz = 0;
  int rdata_contig_text;

  /* Easy thing first */
  entry->coff_nscns = ex->f.f_nscns;
  entry->base_register_value = ex->a.gp_value;
  entry->header.a_magic = ex->a.magic;
#if 0
  /* This would be nice, but we want to make the .rdata readonly */
  entry->header.a_text = ex->a.tsize;
  entry->header.a_data = ex->a.dsize;
#endif
  entry->header.a_bss = ex->a.bsize;
  entry->header.a_entry = ex->a.entry;
  entry->header.a_tstart = ex->a.text_start;
  entry->header.a_dstart = ex->a.data_start;

  ntrel = ex->f.f_nscns * sizeof(*scnp);
  scnp = (struct scnhdr *)alloca(ntrel);
  lseek (desc, entry->starting_offset + sizeof(*ex), 0);
  if (read(desc, scnp, ntrel) != ntrel)
    fatal_with_file ("failure reading section headers of ", entry, 0);

  ntrel = ndrel = 0;
  rdata_contig_text = 1;	/* rdata first is ok too */
  for (i = 0; i < ex->f.f_nscns; i++) {
    if (/* Hope this works */
	(strncmp(scnp[i].s_name, ".xdata", 7) == 0) ||
	(strncmp(scnp[i].s_name, ".pdata", 7) == 0))
	continue;
    if (strncmp(scnp[i].s_name, ".text", 6) == 0) {
      ntrel += scnp[i].s_nreloc;
      rdata_contig_text = 1;
    } else if (strncmp(scnp[i].s_name, ".rdata", 7) == 0) {
      if (!rdata_contig_text)
        fatal_with_file("Sorry, cannot have .rdata section separate from .text section, in ",
			entry, 0);
      ntrel += scnp[i].s_nreloc;
      rsiz = scnp[i].s_size;
    } else {
      ndrel += scnp[i].s_nreloc;
      rdata_contig_text = 0;
    }
  }
  entry->header.a_trsize = ntrel * sizeof(struct relocation_info);
  entry->header.a_drsize = ndrel * sizeof(struct relocation_info);
if (logorrea) fprintf(stderr,"%s: %d trel %d drel\n", entry->filename, ntrel, ndrel);

  entry->header.a_text = ex->a.tsize + rsiz;
  entry->header.a_data = ex->a.dsize - rsiz;
  entry->header.a_dstart += rsiz;
if (logorrea) fprintf(stderr,"%s: x%x text x%x data.\n",
		      entry->filename, entry->header.a_text, entry->header.a_data);

  /* Only needs to know how many. Later. */
  entry->header.a_syms = 0;

}

enum reloc_type
coff_map_rtype(mipstype, external)
    int mipstype, external;
{
    switch (mipstype) {

    case R_ABS:
fprintf(stderr,"map_rtype: ABS\n");
        return RELOC_0_15;    /* mumble.. is it the lda of a ldgp. but. */
        break;
    case R_REFLONG:
        return RELOC_0_31;
        break;
    case R_REFQUAD:
        return RELOC_0_63;
        break;
    case R_GPREL32:
        return BRELOC_0_31;
        break;
    case R_LITERAL:
        return BRELOC_0_15;
        break;
    case R_BRADDR:
if (external)
{
static int didit = 0;
if (!didit++) fprintf(stderr,"{R_BRADDR}\n");
}
        return (external) ? RELOC_WDISP21 : NO_RELOC;
        break;
    case R_HINT:
/*        return RELOC_WDISP14;	but we donno */
	  return NO_RELOC;
        break;
    case R_LITUSE:	/* wont */
    case R_GPDISP:
    default:
        fatal("Embarassed by mipsrel type x%x", mipstype);
    }
}

/* Dive into a coff object, translate all info it contains
   about relocations to be performed into a.out format.
   Assumes symbols have been already read and xlated
   (if not, we do it here) */

xlate_coff_relocs(desc, entry)
  int desc;
  register struct file_entry *entry;
{
    struct scnhdr   *scnp;
    struct relocation_info *textrel, *datarel, *relocs;
    int             i, j, ntrel, ndrel;
    long	    textscn_base, datascn_base, rdata_base;
    struct nlist   *hint;
    long scn_vaddrs[MAX_R_SN+1];

    if (entry->symbols == 0)
        xlate_coff_syms(desc, entry);
    hint = entry->symbols;

    textrel = datarel = 0;
    if (entry->header.a_trsize)
        textrel = (struct relocation_info *) xmalloc(entry->header.a_trsize);
    if (entry->header.a_drsize)
        datarel = (struct relocation_info *) xmalloc(entry->header.a_drsize);

    /* Read section headers, fill in table of vaddrs for all sections */
    i = entry->coff_nscns * sizeof(*scnp);
    scnp = (struct scnhdr *)alloca( i );
    lseek(desc, entry->starting_offset + sizeof(struct exechdr), 0);
    if (read(desc, scnp, i) != i)
        fatal_with_file("failure reading section headers of ", entry, 0);

    bzero(scn_vaddrs, sizeof(scn_vaddrs));
#define	CHECKSCN(n,x) \
        if (strncmp(scnp[i].s_name, n, sizeof(n)) == 0) { \
	    scn_vaddrs[x] = scnp[i].s_vaddr; \
	    continue; \
	}

    for (i = 0; i < entry->coff_nscns; i++) {
        CHECKSCN(".text",R_SN_TEXT)
        CHECKSCN(".rdata",R_SN_RDATA)
        CHECKSCN(".data",R_SN_DATA)
        CHECKSCN(".sdata",R_SN_SDATA)
        CHECKSCN(".sbss",R_SN_SBSS)
        CHECKSCN(".bss",R_SN_BSS)
        CHECKSCN(".init",R_SN_INIT)
        CHECKSCN(".lit8",R_SN_LIT8)
        CHECKSCN(".lit4",R_SN_LIT4)
        CHECKSCN(".xdata",R_SN_XDATA)
        CHECKSCN(".pdata",R_SN_PDATA)
        CHECKSCN(".fini",R_SN_FINI)
        CHECKSCN(".lita",R_SN_LITA)
/*      CHECKSCN(".???",R_SN_ABS)   */
    }

    ntrel = ndrel = 0;
    textscn_base = datascn_base = rdata_base = 0;

    for (i = 0; i < entry->coff_nscns; i++) {

        RELOC          *mipsreloc;
	long		scn_base;

        /* text or data ? and which virtual base */
        if (strncmp(scnp[i].s_name, ".text", 6) == 0) {
            relocs = textrel + ntrel;
            ntrel += scnp[i].s_nreloc;
	    scn_base = textscn_base;
	    textscn_base += scnp[i].s_size;
	} else if (strncmp(scnp[i].s_name, ".rdata", 7) == 0) {
            relocs = textrel + ntrel;
            ntrel += scnp[i].s_nreloc;
	    rdata_base = scn_base = textscn_base;
	    textscn_base += scnp[i].s_size;
	} else if ((strncmp(scnp[i].s_name, ".xdata", 7) == 0) ||
		   (strncmp(scnp[i].s_name, ".pdata", 7) == 0)) {
	    /* can we just ignore this ? hope so */
	    datascn_base += scnp[i].s_size;
	    continue;
        } else {
            relocs = datarel + ndrel;
            ndrel += scnp[i].s_nreloc;
	    scn_base = datascn_base;
	    datascn_base += scnp[i].s_size;
        }

if (logorrea > 1) fprintf(stderr,"%s: Section %s: %x @%x\n", entry->filename,
			scnp[i].s_name, scnp[i].s_nreloc, relocs);

	/* If nothing to do then dont */
        if (scnp[i].s_nreloc == 0)
            continue;

        j = scnp[i].s_nreloc * RELSZ;
        mipsreloc = (RELOC *) xmalloc(j);

        lseek(desc, entry->starting_offset + scnp[i].s_relptr, 0);
        if (read(desc, mipsreloc, j) != j)
            fatal_with_file("failure reading relocations of %d-th section of ", entry, i);
        for (j = 0; j < scnp[i].s_nreloc; j++) {

            int ix, typ;

if (logorrea > 1) fprintf(stderr, "%x: %x %d %x %x\n", j,
			mipsreloc[j].r_vaddr, mipsreloc[j].r_extern,
			mipsreloc[j].r_symndx, mipsreloc[j].r_type);

            /* copy mipsreloc[j] to relocs[j]; */

	    /* Perform_relocation() expects addresses to be
	      zero-based for both text and data section. */
            relocs[j].r_address = mipsreloc[j].r_vaddr - scnp[i].s_vaddr
	    			  + scn_base;

            relocs[j].r_addend = 0;

            ix  = mipsreloc[j].r_symndx;
            typ = mipsreloc[j].r_type;

	    /* External symbol ? */
            if ((relocs[j].r_extern = mipsreloc[j].r_extern) != 0) {

                register struct nlist *sp, *esp;

		/* Find its nlist description */
                sp = hint;
                esp = entry->symbols + entry->n_symbols;
                for (sp = hint; sp < esp; sp++)
                    if (sp->n_symbol_index_was == ix)
                        break;
                if (sp == esp) {
                    sp = entry->symbols;
                    for (esp = hint; sp < esp; sp++)
                        if (sp->n_symbol_index_was == ix)
                            break;
                }
                if (sp == esp) {
#if DEBUG
sp = entry->symbols; esp = sp + entry->n_symbols;
for (; sp < esp; sp++)
fprintf(stderr, " %x %x %x %x %x %x\n", 
sp->n_pad, sp->n_un.n_strx, sp->n_type, sp->n_other, sp->n_desc, sp->n_value);
#endif
                    fatal_with_file("Could not find reloc symindex x%x in ",
		    			entry, ix);
		}
                hint = sp;

                relocs[j].r_index = sp - entry->symbols;

                relocs[j].r_type = coff_map_rtype(mipsreloc[j].r_type, TRUE);

		/* This hack cuz a.out does it different */
		if (relocs[j].r_type == RELOC_WDISP21)
		    relocs[j].r_addend = 0 - (relocs[j].r_address + 4);


		continue;

            }

            /* Local, e.g. use place.  Funny things here */

            /* symndx == section number. Except for LITUSE */

            if (typ == R_LITUSE) {
                if (ix == R_LU_JSR) {
                    relocs[j].r_index = N_TEXT;
                    relocs[j].r_type  = RELOC_WDISP14;
		} else {
                    relocs[j].r_index = N_ABS;
                    relocs[j].r_type  = NO_RELOC;
		}
                continue;
            }

	    /* with GPDISP really need to work on the next one too */
	    if (typ == R_GPDISP) {
                relocs[j].r_index = N_DATA;
                relocs[j].r_type = BRSET_16_31;
		if ((j == scnp[i].s_nreloc - 1) ||
		    (mipsreloc[j+1].r_extern) ||
		    (mipsreloc[j+1].r_type != R_ABS) ||
		    (mipsreloc[j+1].r_symndx != R_SN_LITA))
{
fprintf(stderr," %d %d x%x x%x\n",
	j, mipsreloc[j+1].r_extern, mipsreloc[j+1].r_type,
	mipsreloc[j+1].r_symndx);
		    fatal("Guess I do not grok coff well enough");
}
		j++;
#if 0
                /* This was in the good days when things still made sense */
                relocs[j].r_address = mipsreloc[j].r_vaddr - scnp[i].s_vaddr
					+ scn_base;
#else
                if (mipsreloc[j].r_vaddr >= scnp[i].s_vaddr)
                   relocs[j].r_address = mipsreloc[j].r_vaddr - scnp[i].s_vaddr
					+ scn_base;
                else
                   relocs[j].r_address = mipsreloc[j].r_vaddr + scn_base;
#endif
                relocs[j].r_extern = 0;
                relocs[j].r_index = N_DATA;
                relocs[j].r_type = BRSET_0_15;
		continue;
	    }

	    /* ..like I said, symndx == section number.. */

#if 0
            relocs[j].r_addend = 0 - scn_vaddrs[ix];
#endif

            switch (ix) {
            case R_SN_RDATA:
#if 0
		/* we moved it at the end of the text section, soo..*/
		relocs[j].r_addend = scnp[R_SN_RDATA].s_vaddr;
		/* why not += ? think about it */
#endif
		/* fallthrough */
            case R_SN_TEXT:
            case R_SN_INIT:
            case R_SN_FINI:
                relocs[j].r_index = N_TEXT;
                break;
            case R_SN_DATA:
            case R_SN_SDATA:
            case R_SN_LIT8:
            case R_SN_LIT4:
                relocs[j].r_index = N_DATA;
                break;
            case R_SN_SBSS:
            case R_SN_BSS:
                relocs[j].r_index = N_BSS;
                break;

            case R_SN_LITA:
                relocs[j].r_index = N_DATA;
		break;

            case R_SN_PDATA:
            case R_SN_XDATA:
            case R_SN_NULL:
            default:
                fatal_with_file("Bad mipsreloc type x%x in ", entry, ix);
                break;
            }

            relocs[j].r_type = coff_map_rtype(typ, FALSE);

        }
        free(mipsreloc);
    }
    entry->textrel = textrel;
    entry->datarel = datarel;

if (logorrea > 2) {
    fprintf(stderr,"Text relocs are:\n");
    for (i = 0; i < ntrel; i++)
      fprintf(stderr,"%x: %x %x %d %x %x\n", i, 
      	textrel[i].r_address, textrel[i].r_addend, textrel[i].r_extern,
	textrel[i].r_index, textrel[i].r_type);    

    fprintf(stderr,"Data relocs are:\n");
    for (i = 0; i < ndrel; i++)
      fprintf(stderr,"%x: %x %x %d %x %x\n", i, 
      	datarel[i].r_address, datarel[i].r_addend, datarel[i].r_extern,
	datarel[i].r_index, datarel[i].r_type);    
}
}


static void fixup_symtab(HDRR *, char *, unsigned int);

/* Dive into the symbols of a coff file, mapping them
   to nlist entries. */

xlate_coff_syms(desc, entry)
  int desc;
  register struct file_entry *entry;
{
	char *ss;		/* start of symtab */
	HDRR	*si;		/* symtab header */
	struct exechdr hdr;
	register i, n, c;
	struct nlist *symp = NULL;
	off_t o;

	if (entry->symbols) return;	/* done already */

	/* Ohh well.. */
	lseek(desc, entry->starting_offset, 0);
	(void) read(desc, &hdr, sizeof(hdr));

	/* where is symtab, if any */
	if (hdr.f.f_symptr <= 0)
	        fatal_with_file ("No symbols in ", entry, 0);

	/* read symtab header in */
	if ((si = (HDRR*)malloc(n = hdr.f.f_nsyms)) == 0)
		fatal("out of memory");

	o = hdr.f.f_symptr - sizeof(hdr);
	lseek(desc, o, 1);
	if (read(desc, (char *)si, n) != n)
	        fatal_with_file ("Truncated file: ", entry, 0);

	/* read rest of symtab */
	n =  (si->cbExtOffset + si->iextMax * cbEXTR)
		- (hdr.f.f_symptr +  hdr.f.f_nsyms);
	if ((ss = (char*)malloc(n)) == 0)
		fatal("out of memory");
	if (read(desc, (char *)ss, n) != n)
	        fatal_with_file ("Truncated file: ", entry, 0);

	/* fix it up */
	fixup_symtab(si, ss, hdr.f.f_symptr + hdr.f.f_nsyms);

	if ((entry->strings = (char*)si->cbSsOffset) == 0) {
		entry->strings = (char *)-1;
		entry->string_size = 0;
	} else
		entry->string_size = n - (entry->strings - (char*)ss); /* overkill */

	entry->n_symbols = map_to_nlist(si, &entry->symbols, entry->strings);

}

/*
 * To make things quick, we would like to have the in-memory
 * symbol table hold real pointers and not file offsets.
 * But many fields are too narrow, so we must preserve them
 * as offsets and just use some access funcs/macros in the rest
 * of the code.
 * fixup_symtab() is the function that makes the following work,
 * do not forget to fix it if you change anything in this section.
 */
static void fixup_symtab(HDRR *, char *, unsigned int);

#define fh_iss_base(hdr,fh)	((hdr)->cbSsOffset + (fh)->issBase)
#define fh_isym_base(hdr,fh)	((hdr)->cbSymOffset + (fh)->isymBase)
#define fh_iopt_base(hdr,fh)	((hdr)->cbOptOffset + (fh)->ioptBase)
#define fh_ipd_first(hdr,fh)	((hdr)->cbPdOffset + (fh)->ipdFirst)
#define fh_iaux_base(hdr,fh)	((hdr)->cbAuxOffset + (fh)->iauxBase)
#define fh_rfd_base(hdr,fh)	((hdr)->cbRfdOffset + (fh)->rfdBase)
#define fh_cbline_offset(hdr,fh) ((fh)->cbLineOffset)
#define fh_rss(hdr,fh)		((hdr)->cbSsOffset + (fh)->rss)

#define pr_at_index(hdr,fh,i)	(PDR*)(fh_ipd_first(hdr,fh) + (i) * sizeof(PDR))
static SYMR *pr_isym(HDRR *, PDR *);
static char *pr_iss(HDRR *, PDR *);

#define sh_at_index(hdr,fh,i)	(SYMR*)(fh_isym_base(hdr,fh) + (i) * sizeof(SYMR))
#define sh_iss(hdr,sh)		(char*)((hdr)->cbSsOffset + (sh)->iss)
#define esh_at_index(hdr,i)	(EXTR*)((hdr)->cbExtOffset + (i) * sizeof(EXTR));
#define esh_iss(hdr,esh)	(char*)((hdr)->cbSsExtOffset + (esh)->asym.iss)


static SYMR*
pr_isym(symtab,pr)
	HDRR	*symtab;
	PDR	*pr;
{
	static SYMR	static_procedure =
		{ 0, 0, stStatic, scText, 0, 0};

	if (pr->reserved) {
		if (pr->isym == -1) {
			return &static_procedure;
		} else {
			return & ((EXTR*)(symtab->cbExtOffset + pr->isym))->asym;
		}
	} else {
		return (SYMR*) ((symtab)->cbSymOffset + pr->isym);
	}
}

static char *
pr_iss(symtab, pr)
	register HDRR	*symtab;
	register PDR	*pr;
{
	register char	*name;

	/* was the file stripped ? */
	if (pr->reserved) {
		/* so did we lose static infos ? */
		if (pr->isym == -1) {
			name = "<static procedure>";
		} else {
			register EXTR *esh;

			esh = (EXTR*)(symtab->cbExtOffset + pr->isym);
			name = esh_iss(symtab, esh);
		}
	} else {
		register SYMR *sh;

		sh = (SYMR*) ((symtab)->cbSymOffset + pr->isym);
		name = sh_iss(symtab, sh);
	}
	return name;
}

/*
 * Fixup the symbol table to hold in-memory pointers, as opposed
 * to file displacements.
 */
static void
fixup_symtab(symtab, data, f_ptr)
	HDRR		*symtab;
	char		*data;
	unsigned int	f_ptr;
{
	int             f_idx, s_idx;
	FDR            *fh;
	SYMR	       *sh;
	OPTR	       *op;
	PDR	       *pr;
	EXTR	       *esh;

#define FIX(off) \
	if (symtab->off) symtab->off = (vm_offset_t)data + (symtab->off - f_ptr);

	FIX(cbLineOffset);
	FIX(cbDnOffset);
	FIX(cbPdOffset);
	FIX(cbSymOffset);
	FIX(cbOptOffset);
	FIX(cbAuxOffset);
	FIX(cbSsOffset);
	FIX(cbSsExtOffset);
	FIX(cbFdOffset);
	FIX(cbRfdOffset);
	FIX(cbExtOffset);
#undef FIX
	/*
	 * Now go on and fixup all the indexes within the symtab itself.
	 * We must work on a file-basis.
	 */
	for (f_idx = 0; f_idx < symtab->ifdMax; f_idx++) {
		register int stripped;
		register vm_offset_t code_off;

		fh = (FDR *) (symtab->cbFdOffset + f_idx * sizeof(FDR));

		/*
		 * Fix file descriptor itself. See macros atop.
		 */
		fh->isymBase *= sizeof(SYMR);
		fh->ioptBase *= sizeof(OPTR);
		fh->ipdFirst *= sizeof(PDR);
		fh->iauxBase *= sizeof(AUXU);
		fh->rfdBase *= sizeof(FDR);    
		fh->cbLineOffset += symtab->cbLineOffset;	/* this one fits */

		/* name and address of the file */
		stripped = (fh->rss == -1);	/* lang==4 ? */
		if (!stripped)
			fh->rss += fh->issBase;
		code_off = fh->adr;

		/*
		 * Fixup local symbols. See macros atop.
		 */
		for (s_idx = 0; s_idx < fh->csym; s_idx++) {
			sh = sh_at_index(symtab,fh,s_idx);
			sh->iss += fh->issBase;
		}

		/*
		 * Fixup procedure symbols. See pr_xxx functions atop.
		 */
		for (s_idx = 0; s_idx < fh->cpd; s_idx++) {
			pr = pr_at_index(symtab,fh,s_idx);
			pr->reserved = stripped;
			if (stripped) {
				if (pr->isym != -1) {
					pr->isym *= sizeof(EXTR);
				}
			} else {
				pr->isym = fh->isymBase + pr->isym * sizeof(SYMR);
				if (s_idx == 0 && pr->adr != 0) {
					code_off -= pr->adr;
				}
				pr->adr += code_off;
			}

			/*
			 * Fix line numbers
			 */
			pr->cbLineOffset += fh->cbLineOffset;	/* this one fits */

		}
		/* forget about everything else */
	}

	/*
	 * External symbols. Nothing needed, see macros atop.
	 */
}


/*
 * map MIPSCo symtable codes into nlist codes
 */
static
map(class,type)
{
#	define IS_DATA(c,t)	((c==scData)||(c==scSData))
#	define IS_COMMON(c,t)	((c==scCommon)||(c==scSCommon))
#	define IS_BSS(c,t)	((c==scBss)||(c==scSBss))
#	define IS_TEXT(c,t)	((c==scText)||(c==scRData)||(t==stStaticProc))
#	define IS_LOCAL(c,t)	(t==stLocal)
#	define IS_PARAM(c,t)	(t==stParam)
#	define IS_ABS(c,t)	(c==scAbs)
#	define IS_FILE(c,t)	(t==stFile)
#	define IS_UNDEF(c,t)	((c==scUndefined)||(c==scSUndefined))

	if (IS_FILE(class,type))
		return N_FN;
	if (IS_TEXT(class,type))
		return N_TEXT;
	if (IS_DATA(class,type))
		return N_DATA;
	if (IS_BSS(class,type))
		return N_BSS;
	if (IS_ABS(class,type))
		return N_ABS;
	if ((IS_COMMON(class,type)) ||
	    (IS_UNDEF(class,type)))
		return N_UNDF;
fprintf(stderr, "??map(%d,%d)??\n", class, type);
	return N_UNDF;
}

map_to_nlist( symtab, syms, stringbase)
	HDRR	*symtab;
	struct nlist **syms;
	char	*stringbase;
{
	struct nlist	*symp = 0, *p;
	int		nsyms = 0;
	register int i;
	EXTR	*es;
	PDR	*pr;
	SYMR	*sp;

	/*
	 * Normally only go through external symbols.
	 * If all symbols requested, also do procedures
	 * and local symbols
	 */
	nsyms = symtab->iextMax;
	symp = (struct nlist *)malloc(nsyms * sizeof(struct nlist));
	p = symp;
	for (es = (EXTR*)symtab->cbExtOffset, i = 0;
	     i < symtab->iextMax; i++, es++) {

		/* Sometimes gets junk */
		if (es->asym.sc == scNil) {
			nsyms--;
			continue;
		}

		p->n_un.n_strx  = esh_iss(symtab,es) - stringbase;
		p->n_type  = map(es->asym.sc,es->asym.st);
		if (es->asym.st != stStaticProc)
			p->n_type |= N_EXT;
		p->n_other = 0;
		p->n_desc = 0;
		p->n_value = es->asym.value;
		p->n_symbol_index_was = i;
		p++;
	}

done:
	*syms = symp;
	return nsyms;
}
#endif	/* alpha || mips */

