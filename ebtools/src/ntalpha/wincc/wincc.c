/* Front-end for the "CL" command-line C compiler from Microsoft
 * Visual Studio.  This front end provides a somewhat more UNIX-like
 * look and feel...
 *
 * Transforms performed:
 *	Convert "-o <file>" to /Fo<file>
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <process.h>
#include <string.h>

static int	newargc;
static char **	newargv;
static int	maxargs = 0;

static void newargs_init(int argc)
{
    newargc = 0;
    newargv = (char **)malloc(2 * argc * sizeof(char *));
    maxargs = 2 * argc;
}

static void add_arg(char * arg)
{
    if (newargc >= maxargs) {
	/* Double the number of arguments... */
	newargv = realloc(newargv, 2 * maxargs * sizeof(char *));
	maxargs *= 2;
    }


    newargv[newargc++] = arg;
}

static char** finish_args()
{
    newargv[newargc] = (char *)0;
    return(newargv);
}
main(int argc, char ** argv)
{
    int i;
    char * argbuf;

    newargs_init(argc);

    for(i = 0; i < argc; i++) {
	if(strcmp(argv[i], "-o") == 0) {
	    if((i+1) == argc) {
		fprintf(stderr, "wincc: -o requires an argument\n");
		exit(-1);
	    }
	    argbuf = malloc(strlen(argv[++i]) + 4);
	    strcpy(argbuf, "/Fo");
	    strcat(argbuf, argv[i]);
	    add_arg(argbuf);
	}
	else {
	    add_arg(argv[i]);
	}
    }

    if(_spawnvp(_P_WAIT, "CL", finish_args()) == -1) {
	perror("wincc");
	exit(-1);
    }
}

