/*
 * Copyright (C) 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Context for blex
 *
 * Author:	J. Kirchoff
 *
 */

/*
 * Bit assignments for the mask used to control substitutions
 */
#define	SUB_PARSUB	0x00000001
#define	SUB_CMDSUB	0x00000002
#define	SUB_BLANK	0x00000004
#define	SUB_GENFILES	0x00000008
#define	SUB_QUOTE	0x00000010
#define	SUB_EXPAND	0x00000020
#define	SUB_RENDER	0x80000000
#define	SUB_DEFAULTS	(SUB_PARSUB | SUB_CMDSUB | SUB_BLANK | SUB_GENFILES | SUB_QUOTE)

enum tokens {
	t_null,
	t_space,
	t_eof,
	t_in,			/* keep the next six tokens in this order */
	t_in_here,
	t_out,
	t_out_append,
	t_err,
	t_err_append,
	t_cpuspec,
	t_word,
	t_ampersand,
	t_and,
	t_pipe,
	t_or,
	t_semicolon,
	t_caseterm,
	t_lparen,
	t_rparen,
	t_comment,
	t_universal,
	t_ret,
	t_if,
	t_then,
	t_else,
	t_elif,
	t_fi,
	t_case,
	t_esac,
	t_for,
	t_while,
	t_until,
	t_do,
	t_done,
	t_lbrace,
	t_rbrace,
	t_inkey,
	t_last			/* keep this last	*/
};

enum nodetypes {
	n_andor = 0,
	n_case,
	n_command_list,
	n_command_sequence,
	n_for,
	n_if,
	n_null,
	n_pipe,
	n_simple,
	n_loop,
	n_word,
};

#define SINGLEQUOTE '\''
#define DOUBLEQUOTE '"'
#define BACKQUOTE   '`'
#define BACKSLASH   '\\'

/*
 * Context for the lexical analyzer, including a list of previous tokens.
 * Tokens are denoted by a number.  If the number is small, i.e. one of the
 * enumerated token types, we  assume the same.  Otherwise, we assume it is
 * a word token, and the value is a pointer to a dynamically allocated
 * string.
 *	Input_buf must be first to match ash PARSE definition
 */
struct YYCTX {
	char	input_buf [256];	/* line oriented input buffer	*/
	char	*filename;		/* input filename		*/
	struct FILE	*yyin;		/* current input file		*/
	int	lineno;			/* line number			*/
	int	px;			/* prompt index			*/
	char	*inp;			/* ptr into input buffer	*/
	int	tty;			/* is input channel a tty	*/
	int	debug;			/* print tokens when found	*/
	int	current;		/* current token value		*/
	int	verify;			/* print lines after reading	*/
	int	tkc;			/* number of entries in tkv	*/
	int	tka;			/* number of tokens allocated	*/
	char	**tkv;			/* array of tokens		*/
	int	yyleng;   		/* length of current token	*/
	char	yytext [512];		/* current token		*/
	char	prompts [2] [32];	/* primary and secondary prompts*/
};

/*
 * A node in the parse tree contains information that is used at parse time
 * and at execution time.
 */
struct NODE {
	struct NODE	*flink;		/* links process groups		*/
	struct NODE	*blink;
	struct SEMAPHORE done;		/* completion semaphore		*/
	int		pid;		/* of started child		*/

	int		id;		/* enumerated node type		*/
	int		slots;		/* space allocated		*/
	int		nkids;		/* number of chidren		*/
	int		**kids;		/* pointer to an array of ptrs	*/

	int		misc;		/* miscellaneous field		*/
	int		tstart;		/* index of starting token	*/
	int		tstop;		/* index of final token		*/
	
/* all following values are inherited */
	struct NODE	*header;	/* which node to link to for 	*/
					/* keeping track of child	*/
					/* processes			*/
	char	*stdin_name;
	char	stdin_mode [8];

	char	*stdout_name;
	char	stdout_mode [8];

	char	*stderr_name;
	char	stderr_mode [8];
};

/*
 * Parser context (not the parse tree itself, but variables needed to keep the
 * parser reentrant).
 *	yyctx must be first so input_buf is at beginning address so
 *	it matches the PARSE definition in ash.
 */
struct PARSE {
	struct YYCTX yyctx;	/* lexical analyzer context		*/
	int	lookahead;	/* current lookahead token		*/
	int	level;
	int	rules;		/* -r report rules as they fire		*/
	int	verify;		/* -v print lines as they are read	*/
	int	echo;		/* -x print commands, args as executed	*/
	int	ltoken;		/* -l lexical analyzer reports tokens	*/
	int	show_tree;	/* -p show parse tree			*/	
	int	delstdin;	/* -d close and delete stdin when done	*/
	int	lastpid;
	int	lastbgpid;	/* pid of last background process	*/
	int	laststatus;	/* status of last command		*/
	int	argc;		/* shell's argc				*/
	char	**argv;		/* shell's argv				*/

	struct TTPB	*ttpb;	/* valid only if isatty (stdin)		*/

	int	prec;		/* number of preallocated blocks	*/
	int	preowned [32];	/* list of pre allocated blocks		*/
	jmp_buf	jump_env;
  	int	request_exit;	/* ^x is requesting an exit		*/
	int	breakc;		/* break count (for the break command)	*/
};

/*
 * This descriptor defines dynamic strings.  A dynamic string must keep both
 * the pointer and allocation field in sync.  If no memory is alocated, BOTH
 * these fields must be 0.
 */
struct STRDYN {
	char	*ptr;	/* current pointer			*/
	int	alloc;	/* space currently allocated		*/
	int	len;	/* current size, not including null	*/
};

struct NLIST {
	int	value;
	char	*name;
	int	(*exec) ();
};

