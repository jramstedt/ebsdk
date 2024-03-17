#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

/* very simple-minded version of the UNIX "chmod" utility */
main(int argc, char ** argv)
{
    int mode, umode;
    int i;

    if(argc != 3) {
	printf("Usage: %s <mode> <file>\n", argv[0]);
	exit(-1);
    }

    if(strlen(argv[1]) != 3) {
	printf("Usage: %s <mode> <file>\n", argv[0]);
	exit(-1);
    }

    umode = 0;
    for(i = 0; i < 3; i++) {
	if(!isdigit(argv[1][i])) {
	    printf("%s: Only absolute mode supported\n", argv[0]);
	}
	umode |= ((argv[1][i]) - '0');
    }

    mode = 0;
    if(umode & 4) {
	mode |= _S_IREAD;
    }
    if(umode & 2) {
	mode |= _S_IWRITE;
    }

    if(_chmod(argv[2], mode) < 0) {
	perror(argv[2]);
	exit(-1);
    }
    exit(0);
}

