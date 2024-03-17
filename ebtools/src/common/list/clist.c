/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
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
**	ED64 Software Tools - clist
** 
** FUNCTIONAL DESCRIPTION:
** 
**      Generate a list file from a coff image file.  This
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
**
**      Who	When		What
**	-----	-----------	---------------------------------------------
**	DAR	   Nov-1992	First version.
** $Log: clist.c,v $
** Revision 1.1.1.1  1998/12/29 21:36:23  paradis
** Initial CVS checkin
**
 * Revision 4.5  1997/02/21  20:54:40  fdh
 * Fixed symbol
 *
 * Revision 4.4  1997/02/21  20:50:29  fdh
 * Corrected seen indexing.
 * Report version for verbose mode.
 *
 * Revision 4.3  1995/11/14  19:34:06  cruz
 * Cleaned up code with lint.
 *
 * Revision 4.2  1994/08/06  00:00:23  fdh
 * Updated Copyright header
 *
 * Revision 4.1  1994/05/16  09:28:31  rusling
 * Do not use a large buffer and read the whole file
 * into it.  Now use fseek() to move around the file.
 *
*/
#ifndef lint
static char *rcsid = "$Id: clist.c,v 1.1.1.1 1998/12/29 21:36:23 paradis Exp $";
#endif

/*
typedef long vm_offset_t;
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "coff.h"
#include "syms.h"
#include "reloc.h"
/*
 *  Macros
 */
#define TRUE 1
#define FALSE 0
#define BLOCK_SIZE 1000

#define _debug(name,string)\
     {if (_SEEN('d')) \
	printf("debug: %s %s\n", name, string);	\
     }

#define _ddebug(name,string,number)\
     {if (_SEEN('d')) \
	printf("debug: %s %s %d\n", name, string,number);	\
     }

#define _error(string1,string2)\
     {	\
	printf("error: %s %s\n", string1, string2);	\
	exit(1);    \
     }

#define _SEEN(o) (seen[o-'a'])

void  FGET(FILE *file, long offset, void *buffer, int size) 
{ 
    if (fseek(file, offset, SEEK_SET) != 0) {
        fprintf(stderr, "ERROR: positioning file too %d\n", (offset)); 
        exit(0); 
    }    
    if (fread((char *)buffer, sizeof(char), size, file) == NULL) {    
	fprintf(stderr, "ERROR: reading file at position %d\n", 
		offset); 
	exit(0); 
    } 
}
/*
 *  Global data 
 */
#define SEEN_SIZE ('z' - 'a' + 10)
char seen[SEEN_SIZE];			/* upper and lower case */

char buffer1[255];
char name[255];
char buffer2[255];
/*
 *  Forward routine descriptions.
 */
main(int argc, char **argv);
void usage();
void analyse_coff_file(FILE *in);
void dump_symbol_table(FILE *in, pHDRR hdrr);
void dump_fds(FILE *in, int offset, unsigned int count, pHDRR hdrr);
void dump_relocation_info(FILE *in, int offset, unsigned int count);
void dump_external_symbols(FILE *in, int offset, pHDRR hdrr);
void dump_local_symbols(FILE *in, int offset, pHDRR hdrr);
void dump_data(FILE *in, unsigned int physical, int offset,
  unsigned int size, 
  pHDRR hdrr);
void dump_text(FILE *in, unsigned int physical, int offset,
  unsigned int size, 
  pHDRR hdrr);
void dump_entry_points(FILE *in, pHDRR syms);
void dump_pvc_map(FILE *in, pHDRR syms);
void print_symbols_at_address(FILE *in, unsigned int physical, pHDRR syms);
char *symbol_name_from_value(FILE *in, unsigned int physical, pHDRR syms);
unsigned int is_pvc_label(char *label);

extern void print_instruction(int instruction, FILE *stream);


main(int argc, char **argv)
{
    char *in_file_name = "a.out";	/* default .o file */
    FILE *in;
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
		_debug("main()", "option loop");
		switch (option) {
		    case 'v': 		/* verbose */
		    case 'V': 
			printf("clist - version (%s)\n", rcsid);

		    case 'e': 		/* entry points (for PVC) */
		    case 'E': 
		    case 'm': 		/* map (pvc) symbols (for PVC) */
		    case 'M': 
		    case 'f': 		/* full, don't skip zero locations */
		    case 'F': 
		    case 'b': 		/* just print out the bare bones */
		    case 'B': 
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
/*
 *  If verbose has been selected reveal the arguments.
 */
    if _SEEN('v') {
	char c;

	printf("arguments : ");
	for (c = 'a'; c < 'z'; c++)
	    if _SEEN(c)
		printf("%c", c);
	printf("\n");
    }
/*
 *  Try and open the input file.
 */
    in = fopen(in_file_name, "rb");
    if (in == NULL) {
	fprintf(stderr, "ERROR: unable to open file (%s)\n", 
	  in_file_name);
	exit(0);
    }
/*
 *  Go and analyse the file contents.
 */
    analyse_coff_file(in);
    return 0;
}					/* end of main() */

void usage()
{
    printf("clist - description\n");
    printf("\tDump the contents of a coff format object file\n");
    printf("clist - usage : alist -dDvVeEmMhH <filename>\n");
    printf("\tNOTE: Upper and Lower case options are treated equally\n");
    printf("\td,D = turn on debug mode\n");
    printf("\tv,V = turn on verbose mode\n");
    printf("\te,E = print entry points (for PVC)\n");
    printf("\tm,M = print map information (for PVC)\n");
    printf("\tf,F = print full information (no skipping zero locations)\n");
    printf("\th,H = print this help file\n");
}

void analyse_coff_file(FILE *in)
{
    struct filehdr fhdr;
    struct aouthdr ohdr;
    struct scnhdr shdr;
    int syms;
    unsigned int used = 0, saved_used = 0, i, j, k;
    char *c;
    HDRR hdrr;

/*
 *  first, print out the file header
 */

    FGET(in, 0, (void *)&fhdr, sizeof(fhdr));
    if (!_SEEN('e') && !_SEEN('m')) {
	if _SEEN('v') {
	    printf("File Header (filehdr), after %d bytes\n", used);
	    printf("\tf_magic \t= %O (octal)\n", fhdr.f_magic);
	    printf("\tf_nscns \t= %d (number of sections)\n", fhdr.f_nscns);
	    printf("\tf_timdat \t= 0x%X\n", fhdr.f_timdat);
	    printf("\tf_symptr \t= 0x%X (%d)(file pointer to symbolic header)\n", 
	      fhdr.f_symptr, fhdr.f_symptr);
	    printf("\tf_nsyms \t= 0x%X (%d)(size of symbolic header)\n", 
	      fhdr.f_nsyms, fhdr.f_nsyms);
	    printf("\tf_opthdr \t= 0x%X (%d)(size of optional header)\n", 
	      fhdr.f_opthdr, fhdr.f_opthdr);
	    printf("\tf_flags \t= 0x%X\n", fhdr.f_flags);
	}
    }
    used = sizeof(struct filehdr);
/*
 *  Now print out the optional header.  Save the used offset so that the 
 *  verbose dump can happen.
 */
    FGET(in, used, (void *)&ohdr, sizeof(ohdr));
    if (!_SEEN('e') && !_SEEN('m')) {
	if _SEEN('v') {
	    printf("Optional Header (aouthdr), after %d bytes\n", used);
	    printf("\tmagic \t\t= %O (octal)\n", ohdr.magic);
	    printf("\tvstamp \t\t= 0x%X (version stamp)\n", ohdr.vstamp);
	    printf("\ttsize \t\t= %d (text size in bytes, padded)\n", 
	      ohdr.tsize);
	    printf("\tdsize \t\t= %d (initialized data size in bytes, padded)\n", 
	      ohdr.dsize);
	    printf("\tbsize \t\t= %d (uninitialized data size in bytes, padded)\n", 
	      ohdr.bsize);
	    printf("\tentry \t\t= 0x%X (entry point)\n", ohdr.entry);
	    printf("\ttext_start \t= 0x%X (base of text used for this file)\n", 
	      ohdr.text_start);
	    printf("\tdata_start \t= 0x%X (base of data used for this file)\n", 
	      ohdr.data_start);
	    printf("\tbss_start \t= 0x%X (base of bss used for this file)\n", 
	      ohdr.bss_start);
	    printf("\tgprmask \t= 0x%X (general purpose register mask)\n", 
	      ohdr.gprmask);
	    printf("\tfprmask \t= 0x%X (floating point register mask)\n", 
	      ohdr.fprmask);
	    printf("\tgp_value \t= 0x%X (gp value used for this object)\n", 
	      ohdr.gp_value);
	}
    }

    used = used + fhdr.f_opthdr;

    if (fhdr.f_symptr != 0)
	FGET(in, fhdr.f_symptr, (void *)&hdrr, sizeof(hdrr));

/*
 *  Dump the .data and .text sections.
 */
    if (!_SEEN('e') && !_SEEN('m')) {
	if (!_SEEN('b')) {
	    saved_used = used;
	    for (i = 0; i < fhdr.f_nscns; i++) {
		FGET(in, used, (void *)&shdr, sizeof(shdr));
		if (strcmp(shdr.s_name, ".text") == 0) {
		    printf("\tCode is at %0X (%d) bytes into the file\n", 
		      shdr.s_scnptr, shdr.s_scnptr);
		    dump_text(in, (unsigned int)shdr.s_vaddr, (int)shdr.s_scnptr, 
		      (unsigned int)shdr.s_size, (fhdr.f_symptr == 0 ? NULL : &hdrr));
		}

		if (strcmp(shdr.s_name, ".data") == 0)
		    dump_data(in, (unsigned int)shdr.s_vaddr, (int)shdr.s_scnptr, 
		      (unsigned int)shdr.s_size, (fhdr.f_symptr == 0 ? NULL : &hdrr));

		used = used + sizeof(struct scnhdr);
	    }
	    used = saved_used;
	}
/*
 *  print out all of the sections (fhdr.f_nscns) if we're asked.
 */
	if _SEEN('v') {
	    for (i = 0; i < fhdr.f_nscns; i++) {
		FGET(in, used, (void *)&shdr, sizeof(shdr));
		printf("Section Header (scnhdr), after %d bytes\n", used);
		printf("\tname\t\t= ");
		for (j = 0; j < 8; j++)
		    printf("%c", shdr.s_name[j]);
		printf("\n");
		printf("\ts_paddr\t\t= 0x%X (physical address)\n", 
		  shdr.s_paddr);
		printf("\ts_vaddr\t\t= 0x%X (virtual address)\n", 
		  shdr.s_vaddr);
		printf("\ts_size \t\t= %d (section size)\n", shdr.s_size);
		printf("\ts_scnptr \t= 0x%X (%d)(file pointer to raw data for section)\n", 
		  shdr.s_scnptr, shdr.s_scnptr);
		printf("\ts_relptr \t= 0x%X (file pointer to relocation)\n", 
		  shdr.s_relptr);
		printf("\ts_lnnoptr \t= 0x%X (file pointer to global pointer tables)\n", 
		  shdr.s_lnnoptr);
		printf("\ts_nreloc \t= 0x%d (number of relocation entries)\n", 
		  shdr.s_nreloc);
		printf("\ts_nlnno \t= 0x%d (number of global table pointers)\n", 
		  shdr.s_nlnno);
		printf("\ts_flags \t= 0x%X (flags)\n", shdr.s_flags);
		if (!_SEEN('b')) {
		    if ((shdr.s_nreloc != 0) && (shdr.s_relptr != 0))
			dump_relocation_info(in, (int)shdr.s_relptr, 
			  shdr.s_nreloc);
		}
		used = used + sizeof(struct scnhdr);
	    }

	}
    }
/*
 *  Only carry on if there is symbolic information
 */
    if (fhdr.f_symptr != 0) {
/*
 *  Dump out the symbol table (if asked)
 */
	if (!_SEEN('e') && !_SEEN('m') && !_SEEN('b'))
	    dump_symbol_table(in, &hdrr);

/*
 *  Dump the entry points out (if asked).
 */
	if _SEEN('e')
	    dump_entry_points(in, &hdrr);
/*
 *  Dump out the PVC map (if asked).
 */
	if _SEEN('m')
	    dump_pvc_map(in, &hdrr);
    }
}

void dump_symbol_table(FILE *in, pHDRR hdrr)
{
    if _SEEN('v') {
	printf("symbolic header\n");
	printf("\tmagic \t\t= %o (octal)\n", hdrr->magic);
	printf("\tvstamp \t\t= 0x%X\n", hdrr->vstamp);
	printf("\tilinemax \t= %d (number of line number entries)\n", 
	  hdrr->ilineMax);
	printf("\tidnmax \t\t= %d (maximum index into dense numbers)\n", 
	  hdrr->idnMax);
	printf("\tipdmax \t\t= %d (number of procedures)\n", hdrr->ipdMax);
	printf("\tisymmax \t= %d (number of local symbols)\n", hdrr->isymMax);
	printf("\tioptmax \t= %d (maximum index into optimization entries)\n", 
	  hdrr->ioptMax);
	printf("\tiauxmax \t= %d (number of auxiliary symbols)\n", 
	  hdrr->iauxMax);
	printf("\tissmax \t\t= %d (maximum index into local strings)\n", 
	  hdrr->issMax);
	printf("\tissextmax \t= %d (maximum index into external strings)\n", 
	  hdrr->issExtMax);
	printf("\tifdmax \t\t= %d (number of file descriptors)\n", 
	  hdrr->ifdMax);
	printf("\tcrfd \t\t= %d (number of relative file descriptors)\n", 
	  hdrr->crfd);
	printf("\tiextmax \t= %d (maximum index into external symbols)\n", 
	  hdrr->iextMax);
	printf("\tcbline \t\t= %d (number of bytes for line number entries)\n", 
	  hdrr->cbLine);
	printf("\tcblineoffset \t= %d (index to start of line numbers)\n", 
	  hdrr->cbLineOffset);
	printf("\tcbdnoffset \t= %d (index to start of dense numbers)\n", 
	  hdrr->cbDnOffset);
	printf("\tcbpdoffset \t= %d (index to procedure descriptors)\n", 
	  hdrr->cbPdOffset);
	printf("\tcbsymoffset \t= %d (index to start of local symbols)\n", 
	  hdrr->cbSymOffset);
	printf("\tcboptoffset \t= %d (index to start of optimization entries)\n", 
	  hdrr->cbOptOffset);
	printf("\tcbauxoffset \t= %d (index to start of auxiliary symbols)\n", 
	  hdrr->cbAuxOffset);
	printf("\tcbssoffset \t= %d (index to start of local strings)\n", 
	  hdrr->cbSsOffset);
	printf("\tcbssextoffset \t= %d (index to start of external strings)\n", 
	  hdrr->cbSsExtOffset);
	printf("\tcbfdoffset \t= %d (index to file descriptor)\n", 
	  hdrr->cbFdOffset);
	printf("\tcbrfdoffset \t= %d (index to relative file descriptors)\n", 
	  hdrr->cbRfdOffset);
	printf("\tcbextoffset \t= %d (index to start of external symbols)\n", 
	  hdrr->cbExtOffset);
    }
/*
 *  Dump out the local symbols.
 */

    dump_local_symbols(in, (int)hdrr->cbSymOffset, hdrr);

/*
 *  Dump out the external symbols.
 */
    dump_external_symbols(in, (int)hdrr->cbExtOffset, hdrr);

/*
 *  push out the file descriptor table.
 */
    if _SEEN('v') {
	dump_fds(in, (int)hdrr->cbFdOffset, hdrr->ifdMax, hdrr);
    }
}

void dump_fds(FILE *in, int offset, unsigned int count, pHDRR hdrr)
{
    unsigned int i, j;
    SYMR symbol;
    FDR table;

    for (i = 0; i < count; i++) {
	FGET(in, offset + (sizeof(table) * i), (void *)&table, sizeof(table));
	printf("File Descriptor Entry (at offset %d, index = %d)\n", 
	  offset + (sizeof(table) * i), i);
	printf("\tadr \t\t= 0x%X (%d) (Memory address of start of file)\n", 
	  table.adr, table.adr);
	printf("\tcbLineOffset\t= %d (byte offset from header or file lines)\n", 
	  table.cbLineOffset);
	printf("\tcbLine \t\t= %d\n", table.cbLine);
	printf("\tcbSs \t\t= %d (number of bytes in local strings)\n", 
	  table.cbSs);
	printf("\trss \t\t= 0x%08X (source file name)\n", table.rss);
	printf("\tissBase \t= %d (start of local strings)\n", table.issBase);
	printf("\tisymBase \t= %d (start of local symbol entries)\n", 
	  table.isymBase);
	printf("\tcsym \t\t= %d (count of local symbols)\n", 
	  table.csym);

	printf("\tilineBase \t= %d (start of line number entries)\n", 
	  table.ilineBase);
	printf("\tcline \t\t= %d (count of line number entries)\n", 
	  table.cline);


	printf("\tioptBase \t= %d (start of optimization symbol entries)\n", 
	  table.ioptBase);
	printf("\tcopt\t\t= %d (count of optimization symbol entries)\n", 
	  table.copt);
	printf("\tipdFirst\t= %d (start of procedure description table)\n", 
	  table.ipdFirst);
	printf("\tcpd\t\t= %d (count of procedure descriptors)\n", table.cpd);

	printf("\tiauxBase\t= %d (start of auxiliary symbol entries)\n", 
	  table.iauxBase);
	printf("\tcaux\t\t= %d (count of auxiliary symbol entries)\n", 
	  table.caux);
	printf("\trfdBase\t\t= %d (index into relative file descriptors)\n", 
	  table.rfdBase);
	printf("\tcrfd\t\t= %d (count of relative file descriptors)\n", 
	  table.crfd);
	printf("\tlang\t\t= 0x%05X (language for this file)\n", table.lang);
	printf("\tfMerge\t\t= %s (can this file be merged?)\n", 
	  table.fMerge == TRUE ? 
	  "YES" : "NO");
	printf("\tfReadin\t\t= %s (was this file read?)\n", 
	  table.fReadin == TRUE ? 
	  "YES" : "NO");
	printf("\tglevel\t\t= %d (level this file was compiled with)\n", 
	  table.glevel);

	if (_SEEN('v')) {
	    for (j = 0; j < table.csym; j++) {
		FGET(in,
		  hdrr->cbSymOffset + (table.isymBase * sizeof(symbol)) 
		     + (sizeof(symbol) * j), 
		  (void *)&symbol, sizeof(symbol));
		printf("\tSymbol Table Entry at offset %d\n", 
		       hdrr->cbSymOffset + (table.isymBase * sizeof(symbol)) 
		       + (sizeof(symbol) * j));
		printf("\t\tvalue \t= 0x%X (value of symbol)\n", 
		  symbol.value);
		FGET(in, hdrr->cbSsOffset + table.issBase + symbol.iss,
		     (void *)name, sizeof(name));
		printf("\t\tiss \t= %d (index into local strings of symbol name), %s\n", 
		  symbol.iss, name);
		printf("\t\tst \t= 0x%X (symbol type)\n", symbol.st);
		printf("\t\tsc \t= 0x%X (storage class)\n", symbol.sc);
		printf("\t\tindex \t= %d (index)\n", symbol.index);
	    }
	}

	if (_SEEN('v')) {
	    PDR proc;

	    if (table.cpd != 0) {
		for (j = 0; j < table.cpd; j++) {
		    FGET(in,
		      hdrr->cbPdOffset + table.ipdFirst + (sizeof(proc) * j), 
		      (void *)&proc, sizeof(proc));
		    printf("\tProcedure Descriptor Table Entry at offset %d\n", 
		      hdrr->cbPdOffset + table.ipdFirst + (sizeof(proc) * j));
		    printf("\t\tadr\t\t= 0x%08X (memory address of start of procedure)\n", 
		      proc.adr);
		    printf("\t\tcbLineOffset\t= %d (byte offset for this procedure)\n", 
		      proc.cbLineOffset);
		    printf("\t\tisym\t\t= %d (start of local symbols)\n", 
		      proc.isym);
		    printf("\t\tiline\t\t= %d (procedure's line numbers)\n", 
		      proc.iline);
		    printf("\t\tregmask\t\t= 0x%X (saved register mask)\n", 
		      proc.regmask);
		    printf("\t\tregoffset\t= 0x%X (saved register offset)\n", 
		      proc.regoffset);
		    printf("\t\tframeoffset\t= %d (frame size)\n", 
		      proc.frameoffset);
		    printf("\t\tLnLow\t\t= %d (lowest line in the procedure)\n", 
		      proc.lnLow);
		    printf("\t\tlnHigh\t\t= %d (highest line in the procedure)\n", 
		      proc.lnHigh);
		    printf("\t\tgp_prologue\t= 0x%02X (byte size of GP prologue)\n", 
		      proc.gp_prologue);
		    printf("\t\tgp_used \t= %s (true if the procedure uses GP)\n", 
		      proc.gp_used == TRUE ? "TRUE" : "FALSE");
		    printf("\t\tframereg\t= %d (frame pointer register)\n", 
		      proc.framereg);
		    printf("\t\tpcreg\t\t= %d (index of reg of return pc)\n", 
		      proc.pcreg);
		}
	    }

	}
    }
}


void dump_relocation_info(FILE *in, int offset, unsigned int count)
{
    struct reloc r;
    int i;

    for (i=0; i < count; i++) {
	FGET(in, offset + (sizeof(r) * i), (void *)&r, sizeof(r));
	printf("\tRelocation information at offset %d\n", offset + (sizeof(r) * i));
	printf("\t\tr_vaddr\t\t= %08X (va of item to be relocated)\n", 
	  r.r_vaddr);
	printf("\t\tr_symndx\t= %d (", 
	  r.r_symndx);
	if (r.r_extern == TRUE)
	    printf("index into external symbols)");
	else {
	    printf("section containing symbol = ");
	    switch (r.r_symndx) {
		case R_SN_TEXT: 
		    printf(".text");
		    break;
		case R_SN_INIT: 
		    printf(".init");
		    break;
		case R_SN_RDATA: 
		    printf(".rdata");
		    break;
		case R_SN_DATA: 
		    printf(".data");
		    break;
		case R_SN_XDATA: 
		    printf(".xdata");
		    break;
		case R_SN_PDATA: 
		    printf(".pdata");
		    break;
		case R_SN_SDATA: 
		    printf(".sdata");
		    break;
		case R_SN_SBSS: 
		    printf(".sbss");
		    break;
		case R_SN_BSS: 
		    printf(".bss");
		    break;
		case R_SN_LIT8: 
		    printf(".lit8");
		    break;
		case R_SN_LIT4: 
		    printf(".lit4");
		    break;
		case R_SN_FINI: 
		    printf(".fini");
		    break;
		case R_SN_LITA: 
		    printf(".lita");
		    break;
		case R_SN_ABS: 
		    printf(".abs");
		    break;
		default: 
		    printf("UNKNOWN");
	    }
	}
	printf(")\n");
	printf("\t\tr_type\t\t= %d (", r.r_type);
	switch (r.r_type) {
	    case R_ABS: 
		printf("Relocation already performed");
		break;
	    case R_REFLONG: 
		printf("32 bit reference to symbol's va");
		break;
	    case R_REFQUAD: 
		printf("64 bit reference to symbol's va");
		break;
	    case R_GPREL32: 
		printf("32 bit displacement from gp");
		break;
	    case R_LITERAL: 
		printf("literal in the literal, offset from gp");
		break;
	    case R_LITUSE: 
		printf("usage of a literal address");
		break;
	    case R_GPDISP: 
		printf("lda/ldah use to init a proc's gp");
		break;
	    case R_BRADDR: 
		printf("21 bit branch reference to symbol's va");
		break;
	    case R_HINT: 
		printf("14 bit jsr hint reference to symbol's va");
		break;
	    case R_SREL16: 
		printf("self relative 16 bit offset");
		break;
	    case R_SREL32: 
		printf("self relative 32 bit offset");
		break;
	    case R_SREL64: 
		printf("self relative 64 bit offset");
		break;
	    case R_OP_PUSH: 
		printf("push onto stack");
		break;
	    case R_OP_STORE: 
		printf("store onto stack");
		break;
	    case R_OP_PSUB: 
		printf("some stack operation (PSUB)");
		break;
	    case R_OP_PRSHIFT: 
		printf("another stack operation (PRSHIFT)");
	}
	printf(")\n");
	printf("\t\tr_extern\t= %s\n", 
	  r.r_extern == TRUE ? "EXTERNAL" : "INTERNAL");
    }
}

void dump_external_symbols(FILE *in, int offset, pHDRR hdrr)
{
    unsigned int i;
    unsigned int count = hdrr->iextMax;
    pFDR fdr;
    EXTR external;

    printf("External Symbols\n");
    for (i = 0; i < count; i++) {

	FGET(in, offset + (sizeof(external) * i), (void *)&external, sizeof(external));
	FGET(in, hdrr->cbSsExtOffset + external.asym.iss, (void *)name, sizeof(name));

	if _SEEN('v') {
	    printf("\tExternal Symbol at offset %d\n", offset + (sizeof(external) * i));
	    printf("\t\tasym.value\t= %08X (%d)\n", external.asym.value, 
	      external.asym.value);
	    printf("\t\tasym.iss\t= %d (%s) (index in string space of name)\n", 
	      external.asym.iss, name);
	    printf("\t\tasym.st\t\t= %d (storage type)\n", 
	      external.asym.st);
	    printf("\t\tasym.sc\t\t= %d (storage class)\n", 
	      external.asym.sc);
	    printf("\t\tasym.index\t= %d (index into sym/aux table)\n", 
	      external.asym.index);
	    printf("\t\tjmptbl\t\t= %s (jump table entry for share libraries?)\n", 
	      external.jmptbl == TRUE ? "TRUE" : "FALSE");
	    printf("\t\tcobol_main\t= %s (cobol main procedure)\n", 
	      external.cobol_main == TRUE ? "TRUE" : "FALSE");
	    printf("\t\tweakext\t\t= %s (weak external?)\n", 
	      external.weakext == TRUE ? "TRUE" : "FALSE");
	    printf("\t\tifd\t\t= %d (where iss and index fields point to)\n", 
	      external.ifd);
	} else {
	    printf("\tifd = %3d, value = %08X, index = %08X, %s\n", 
	      external.ifd, 
	      external.asym.value, external.asym.index, name);
	}
    }
}

void dump_local_symbols(FILE *in, int offset, pHDRR hdrr)
{
    unsigned int i;
    unsigned int count = hdrr->isymMax;
    SYMR local;

    printf("Local Symbols\n");
    for (i = 0; i < count; i++) {
	FGET(in, offset + (sizeof(local) * i), (void *)&local, sizeof(local));
	FGET(in, hdrr->cbSsOffset + local.iss, (void *)name, sizeof(name));
	if _SEEN('v') {
	    printf("\tLocal Symbol at offset %d\n", offset + (sizeof(local) * i));
	    printf("\t\tvalue\t= %08X (%d)\n", local.value, 
	      local.value);
	    printf("\t\tiss\t= %d (%s)(index in string space of name)\n", 
	      local.iss, name);
	    printf("\t\tst\t= %d (storage type)\n", 
	      local.st);
	    printf("\t\tsc\t= %d (storage class)\n", 
	      local.sc);
	    printf("\t\tindex\t= %d (index into sym/aux table)\n", 
	      local.index);
	} else {
	    printf("\tvalue = 0x%08X, index = %3d, %s\n", 
	      local.value, local.index, name);
	}
    }
}

void dump_text(FILE *in, unsigned int physical, int offset, unsigned int size, 
  pHDRR hdrr)
{
    unsigned int i, zeros = 0;
    char *symbol = NULL;
    unsigned int p;

    printf("Executable Code (starting at %08X)\n", physical);
    size = size / sizeof(unsigned int);

    for (i = 0; i < size; i++) {
/*
 *  Get the next instruction from the file.
 */
	FGET(in, offset + (i * sizeof(unsigned int)), (void *)&p,
	     sizeof(p));
/*
 *  Print out the symbols at this address.  Make sure we code with no
 *  instructions at that address.
 */

	symbol = symbol_name_from_value(in, physical, hdrr);
	if (symbol != NULL) {
	    if (p == 0)
		printf("\t[0x%08X] 0x%08X\n", physical, p);
	    print_symbols_at_address(in, physical, hdrr);
	}

	if (p == 0) {
	    if (zeros == 0)
		printf("\t\t:\n\tzero locations skipped\n\t\t:\n");
	    zeros++;
	} else {
	    printf("\t[0x%08X] 0x%08X ", physical, p);
	    print_instruction((int) p, stdout);
	    symbol = symbol_name_from_value(in, physical, hdrr);
	    if (symbol != NULL)
		printf("\t(%s)", symbol);
	    printf("\n");
	    zeros = 0;
	}
	physical += 4;
    }
}

void dump_data(FILE *in, unsigned int physical, int offset, unsigned int size, 
  pHDRR hdrr)
{
    unsigned int i, zeros = 0;
    char *ref = NULL, *symbol = NULL;
    unsigned int p;

    printf("Data\n");
    size = size / sizeof(unsigned int);
    for (i = 0; i < size; i++) {
/*
 *  Get the next instruction from the file.
 */
	FGET(in, offset + (i * sizeof(unsigned int)), (void *)&p,
	     sizeof(p));
/*
 *  Print out the symbols at this address.  Make sure we cope with null
 *  data at that address.
 */
/*	symbol = symbol_name_from_value(in, physical);   */
	if (symbol != NULL) {
	    if (!_SEEN('f')) {
		if (p == 0)
		    printf("\t[0x%08X] 0x%08X\n", physical, p);
	    }
/*	    print_symbols_at_address(in, physical); */
	}

	if (_SEEN('f')) {
	    printf("\t[0x%08X] 0x%08X\n", physical, p);
	} else {
	    if (p == 0) {
		if (zeros == 0)
		    printf("\t\t:\n\tzero locations skipped\n\t\t:\n");
		zeros++;
	    } else {
		printf("\t[0x%08X] 0x%08X\n", physical, p);
		zeros = 0;
	    }
	}
	physical += 4;
    }
}


void dump_entry_points(FILE *in, pHDRR hdrr)
{
    unsigned int i;
    unsigned int count;
    SYMR local;
    EXTR external;

    if (!_SEEN('e'))
	printf("Entry Points\n");

/*
 *  Search the local symbols first.
 */
    count = hdrr->isymMax;
    for (i = 0; i < count; i++) {
	FGET(in, hdrr->cbSymOffset + (sizeof(local) * i), 
	  (void *)&local, sizeof(local));
	if (((local.st == stLabel) || (local.st == stProc)) && 
	  (local.sc == scText)) {
	    FGET(in, hdrr->cbSsOffset + local.iss, (void *)name, sizeof(name));
	    if (is_pvc_label(name) == FALSE)
		printf("%08X\t%s\n", local.value, name);
	}
    }
/*
 *  Now search the external (externally available) symbols
 */
    count = hdrr->iextMax;
    for (i = 0; i < count; i++) {
	FGET(in, hdrr->cbExtOffset + (sizeof(external) * i), 
	  (void *)&external, sizeof(external));
	if (((external.asym.st == stLabel) || 
	  (external.asym.st == stProc)) && 
	  (external.asym.sc == scText)) {
	    FGET(in, hdrr->cbSsExtOffset + external.asym.iss, 
	      (void *)name, sizeof(name));
	    if (is_pvc_label(name) == FALSE)
		printf("%08X\t%s\n", external.asym.value, name);
	}
    }
}

void dump_pvc_map(FILE *in, pHDRR hdrr)
{
    if (hdrr != NULL) {
	unsigned int i;
	unsigned int count;
	SYMR local;
	EXTR external;


	printf("Map locations for PVC labels\n");
/*
 *  First, search the local symbols for this value.
 */
	count = hdrr->isymMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbSymOffset + (sizeof(local) * i), 
	      (void *)&local, sizeof(local));
	    if (((local.st == stLabel) || (local.st == stProc)) && 
	      (local.sc == scText)) {
		FGET(in, hdrr->cbSsOffset + local.iss, (void *)name, 
		  sizeof(name));
		if (is_pvc_label(name) == TRUE)
		    printf("%08X\t%s\n", local.value, name);
	    }
	}

/*
 *  Now search the external (externally available) symbols
 */
	count = hdrr->iextMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbExtOffset + (sizeof(external) * i), 
	      (void *)&external, sizeof(external));
	    if (((external.asym.st == stLabel) || 
	      (external.asym.st == stProc)) && 
	      (external.asym.sc == scText)) {
		FGET(in, hdrr->cbSsExtOffset + external.asym.iss, 
		  (void *)name, sizeof(name));
		if (is_pvc_label(name) == TRUE)
		    printf("%08X\t%s\n", external.asym.value, name);
	    }
	}
    }
}

void print_symbols_at_address(FILE *in, unsigned int physical, pHDRR hdrr)
{
    if (hdrr != NULL) {
	unsigned int i;
	unsigned int count;
	SYMR local;
	EXTR external;
/*
 *  First, search the local symbols for this value.
 */

	count = hdrr->isymMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbSymOffset + (sizeof(local) * i), 
	      (void *)&local, sizeof(local));
	    if (((local.st == stLabel) || (local.st == stProc)) && 
	      (local.sc == scText)) {
		if (local.value == physical) {
		    FGET(in, hdrr->cbSsOffset + local.iss, (void *)name, 
		      sizeof(name));
		    printf("%s:\n", name);
		}
	    }
	}
/*
 *  Now search the external (externally available) symbols
 */
	count = hdrr->iextMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbExtOffset + (sizeof(external) * i), 
	      (void *)&external, sizeof(external));
	    if (((external.asym.st == stLabel) || 
	      (external.asym.st == stProc)) && 
	      (external.asym.sc == scText)) {
		if (external.asym.value == physical) {
		    FGET(in, hdrr->cbSsExtOffset + external.asym.iss, 
		      (void *)name, sizeof(name));
		    printf("%s:\n", name);
		}
	    }
	}
    }
}

char *symbol_name_from_value(FILE *in, unsigned int physical, pHDRR hdrr)
{
    if (hdrr != NULL) {
	unsigned int i;
	unsigned int count;
	SYMR local;
	EXTR external;
/*
 *  First, search the local symbols for this value.
 */
	count = hdrr->isymMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbSymOffset + (sizeof(local) * i), 
	      (void *)&local, sizeof(local));
	    if (local.value == physical) {
		FGET(in, hdrr->cbSsOffset + local.iss, 
		  (void *)name, sizeof(name));
		return (name);
	    }
	}
/*
 *  Now search the external (externally available) symbols
 */
	count = hdrr->iextMax;
	for (i = 0; i < count; i++) {
	    FGET(in, hdrr->cbExtOffset + (sizeof(external) * i), 
	      (void *)&external, sizeof(external));
	    if (external.asym.value == physical) {
		FGET(in, hdrr->cbSsExtOffset + external.asym.iss,
		     (void *)name, sizeof(name));
		return (name);
	    }
	}
    }
    return (NULL);
}

unsigned int is_pvc_label(char *label)
{
    if ((strstr(label, "PVC$") != NULL) || 
      (strstr(label, "pvc$") != NULL))
	return (TRUE);
    else
	return (FALSE);
}
