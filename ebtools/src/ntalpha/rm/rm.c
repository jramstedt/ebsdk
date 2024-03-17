/* VERY simple-minded "rm" function.  All it does is delete the
 * specified files.
 */

main(int argc, char ** argv)
{
    int		i;
    int		force=0;

    for(i = 1; i < argc; i++) {
	if(strcmp(argv[i], "-f") == 0) {
	    force=1;
	    continue;
	}
	if(_unlink(argv[i]) < 0) {
	    if (!force) {
	        perror(argv[i]);
	    }
	}
    }
}

