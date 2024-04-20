/* file:	eval.c
 *
 * Copyright (C) 1990 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      A shell command and a callable routine that evaluate postfix
 *	expressions.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      25-Sep-1990
 *
 *  MODIFICATION HISTORY:
 *
 *	ajb	31-jan-1992	Add REM operator which removes an entry from
 *				the stack.
 *
 *	ajb	17-jan-1992	change ntoi calling sequence.  Use 64 bit
 *				arithmetic on alpha.
 *
 *	cfc	26-Aug-1991	Add logical shifts. n>> <<n.  Syntax is eval 'operand shift_dist >>'
 *
 *	ajb	24-Jul-1991	Value stack overflow was detected one item too
 *				late
 *
 *	ajb	03-Jun-1991	Add the srnd operator for the FBE.
 *
 *	ajb	17-Apr-1991	Minor performance optimization.  Treat each
 *				argument on the command line as distinct
 *				expressions.
 *				
 *	ajb	25-Feb-1991	Print numbers in octal for eval
 *
 *	ajb	29-Jan-1991	Add assignment to shell variables
 *
 *	ajb	23-Jan-1991	add -r qualifier
 *
 *	ajb	17-Dec-1990	Add "if" construct
 *
 *	ajb	02-Nov-1990	Let validation routine continue on errors
 *
 *	ajb	24-Oct-1990	Print results in multiple radices
 *
 *	ajb	28-Sep-1990	add validation routines
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include "cp$inc:kernel_entry.h"

extern UINT log2 (UINT x);

struct STACK {
	UINT	value;
	char	token [MAX_NAME];
};

#include "cp$inc:prototypes.h"

/*
 * The routines that implement the operators for the evaluator
 */
UINT op_if  (struct STACK *v, INT *i) { return v [*i + 2].value ?  v [*i+1].value : v [*i].value;}

UINT op_add (struct STACK *v, UINT *i) { return v [*i].value +  v [*i+1].value;}
UINT op_sub (struct STACK *v, UINT *i) { return v [*i].value -  v [*i+1].value;}
UINT op_mul (struct STACK *v, UINT *i) { return v [*i].value *  v [*i+1].value;}
UINT op_div (struct STACK *v, UINT *i) { return v [*i].value /  v [*i+1].value;}
UINT op_rem (struct STACK *v, UINT *i) { return v [*i].value %  v [*i+1].value;}
UINT op_and (struct STACK *v, UINT *i) { return v [*i].value &  v [*i+1].value;}
UINT op_or  (struct STACK *v, UINT *i) { return v [*i].value |  v [*i+1].value;}
UINT op_xor (struct STACK *v, UINT *i) { return v [*i].value ^  v [*i+1].value;}
UINT op_lt  (struct STACK *v, UINT *i) { return (v [*i].value <  v [*i+1].value) ? 1 : 0;}
UINT op_le  (struct STACK *v, UINT *i) { return (v [*i].value <= v [*i+1].value) ? 1 : 0;}
UINT op_eq  (struct STACK *v, UINT *i) { return (v [*i].value == v [*i+1].value) ? 1 : 0;}
UINT op_ne  (struct STACK *v, UINT *i) { return (v [*i].value != v [*i+1].value) ? 1 : 0;}
UINT op_ge  (struct STACK *v, UINT *i) { return (v [*i].value >= v [*i+1].value) ? 1 : 0;}
UINT op_gt  (struct STACK *v, UINT *i) { return (v [*i].value >  v [*i+1].value) ? 1 : 0;}

UINT op_lsh (struct STACK *v, UINT *i) { return v [*i].value << v [*i+1].value;}
UINT op_rsh (struct STACK *v, UINT *i) { return v [*i].value >> v [*i+1].value;}

UINT op_log (struct STACK *v, UINT *i) { return log2 ((UINT) v [*i].value);}
UINT op_not (struct STACK *v, UINT *i) { return ~v [*i].value;}
UINT op_ent (struct STACK *v, UINT *i) {
	memcpy (& v [*i+1], & v [*i], sizeof (struct STACK));
	(*i)++;
	return v [*i].value;
}

UINT op_del (struct STACK *v, UINT *i) {
	if (*i) {
	    (*i)--;
	    return v [*i].value;
	} else {
	    return 0;
	}
}

#define SEEDCOUNT (sizeof (UINT) * 8)

UINT seeds [SEEDCOUNT];	/* a seed for each generator */

/* seed index SRND */
UINT op_srnd (struct STACK *v, UINT *i) {
	INT	index;

	index = v [*i+1].value & (SEEDCOUNT - 1);
	seeds [index] = v [*i].value;
	return seeds [index] =  random (seeds [index], index);
}

/* index SRND */
UINT op_rnd (struct STACK *v, UINT *i) {
	INT	index;


	index = v [*i].value & (SEEDCOUNT - 1);
	return seeds [index] =  random (seeds [index], index);
}

/*
 * Assign a number to an environment variable.  If the variable doesn't
 * exist, then create it.
 */
UINT op_assign (struct STACK *v, UINT *i) {
	char	buf [64];
	INT	evstat;
	struct EVNODE	*evp, evnode;
	INT	data_value;
	INT	data_type;

	/*
	 * See if the environment variable already exists
	 */
	evp = &evnode;
	evstat = ev_read (v [*i].token, &evp, EV$K_SYSTEM);

	/*
	 * inherit the attributes if the variable already exists, otherwise
	 * create it as an integer.
	 */
	if (evstat == msg_success) {
	    switch (evp->attributes & EV$M_TYPE) {
	    case EV$K_STRING:
		sprintf (buf, "%d", v [*i+1].value);
		data_type = EV$K_STRING;
		data_value = & buf [0];
		break;
	    case EV$K_INTEGER:
	    default:
		data_type = EV$K_INTEGER;
		data_value = v [*i+1].value;
		break;
	    }
	} else {
	    data_type = EV$K_INTEGER;
	    data_value = v [*i+1].value;
	}

	/*
	 * Write it back
	 */
	ev_write (v [*i].token, data_value, data_type);
	return v [*i +1].value;
}


#define	MAX_VSTACK	16	/* maximum value stack */

/*
 * Associate strings that define operators with their respective routines
 */
struct OPR {
	char	mne [MAX_NAME];
	UINT	(*value) ();
	int	opc;	/* operand count */
} operators [] = {
	{"if",	op_if,	3},

	{"srnd",op_srnd, 2},
	{"+",	op_add,	2},
	{"-",	op_sub,	2},
	{"*",	op_mul,	2},
	{"/",	op_div,	2},
	{"%",	op_rem,	2},
	{"&",	op_and,	2},
	{"|",	op_or,	2},
	{"^",	op_xor,	2},
	{"<",	op_lt,	2},
	{"<=",	op_le,	2},
	{"==",	op_eq,	2},
	{"!=",	op_ne,	2},
	{">=",	op_ge,	2},
	{">",	op_gt,	2},
	{"=",	op_assign,	2},

	{"<<",	op_lsh,	2},
	{">>",	op_rsh,	2},

	{"log",	op_log, 1},
	{"rnd",	op_rnd, 1},
	{"ent",	op_ent,	1},
	{"del",	op_del,	1},
	{"~",	op_not,	1},
	{"",	0,	0}
};

/*+
 * ============================================================================
 * = eval - Evaluate a postfix expression.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Evaluates postfix expressions.  See eval_cmd for a
 *	complete description.
 *  
 * FORM OF CALL:
 *  
 *	eval (expression, result, defradix)
 *  
 * RETURN CODES:
 *
 *	msg_success - normal completion
 *	msg_eval_underflow - value stack underflow
 *	msg_eval_overflow - value stack overflow
 *	msg_eval_values - more than one value left on stack at routine completion
 *       
 * ARGUMENTS:
 *
 * 	char *expression - string that defines the postfix expression.
 *	int *result - address of resulting number
 *	int defradix - default radix to use in conversions
 *
 * SIDE EFFECTS:                                          
 *
 *      None.
 *
-*/
int eval (char *s, UINT *result, int defradix) {
	struct STACK	vstack [MAX_VSTACK];
	UINT		vsp;	/* stack pointer */
	struct OPR	*op;
	struct EVNODE	*evp, evnode;
	int		evstat, convstat;


	vsp = 0;
	*result = 0;
	evp = &evnode;

	while (next_element (&s, " 	", & vstack [vsp].token [0])) {
	    if (vsp >= MAX_VSTACK-1) {
		return msg_eval_overflow;
	    }

	    op = sym_find_name (operators, vstack [vsp].token);

	    /*
	     * token is an operator
	     */
	    if (op) {
		if (vsp >= op->opc) {
		    vsp -= op->opc;
		    vstack [vsp].value = (*(op->value)) (&vstack, &vsp);
		    vsp++;
		} else {
		    return msg_eval_underflow;
		}

	    /*
	     * Token is an operand, and therefore is either a number
	     * or an environment variable.  Treat it as a number first,
	     * primarily for performance reasons (looking up envirnoment
	     * variables is expensive).
	     */
	    } else {
                   
		/* Try as a regular number */
		convstat = common_convert (
		    &vstack [vsp].token [0],
		    defradix,
		    &vstack [vsp].value,
		    sizeof (vstack [vsp].value)
		);
		if (convstat == msg_success) {
 		    vsp++;
                    
		/* treat as an environment variable */                  
		} else {
		    evstat = ev_read (vstack [vsp].token, &evp, EV$K_SYSTEM);
		    if (evstat != msg_success) return convstat;
		    if ((evp->attributes & EV$M_TYPE) == EV$K_INTEGER) {
			vstack [vsp++].value = evp->value.integer;
		    } else if ((evp->attributes & EV$M_TYPE) == EV$K_STRING) {
			convstat = common_convert (                                      
			    evp->value.string,
			    defradix,
			    &vstack [vsp].value,
			    sizeof (vstack [vsp].value)
			);
			vsp++;
			if (convstat != msg_success) return convstat;
		    } else {
			return convstat;
		    }
		}
	    }
	}

	/*
	 * One and only one item can be left on the stack
	 */
	if (vsp != 1) {
	    return msg_eval_values;
	} else {
	    *result = vstack [0].value;
	    return msg_success;
	}
}


#define	QIB	0
#define	QIO	1
#define	QID	2
#define	QIX	3
#define	QB	4   
#define	QO	5
#define	QD	6
#define	QX	7
#define	QMAX	8
                 


/*+
 * ============================================================================
 * = eval - Evaluate a postfix expression.                                    =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Evaluates a specified arithmetic expression and displays the results.  
 *	You can specify standard operations, such as addition, subtraction, 
 *	and bitwise operators. Eval can also substitute environment 
 *	variables. Internally, the evaluator keeps a 16 entry value stack 
 *	which bounds the complexity of the expression.
 *
 *	All arithmetic operations are done with unsigned integers.  
 *	Boolean operators (less than, greater than, etc.) always return 0 or 1.
 *	Spaces must be used to separate tokens in an expression. 
 *
 *	You must place certain operators in quotes to protect them from 
 *	the shell.
 *      
 *	The following operators are supported:
 *~
 *		+	addition
 *		-	subtraction
 *		*	multiplication
 *		/	unsigned division
 *		%	modulus
 *		&	bitwise and
 *		|	bitwise inclusive or
 *		^	bitwise exclusive or
 *		<	unsigned less than
 *		<=	unsigned less than or equal
 *		==	equal
 *		!=	not equal
 *		>=	unsigned greater than or equal
 *		>	unsigned greater than
 *		~	one's complement
 *		if	conditional operator
 *		ent	replicate top entry on value stack
 *		del	pop off top entry on value stack
 *		'='	assign a value to an environment value
 *		srnd	random number with a seed
 *		log	log base 2
 *		rnd	random number
 *		<<	shift logical left
 *		>>	shift logical right
 *		~	ones complement
 *~  
 *   COMMAND FMT: eval_cmd 2 0 6 eval
 *      
 *   COMMAND FORM:
 *  
 *	eval ( [-{ib,io,id,ix}] [-{b|d|o|x}] <postfix_expression> )
 *
 *   COMMAND TAG: eval_cmd 0 RXB eval
 *      
 *   COMMAND ARGUMENT(S):
 *
 * 	<postfix_expression> - Specifies the postfix expression to be evaluated.
 *
 *   COMMAND OPTION(S):
 *
 *	-ib	- Set default input radix to binary.
 *	-id	- Set default input radix to decimal, the default.
 *	-io	- Set default input radix to octal.
 *	-ix	- Set default input radix to hexadecimal.
 *	-b	- Print output in binary.
 *	-d	- Print output in decimal, the default.
 *	-o	- Print output in octal.
 *	-x	- Print output in hexadecimal.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>>eval 5 6 '*'
 *	30
 *	>>>eval -bodx '3 4 *'
 *	1100 14 12 C
 *	>>>eval -x '%xaa %x55 |'
 *	              FF
 *	>>>eval -ix -x 'aa 55 |'
 *	              FF
 *	>>>cat bswap
 *	for aa; do
 *	set bb %x$aa
 *	eval -ix -x \
 *	"$bb ff & 18 << $bb ff00 & 8 << $bb ff0000 & 8 >> $bb ff000000 & 18 >> | | |"
 *	done
 *	>>>create bb		# use create for AS4x00/AS8x00 only
 *	>>>bswap 01234567
 *	        67452301
 *	>>>bswap 01234567 89abcdef
 *	        67452301
 *	        EFCDAB89
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	eval_cmd (int argc, char *argv [])     
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int eval_cmd (int argc, char *argv []) {
	int	argix;
	UINT	result;
	int	status;
	struct QSTRUCT qual [QMAX];
	int	defradix;
	char	buf [512], *bp;
	int	slen;

	/*
	 * Check for radix qualifiers and apply decimal as the default if none
	 * are specified.
	 */                                    
	status = qscan (&argc, argv, "-", "ib io id ix b o d x", qual);
	if (status != msg_success) {
	    err_printf (status);
	    return status;
	}

	/*
	 * If no arguments are present return 0
	 */
	if (argc == 1) {
	    return msg_success;
	}

	/* default the input radix to decimal */
	defradix = 10;
	if (qual [QIB].present) defradix = 2;
	if (qual [QIO].present) defradix = 8;
	if (qual [QID].present) defradix = 10;
	if (qual [QIX].present) defradix = 16;

	/* default the output radix to decimal */
	if ((qual [QB].present + qual [QO].present +
	    qual [QD].present + qual [QX].present) == 0) {
	    qual [QD].present = 1;
	}

	/*
	 * Concatenate all the arguments together
	 */
	bp = buf;
	for (argix=1; argix<argc; argix++) {
	    slen = strlen (argv [argix]);
	    if ((slen + (bp - buf + 2)) >= sizeof buf) {
		err_printf (msg_overflow);
		return msg_overflow;
	    }
	    strcpy (bp, argv [argix]);
	    bp += slen;
	    *bp++ = ' ';
	    *bp = 0;
	}

	if ((status = eval (buf, &result, defradix)) == msg_success) {
	    if (qual [QB].present) printf ("%b ", (UINT) result);
	    if (qual [QO].present) printf ("%o ", (UINT) result);
	    if (qual [QD].present) printf ("%d ", (UINT) result);
	    if (qual [QX].present) printf ("%*.*X",
		(UINT) sizeof (UINT) * 2,
		(UINT) sizeof (UINT) * 2,
		result);
	    printf ("\n");
	} else {
	    printf (status);
	}

	return result;
}
