/* file:	check.c
 *
 * Copyright (C) 1992 by
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
 *      Cobra Firmware
 *                                            
 *  MODULE DESCRIPTION:
 *
 *      Test to see if a file has certain attributes
 *
 *  AUTHORS:
 *
 *      Philippe Klein
 *
 *  CREATION DATE:
 * 
 *      08-Oct-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	phk	10-Jun-1992	Add the ! negation
 *	ajb	12-Feb-1992	Broken out from filesys.c
 *
 *--
 */
#include "cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:common.h"
#include "cp$inc:prototypes.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:parse_def.h"

/*+
 * ============================================================================
 * = check - Evaluate a string or the attributes of a inode.                  =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Check evaluates a predicate and returns the result as status.
 *      By default, check returns 'true', if the argument 's'
 *	is not a null string.
 *
 *   COMMAND FMT: check 3 0 0 check
 *
 *   COMMAND FORM:
 *  
 *	check ( [-{f,r,w,x,b}] [!] [<string>] )
 *
 *   COMMAND TAG: check 0 RXBZ check
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<string> - Specifies a string or an inode name to be checked.
 *
 *   COMMAND OPTION(S):
 *
 *      -f - Return true, if the inode exists.
 *      -r - Return true, if the inode is readable.
 *      -w - Return true, if the inode is writeable.
 *      -x - Return true, if the inode is executable.
 *      -b - Return true, if the inode is binary.
 *       ! - Return the negation of the evaluated check status.
 *
 *   COMMAND EXAMPLE(S):
 *~
 *      >>> echo > foo 'Hello World.'
 *      >>>cat foo
 *      Hello World.
 *      >>>ls -l foo
 *      rwx-   rd                 13/2048           0   foo
 *      >>>if check -f foo; then cat foo; fi
 *      Hello World.
 *      >>>if check -w foo; then cat foo; fi
 *      Hello World.
 *      >>>if check -b foo; then cat foo; fi
 *
 *      >>>rm foo
 *      >>>if check -f foo; then cat foo; fi
 *
 *      >>>if check -f ! foo; then cat foo; fi
 *      Hello World.
 *      >>>
 *~
 *   COMMAND REFERENCES:
 *
 *	None
 *
 * FORM OF CALL:
 *  
 *	check (argc, argv);
 *  
 * RETURN CODES:
 *
 *      msg_success - if true
 *      msg_failure - if false
 *       
 * ARGUMENTS:
 *
 *      int argc        - number of command line arguments
 *      char *argv []   - array of command line arguments
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

#define QF	0 /* qualifier -f */
#define QR	1 /* qualifier -r */
#define QW	2 /* qualifier -w */
#define QX	3 /* qualifier -x */
#define QB	4 /* qualifier -b */
#define QMAX	5

int check (int argc, char *argv []) {

    struct QSTRUCT qual [QMAX];
    struct INODE *ip;
    int status;
    int option;
    int negate = 0;

    status = qscan (&argc, argv,"-", "f r w x b", qual);
    if (status != msg_success) {
      err_printf (status);
      return status;
    }

   /* check the arguments */
    switch (argc) {
       case 2:
          if (strcmp(argv[1],"!") == 0)
            return msg_success;
          break;

       case 3:
          negate = (strcmp(argv[1],"!") == 0);
          break;

       default:
          return msg_failure;
    }

    option = qual[QF].present | qual[QR].present |
             qual[QW].present | qual[QX].present | 
             qual[QB].present ;

    if (!option){
        /* no file option */
        return  ((strlen(argv[argc-1]) != 0) ^ negate) ?     /* null string ?  */
                msg_success : msg_failure;
    }
  
    if (!(ip = finode (argv[argc-1], 0)))
      return negate ? msg_success  : msg_failure ; 
    else {
      if ((   (qual[QF].present)
           || (qual[QR].present && (ip->attr & ATTR$M_READ)) 
           || (qual[QW].present && (ip->attr & ATTR$M_WRITE)) 
           || (qual[QX].present && (ip->attr & ATTR$M_EXECUTE)) 
           || (qual[QB].present && (ip->attr & ATTR$M_BINARY)) 
         ) ^ negate)
            return msg_success;
        else 
            return msg_failure;
    }

}

#undef	QF
#undef	QR
#undef	QW
#undef	QX
#undef	QB
#undef	QMAX
