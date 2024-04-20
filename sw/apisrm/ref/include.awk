# file: include.awk
#
# Expand the configuration file's include facility.  Look for all include
# commands, and expand that inclusion.  Write a results file that has no
# inclusions.
#
BEGIN {

   depth = 0

   while(depth >= 0) {
	if (depth == 0) {
	    more = getline line
	} else {
	    more = getline line < file[depth]
	}
	
	if (more) {
	    split(line, f)
	    if (!in_c_header && f[1] == "c_header") {
		in_c_header = 1
		print line
	    } else if (!in_c_header && f[1] == "endc_header") {
		in_c_header = 0
		print line
	    } else if (f[1] == "include") {
		depth++
		file[depth] = f[2]
		printf("# start include of %s (depth %d)\n",
		       file[depth], depth)
	    } else print line
	} else {
	    printf("# end include of %s (depth %d)\n", file[depth], depth)
	    depth--
	}
    }
}
