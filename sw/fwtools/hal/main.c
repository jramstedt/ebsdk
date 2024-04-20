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
#include <signal.h>
#include <ctype.h>
#include <assert.h>

#include "hal.h"
#include "version.h"

/*
 * Variables which are only externed to be intialized.
 */
extern int macro_line_size;
extern char* macro_line;
extern char* macro_line_ptr;
extern int line_size;
extern char* line;
extern char* lptr;
extern int listing_line_size;
extern char* listing_line;
extern char* listing_line_ptr;

extern int total_alloc;
extern int numtokens;

extern int num_errors;
extern int num_warnings;

int allow_core_dump;
int warn_reloc_absexpr;
int quiet;
int page_size;
int object_output;
int chip;
int print_no_trace;
unsigned int global_pragma;

FILE* listing_file;
int listing_enable;
char* listing_fname;
int listing_flags;

FILE* save_file;
int save_enable;
FILE* rest_file;
int rest_enable;

CMDINFO *cmd_list;
TOKEN_LIST* fname_list;

/*
 * Holds the old SIGSEGV handler.
 */
void (*oldsegv) PROTO((int));

void psectlist PROTO((char*, PENTRY*));
void symtableit PROTO((char*, SENTRY*));

#ifdef __osf__
int osf_object();
#else
int osf_object()
{
    /*
     * Stub routine for non-OSF platforms.
     */
    return 0;
}
#endif

#ifdef __STDC__
void parse_cmdline(int argc, char *argv[])
#else
void parse_cmdline(argc, argv)
int argc;
char *argv[];
#endif
{
    char *tstr;
    FILE *tfil;
    int i;

    /* skip over the image name */
    for (i=1; i<argc; ++i) {
        char *arg = argv[i];

        switch (*arg) {
        case '-':
            switch (*(++arg)) {
            case 'o':
                /* output file specification, linked image */
                ++i;
                output_file = argv[i];
                break;
#ifdef __osf__
            case 'O':
                /* output file specification, osf object module */
                ++i;
                output_file = argv[i];
                object_output = 1;
                break;
#endif
            case 'p':
                /* page size specification */
                arg = argv[++i];
                page_size = strtol(arg, NULL, 10);
                break;
            case 'D':
                ++arg;
                if ((*arg != '\0') && (strcmp(arg, "all") != 0)) {
                    while (*arg) {
                        if (*arg == 'b') yydebug = 1;
                        else if (*arg == 'f') yy_flex_debug = 1;
                        else if (*arg == 't') {
                            if (Trace) {
                                errout(E_WARN | E_NO_LNUM, "hal", "tracing already specified, ignoring repeat request.\n");
                            } else {
                                ++i;
#ifdef UNIX
                                {
                                    char tmp_flnm[PATH_MAX + 1];
                                    UNIX_expand_pathname(argv[i], tmp_flnm);
                                    Ot = fopen(tmp_flnm, "w");
                                }
#else
                                Ot = fopen(argv[i], "w");
#endif
                                if (Ot == NULL) {
                                    errout(E_ERR | E_NO_LNUM, "hal", "%s (file %s)\n", strerror(errno), argv[i]);
                                    errout(E_ERR | E_NO_LNUM, "hal", "unable to open trace file %s, tracing disabled..\n", argv[i]);
                                } else {
                                    Trace = 1;
                                    errout(E_INFO | E_NO_LNUM, "hal", "opening trace file %s.\n", argv[i]);
                                }
                            }
                        } else
                            errout(E_ERR | E_NO_LNUM, "hal", "unknown debug character: \"%c\".\n", *arg);
                        ++arg;
                    }
                } else {
                    yydebug = 1;
                    yy_flex_debug = 1;
                }
                break;
            case 'q':
                /* shuts off info (%I) messages */
                quiet = 1;
                break;
            case 'i':
                /* input file specification */
                ++i;
                add_input(strcpy(ALLOC_STR(strlen(argv[i])), argv[i]), F_FILE, 1, NULL, ADD_APPEND, 0, 0);
                break;
            case 'e':
                /* executable line specification */
                ++arg;
                if (*arg != '\0') {
                  sprintf(tstr = ALLOC_STR(strlen(arg)+13), ".cmd_input %s\n",arg);
                  add_input(tstr, F_STRING, 1, NULL, ADD_APPEND, 0, 0);
                }
                ++i;
                sprintf(tstr = ALLOC_STR(strlen(argv[i])+2), "%s\n", argv[i]);
                add_input(tstr, F_STRING, 1, NULL, ADD_APPEND, 0, 0);
                if (*arg != '\0') {
                  sprintf(tstr = ALLOC_STR(7), ".endi\n",arg);
                  add_input(tstr, F_STRING, 1, NULL, ADD_APPEND, 0, 0);
                }
                break;
            case 'C':
                allow_core_dump = 1;
                break;
            case 'h':
                /* hardware opcodes selector */
                arg = argv[++i];
                if (strcmp(arg,"EV4")==0 || strcmp(arg,"ev4")==0)
                    chip = EV4;
                else if (strcmp(arg,"EV5")==0 || strcmp(arg,"ev5")==0)
                    chip = EV5;
                else if (strcmp(arg,"EV6")==0 || strcmp(arg,"ev6")==0)
                    chip = EV6;
                else
                    errout(E_ERR | E_NO_LNUM, "hal", "unknown hardware type: \"%s\".\n", arg);
                break;
            case 'l':
                /* listing file name */
                ++i;

#ifdef UNIX
                {
                    char tmp_flnm[PATH_MAX + 1];
                    UNIX_expand_pathname(argv[i], tmp_flnm);
                    listing_file = fopen(tmp_flnm, "w");
                }
#else
                listing_file = fopen(argv[i], "w");
#endif
                if (listing_file == NULL) {
                    errout(E_ERR | E_NO_LNUM, "hal", "%s (file %s)\n", strerror(errno), argv[i]);
                    errout(E_ERR | E_NO_LNUM, "hal", "unable to open listing file %s, listing disabled..\n", argv[i]);
                } else {
                    errout(E_INFO | E_NO_LNUM, "hal", "opening listing file %s.\n", argv[i]);
                    listing_fname = argv[i];
                    listing_enable = 1;
                }
                break;
            case 'd':
                /* macro save file name */
                ++i;

#ifdef UNIX
                {
                    char tmp_flnm[PATH_MAX + 1];
                    UNIX_expand_pathname(argv[i], tmp_flnm);
                    save_file = fopen(tmp_flnm, "w");
                }
#else
                save_file = fopen(argv[i], "w");
#endif
                if (save_file == NULL) {
                    errout(E_ERR | E_NO_LNUM, "hal", "%s (file %s)\n", strerror(errno), argv[i]);
                    errout(E_ERR | E_NO_LNUM, "hal", "unable to open macro save file %s, saving disabled..\n", argv[i]);
                } else {
                    errout(E_INFO | E_NO_LNUM, "hal", "opening save file %s.\n", argv[i]);
                    save_enable = 1;
                }
                break;
            case 'c':
                /* macro restore file name */
                ++i;

#ifdef UNIX
                {
                    char tmp_flnm[PATH_MAX + 1];
                    UNIX_expand_pathname(argv[i], tmp_flnm);
                    rest_file = fopen(tmp_flnm, "r");
                }
#else
                rest_file = fopen(argv[i], "r");
#endif
                if (rest_file == NULL) {
                    errout(E_ERR | E_NO_LNUM, "hal", "%s (file %s)\n", strerror(errno), argv[i]);
                    errout(E_ERR | E_NO_LNUM, "hal", "unable to open macro configuration file %s, restoring disabled..\n", argv[i]);
                } else {
                    errout(E_INFO | E_NO_LNUM, "hal", "opening configuration file %s.\n", argv[i]);
                    rest_enable = 1;
                }
                break;
            case 's':
                /* system start address of code */
                {
                    TOKEN_LIST* address;
                    int radix = 16;
                    char* tstr = argv[++i];

                    if (*tstr == '^') {
                        switch(tolower(*(++tstr))) {
                        case 'x': radix = 16; break;
                        case 'o': radix = 8; break;
                        case 'b': radix = 2; break;
                        default:
                            errout(E_ERR | E_NO_LNUM, "hal", "unknown radix (^%c) on -system parameter ignored..\n", *tstr);
                            break;
                        }
                        ++tstr;
                    }

                    address = cvtradix(tstr, radix, NUMBER);
                    Sa[1] = address->tok.q.v[1];
                    Sa[0] = address->tok.q.v[0];
                }
                break;
            case 'w':
                switch (*(++arg)) {
                case 'a':
                    warn_reloc_absexpr = 1;
                    break;
                case '\0':
                    errout(E_ERR | E_NO_LNUM, "hal", "illegal option: -w option requires a second character.\n");
                    break;
                default:
                    errout(E_ERR | E_NO_LNUM, "hal", "unknown option: -w%c ignored.\n", *arg);
                    break;
                }
                break;                    
            case 'n':
                switch (*(++arg)) {
                case 't':
                    print_no_trace = 1;
                    break;
                case '\0':
                    errout(E_ERR | E_NO_LNUM, "hal", "illegal option: -n option requires a second character.\n");
                    break;
                default:
                    errout(E_ERR | E_NO_LNUM, "hal", "unknown option: -n%c ignored.\n", *arg);
                    break;
                }
                break;                    
            default:
                errout(E_ERR | E_NO_LNUM, "hal", "unknown option: -%c ignored.\n", *arg);
                break;
            }
            break;
        default:
            /* by default, an argument is an input filename */
            add_input(strcpy(ALLOC_STR(strlen(arg)), arg), F_FILE, 1, NULL, ADD_APPEND, 0, 0);
        }
    }

#ifdef __DATE__
    errout(E_INFO | E_NO_LNUM, "hal", "HAL (Hudson Assembler Linker) %s, compiled %s %s\n",
	   VERSION, __DATE__, __TIME__);
#else
    errout(E_INFO | E_NO_LNUM, "hal", "HAL (Hudson Assembler Linker) %s\n",
	   VERSION);
#endif

    if (argc == 1) {
        /* no arguments specified, print usage info. */
        fprintf(stderr, "USAGE: hal <input files> [options]\n");
        fprintf(stderr, "\t-o <file>\t\tspecifies image output file.\n");
#ifdef __osf__
        fprintf(stderr, "\t-O <file>\t\tspecifies object module output file (OSF only).\n");
#endif
        fprintf(stderr, "\t-l <file>\t\tspecifies (and enables) listing file.\n");
        fprintf(stderr, "\t-d <file>\t\tspecifies (and dumps) macros to save file.\n");
        fprintf(stderr, "\t-c <file>\t\tspecifies (and configures) macros from save file.\n");
        fprintf(stderr, "\t-p <size>\t\tspecifies page size.\n");
        fprintf(stderr, "\t-D [bft|all] [<file>]\trun in debug mode.\n");
        fprintf(stderr, "\t\tb\tturn on bison debug info\n");
        fprintf(stderr, "\t\tf\tturn on flex debug info\n");
        fprintf(stderr, "\t\tt\tturn on internal debug info\n");
        fprintf(stderr, "\t\tall\tturn on all debug info\n");
        fprintf(stderr, "\t\t<file>\tname of file to place info from \"t\" option in.\n");
        fprintf(stderr, "\t-q\t\t\tquiet mode, suppresses info messages.\n");
        fprintf(stderr, "\t-i <file>\t\tspecifies an input file.\n");
        fprintf(stderr, "\t-e <line>\t\tspecifies a MACRO line to execute.\n");
        fprintf(stderr, "\t-e# <line>\t\tspecifies a .cmd_input line to place in buffer #.\n");
        fprintf(stderr, "\t-C\t\t\tallows core/stack dump.\n");
        fprintf(stderr, "\t-s <base>\t\tspecifies base address for link.\n");
        fprintf(stderr, "\t-h <name>\t\tspecifies which hardware opcodes to use (default = EV5).\n");
        fprintf(stderr, "\t-wa\t\t\tprints warning if absolute expression was relocatable.\n");
        fprintf(stderr, "\t-nt\t\t\tsuppresses stack trace printing for .print directives.\n");
        exit(EXIT_FAILURE);
    }
    
    return;
}

void init()
{
  /* must be first, avl_init_table uses the memmgt system */
  init_memmgt();

  allow_core_dump = 0;
  warn_reloc_absexpr = 0;
  quiet = 0;
  print_no_trace = 0;
  chip = EV5;
  page_size = 13;
  local_label = 30000;
  listing_enable = 0;
  listing_flags = L_CALLS | L_CONDITIONALS | L_DEFINITIONS | L_MISC;

  listing_file = NULL;
  listing_fname = NULL;

  object_output = 0;

  save_enable = 0;
  rest_enable = 0;
  
  line_size = 8192;
  macro_line_size = 2048;
  listing_line_size = 2048;
#if defined(VMS) || defined(sun)
  /* both VMS and sun's fail when NULL is passed to realloc
     so much for standards, huh? */
  lptr = line = (char *)malloc(line_size + 1);
  *lptr = '\0';
  macro_line_ptr = macro_line = (char *)malloc(macro_line_size + 1);
  *macro_line_ptr = '\0';
  listing_line_ptr = listing_line = (char *)malloc(listing_line_size + 1);
  *listing_line_ptr = '\0';
#else
  lptr = line = (char *)realloc(NULL, line_size + 1);
  *lptr = '\0';
  macro_line_ptr = macro_line = (char *)realloc(NULL, macro_line_size + 1);
  *macro_line_ptr = '\0';
  listing_line_ptr = listing_line = (char *)realloc(NULL, listing_line_size + 1);
  *listing_line_ptr = '\0';
#endif
  
  macstk = NULL;
  global_pragma = PRAGMA_MAC_UNLIMITED;

  cur_psect = NULL;
  psect_stk = NULL;

  cur_fname = NULL;
  cur_base_lineno = 0;

  ifile = NULL;
  cur_ifile = NULL;

  yy_flex_debug = 0;
  
  output_file = NULL;
  
  cmd_list = NULL;

  fname_list = NULL;

  idirs = NULL;
  
  return;
}

void uninit()
{
  int i;
  CMDINFO *cmd_p_next;
  TOKEN_LIST* fname_next;
  
  for (; cmd_list; cmd_list = cmd_p_next) {
      cmd_p_next = cmd_list->next;
      if (cmd_list->fileptr)
          fclose(cmd_list->fileptr);
      remove(cmd_list->filename);
      FREE(cmd_list);
  }

#ifdef CLEANUP_AT_END
  free(line);
  free(macro_line);
  free(listing_line);

  for (; fname_list != NULL; fname_list = fname_next) {
      fname_next = fname_list->next;
      free_tok_s(fname_list);
  }

  free_tok_s(cur_psect);
  
#ifdef USE_MALLOC_FREE
  avl_free_table(SymTable,FREE_FUNCT,symfree);
  dll_free(DSecTable,NULL);	/* all dsects are also in the psect table. */
  avl_free_table(SecTable,FREE_FUNCT,sect_free);
#endif
  
  end_memmgt();
#endif

  return;
}

/* segv_catcher
 *
 * Handles Segmentation faults (access violations).
 *
 * Prints some info about where in the source files we were.
 */
void segv_catcher(code)
int code;
{
  signal(SIGSEGV, oldsegv);

  errout(E_ERR, "segv_catcher", "Segmentation fault\n");

  uninit();
  exit(EXIT_FAILURE);
}

#ifdef __STDC__
main(int argc, char *argv[])
#else
main(argc, argv)
int argc;
char *argv[];
#endif
{
    TOKEN_LIST *tname, *tone;
    int status;
    fpos_t lpos;
    char *start_cmd_p, *cmd_p;

    /* local init */
    init();

    /* init for hal.c */
    hal_init_before();
  
#ifdef THINK_C
    argc = ccommand(&argv);
#endif

    /* setup is_hal variable */
    start_cmd_p = "is_hal = 1\n";
    cmd_p = strcpy(ALLOC_STR(strlen(start_cmd_p)), start_cmd_p);
    add_input(cmd_p, F_STRING, 1, NULL, ADD_APPEND, 0, 0);
    
    parse_cmdline(argc,argv);

    hal_init_after();

    if (!quiet) {
        if (chip == EV5)
            errout(E_INFO| E_NO_LNUM, "hal", "Hardware mode is EV5\n");
        else if (chip == EV6)
            errout(E_INFO | E_NO_LNUM, "hal", "Hardware mode is EV6\n");
        else
            errout(E_INFO | E_NO_LNUM, "hal", "Hardware mode is EV4\n");
    }    

    load_default_operators();

    if (rest_enable) {
        rest_macro(rest_file);
    }
    
    if (ifile == NULL) {
        errout(E_WARN | E_NO_LNUM, "hal", "no input files specified.\n");
        uninit();
        exit(EXIT_FAILURE);
    }

    /* open first input file */
    (void)next_file();

    if (!allow_core_dump)
        oldsegv = signal(SIGSEGV, segv_catcher);

    (void)yyparse();

    psect_save(Cpsect);	/* Save the last Psect information */


    errout(E_INFO | E_NO_LNUM, "hal", "Finished assembling files.  Now entering link stage...\n");

    if (listing_file != NULL)
        fgetpos(listing_file, &lpos);
    
    if (DSecTable->head != NULL) {
        dll_sort(DSecTable);
    }
    else {
      sect_foreach(SecTable,DSecTable,psectcalc,NULL,SECT_FORWARD); /* NULL is ok since it won't be used */
    }
    avl_foreach(SymTable,symupdate,AVL_FORWARD);
    avl_foreach(SymTable,symresolve,AVL_FORWARD);

    if (absexpr_head != NULL) {
        int numexpr = 0, numfail = 0;
        ABSEXPR* next_abs;
        
        /*
         * reevaluate absolute expressions which contained relocateable
         * symbols.  if the value is different than the first evaluation,
         * print a warning for the user.
         */
        do {
            TOKEN_LIST* newresult = eval_expr(absexpr_head->expr, NULL);

            if (newresult != NULL) {
                assert(absexpr_head->result->type == NUMBER);
                assert(newresult->type == NUMBER);
                
                /*
                 * Check to see if the result is the same as the previous result.
                 */
                if ((absexpr_head->result->tok.q.v[0] != newresult->tok.q.v[0]) ||
                    (absexpr_head->result->tok.q.v[1] != newresult->tok.q.v[1])) {
                    int fail = 1;

                    if (absexpr_head->qual != 0) {
                        switch (absexpr_head->qual) {
                        case EQ:
                            fail = COND_EQ(newresult) ^ absexpr_head->qual_result;
                            break;
                        case GT:
                            fail = COND_GT(newresult) ^ absexpr_head->qual_result;
                            break;
                        case LT:
                            fail = COND_LT(newresult) ^ absexpr_head->qual_result;
                            break;
                        case NE:
                            fail = (!COND_EQ(newresult)) ^ absexpr_head->qual_result;
                            break;
                        case GE:
                            fail = (COND_EQ(newresult) || COND_GT(newresult)) ^ absexpr_head->qual_result;
                            break;
                        case LE:
                            fail = (COND_EQ(newresult) || COND_LT(newresult)) ^ absexpr_head->qual_result;
                            break;
                        case DOT:
                            /*TEMPORARY*/
                            fail = 0;
                            break;
                        }
                    }

                    if (fail) {
                        errout(E_WARN | E_NO_LNUM, "hal", "result of absolute expression altered by psect relocation.\n");
                        errout(E_WARN | E_NO_LNUM, "hal", "original  value = %08X %08X.\n", absexpr_head->result->tok.q.v[1], absexpr_head->result->tok.q.v[0]);
                        errout(E_WARN | E_NO_LNUM, "hal", "relocated value = %08X %08X.\n", newresult->tok.q.v[1], newresult->tok.q.v[0]);
                        print_location(E_WARN, "hal", absexpr_head->lloc);
                        ++numfail;
                    }
                }

                free_tok(newresult);
            }

            next_abs = absexpr_head->next;
            free_absexpr(absexpr_head);
            absexpr_head = next_abs;
            numexpr++;
        } while (absexpr_head != NULL);

        errout(E_INFO | E_NO_LNUM, "hal", "%d absolute expression%s checked, %d failed.\n", numexpr, (numexpr>1) ? "s" : "", numfail);
    }
    
    if (object_output) {
        osf_object();
    } else {
        
        if (output_file)
            generate_code();
        else
            errout(E_INFO | E_NO_LNUM, "hal", "no output file specified, no code generated.\n");

        if (save_enable)
            save_macro();
    }

    if (Trace)
        fclose(Ot);

    if (listing_file != NULL) {
        fsetpos(listing_file, &lpos);

        fprintf(listing_file, "\n\n\t\t\tSYMBOL TABLE\n\t\t\t------------\n");
        avl_foreach(SymTable, symtableit, AVL_FORWARD);

        fprintf(listing_file, "\n\n%-20s  %-9s  %-8s  %-19s\n", "PSECT name","Alignment","Bytes","Base address");
        avl_foreach(SecTable, psectlist, AVL_FORWARD);

        fclose(listing_file);
    }

    uninit();

#ifdef BLOCK_TOKEN_ALLOC
    errout(E_INFO | E_NO_LNUM, "hal", "Peak token usage: %d.\n", numtokens);
#endif
#ifndef USE_MALLOC_FREE
    errout(E_INFO | E_NO_LNUM, "hal", "total memory requested: %d bytes (%dK).\n", total_alloc, (total_alloc + 1023)/1024);
#endif

    if (!allow_core_dump)
        (void)signal(SIGSEGV, oldsegv);

    if ((num_warnings != 0) || (num_errors != 0))
        exit(EXIT_FAILURE);
    
    exit(EXIT_SUCCESS);
}
