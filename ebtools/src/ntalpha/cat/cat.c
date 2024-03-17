/* VERY simple-minded "cat" utility.  No fancy options, all it does
 * is spew the specified files and/or stdin to stdout.
 */
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

main(int argc, char ** argv)
{
    int		i;
    char	buf[512];
    int		in;
    int		count;

    /* Set "stdout" in binary mode */
    _setmode(_fileno(stdout), _O_BINARY);
    if(argc == 1) {
	/* Set "stdin" in binary mode */
	_setmode(_fileno(stdin), _O_BINARY);
	while( ( count=_read(_fileno(stdin), buf, 512) ) > 0) {
	    _write(_fileno(stdout), buf, count);
	}
	exit(0);
    }


    for(i = 1; i < argc; i++) {
	in = _open(argv[i], _O_RDONLY | _O_BINARY);
	if(in < 0) {
	    perror(argv[i]);
	    exit(0);
	}
	while( ( count=_read(in, buf, 512) ) > 0) {
	    _write(_fileno(stdout), buf, count);
	}
	_close(in);
    }
    exit(0);
}

