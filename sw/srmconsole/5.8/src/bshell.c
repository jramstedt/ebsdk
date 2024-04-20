/* file:	bshell.c
 *
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
 *      Bourne shell support routines
 *
 *  AUTHORS:
 *
 *      J. Kirchoff
 *
 *  CREATION DATE:
 *  
 *      29-Sep-1993
 *
 *--
 */

#include	"cp$src:platform.h"
#include "cp$src:common.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:tt_def.h"
#include	"cp$src:blex.h"
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

extern int advanced_mode_flag;
extern int diagnostic_mode_flag;

int shell_routine();
int shell_startup();

int pipe_flag;

/*
 * If the value below is changed, make sure the command tag for builtins
 * is also changed
 */
int shell_stack = 1024 * 12;

#if !MODULAR
extern sh_routine();
#endif

extern struct LOCK spl_kernel;

/*
 * Following are the legal characters that may be in a name, in a format
 * suitable for use by strchr.
 */
unsigned char idset [] = {
'_',
'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

/* iso-latin */
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,

0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,

0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,

0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,

0x00	/* terminator */
};

/*+
 * ============================================================================
 * = sh - Create a new shell process.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The sh command creates another shell process.
 *	This routine does a dispatch to the real sh routine.
 *  
 * FORM OF CALL:
 *  
 *	sh ( argc, *argv[] )
 *  
 * RETURN CODES:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#if MODULAR
int sh(int argc, char *argv [])
{
    struct PCB *pcb;
    int status;

    pcb = getpcb();

    while (1) {

	status = sh_routine(argc, argv);

	switch (pcb->pcb$l_cmd_switch) {

	case 0: 			/* do nothing */
	    return status;
	    break;

	case 1:				/* switch to adv shell */
	    pcb->pcb$l_cmd_switch = 0;
	    break;

	case 2:				/* stay with adv shell */
	    return status;
	    break;
	}
    }
    return status;
}

/* all others only have advanced mode */
#if !TURBO
int ash_sh_routine()
{
    return 0;
}
#endif

int sh_routine(int argc, char *argv [])
{
    int status;

    if (advanced_mode_flag == 0)		/* BASIC mode */
	return ash_sh_routine(argc, argv);

    if (advanced_mode_flag) {

	if (!diagnostic_mode_flag)	/* ADVANCED mode */
	    return adv_sh_routine(argc, argv);
	else				/* DIAG mode */
	    return adv_sh_routine(argc, argv);
    }
    return msg_success;
}
#endif

#if !MODULAR
int sh (int argc, char *argv [])
{
    return sh_routine(argc, argv);
}
#endif

/*+
 * ============================================================================
 * = shell_cmd - Execute a shell command from a program.     	              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Allows shell commands to be called under program control.
 *      Returns after starting a shell process to execute the command string
 *      argument to shell_cmd.
 *                                      
 *	It is up to the caller to decide to wait for the spawned shell process
 *	to complete or not.  If the caller wants to wait it must create a
 *	process completion semaphore, pass the address of that semaphore to 
 *	shell_cmd, wait on that semaphore, and release it after it
 *	isn't needed.
 *  
 * FORM OF CALL:
 *  
 *	shell_cmd ( cmd, &sem )
 *  
 * RETURNS:
 *
 *	PID of process created if success
 *       
 *	NUL if error
 *       
 * ARGUMENTS:
 *
 *	char *cmd - any legal shell command string terminated w/ CR.
 *		For example, shell_cmd( "ls -l | grep \n");
 *
 *	struct SEMAPHORE *sem - address of a process completion semaphore or
 *		NULL.
 *
 *		For example, shell_cmd( "ls -l | grep \n", &sem);
 *
 * SIDE EFFECTS:
 *
 *	Starts up another shell to run the specified command.
 *
-*/

int shell_cmd(char *cmd, struct SEMAPHORE *sem)
{
    struct FILE *cmd_fp;
    char cmd_line_file[MAX_NAME];
    int argc;
    char **argv;
    struct PCB *pcb;

    krn$_unique("cmd_", cmd_line_file);
    if ((cmd_fp = fopen(cmd_line_file, "w")) == 0) {
	return 0;
    }
    fprintf(cmd_fp, "%s\n", cmd);
    fclose(cmd_fp);
    argv = malloc(4 * sizeof(char *));
    argv[0] = mk_dynamic("sh");
    argv[1] = mk_dynamic("-shellquals");
    argv[2] = mk_dynamic("-d");
    argv[3] = 0;
    argc = 3;
    pcb = getpcb();

    krn$_create(			/* */
      sh_routine, 			/* routine to start     */
      shell_startup, 			/* startup routine      */
      sem, 				/* completion semaphore or NULL */
      2, 				/* priority             */
      pcb->pcb$l_affinity, 		/* parent's affinity    */
      shell_stack, 			/* stack size           */
      "shell_cmd", 			/* process name         */
      cmd_line_file, "r", 		/* stdin                */
      0, 0, 				/* parent's stdout      */
      0, 0, 				/* parent's stderr      */
      argc, argv);

    return msg_success;
}

/*+
 * =============================================================================
 * = shell_startup - Startup routine executed when a shell process is invoked. =
 * =============================================================================
 *
 * OVERVIEW:
 *  
 *  Changes ownership of the ARGV array passed to a process, so that the
 *  array automatically goes away when the process is run down.  This routine
 *  called in process startup in process context.
 *  
 * FORM OF CALL:
 *  
 *	shell_startup ( int argc, char **argv )
 *  
 * RETURN CODES:
 *
 *	None
 *       
 * ARGUMENTS:
 *
 *	int argc - number of command line arguments passed by the shell
 *	char *argv [] - array of pointers to arguments,
 *			the command arguments are process IDs.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int shell_startup (int argc, char **argv)
{
    int i;
    struct PCB *pcb;
    struct PCB *parent_pcb;

    pcb = getpcb();
    parent_pcb = krn$_findpcb(pcb->pcb$l_parentpid);
    spinunlock (&spl_kernel);

    dyn$_chown (argv);
    for (i=0; i<argc; i++) {
	dyn$_chown (argv [i]);
    }

    return msg_success;
}

/*
 * Set a bit so that the shell will exit after the current command is complete.
 * This is used by the control X handler and the EXIT command.
 */
void sh_request_exit (struct PARSE *p)
{
    p->request_exit = 1;
}

/*
 * register a process with the control C handler associated with
 * that process' stdin
 */

int controlc_register (int pid)
{
    struct PCB *pcb;
    struct TTPB	*ttpb;	/* valid only if isatty (stdin)		*/

/* find the process first */

    if (pcb = krn$_findpcb (pid)) {

/* we can only register if stdin is a tty */

	if (isatty (pcb->pcb$a_stdin)) {
	    ttpb = pcb->pcb$a_stdin->ip->misc;
	    insq (&pcb->pcb$r_ctrlcq, ttpb->ctrlch.blink);
	    spinunlock (&spl_kernel);
	    return msg_success;

/* otherwise we fail */

	} else {

	    spinunlock (&spl_kernel);
	    return msg_failure;
	}
    }

/* not found, return failure */

    return msg_failure;
}

/*
 * Take a statically allocated string and clone it into a buffer that has
 * been dynamically allocated, and return the address of the new buffer.
 */

int mk_dynamic (char *s)
{
    return strcpy (malloc (strlen (s) + 1), s);
}
