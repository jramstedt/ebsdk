# file:	build_depend.awk
#
# Abstract:	Build an MMS macro file that lists C include file dependencies.
#		Scan down the list of C files, picking out all the inlcude
#		files.   Files that are to be excluded from the dependency list
#		have a " /* exclude */ " comment on the same line.
#
#

BEGIN {
	skip = 0
	print "! File dependencies for the common console firmware"

	#
	# There are certain header files that we always ignore (some are
	# built automaticaly).
	#
	n = split ("stdio platform", x)
	for (i in x) {
	    exclusions [x [i]] = 1
	}
}

/^\#include/ {
	name = $2

	#
	# Ignore this inclusion if it has been tagged
	#
	if ($0 ~ /exclude/) {
	    next
	}

	#
	# Ignore it if it's on the exclusion list
	#
	if (match(name, exch)) {
#	    printf ("%s excluded\n", name)
	    next	# skip this line
	}

	#
	# Start a new macro if the filename has changed.
	#
	if (FILENAME != lastfilename) {
	    match (FILENAME, /[A-Za-z0-9_]*\./)
	    root = substr(FILENAME, RSTART, RLENGTH-1)

	    if (found [root]) {
		skip_file = 1
	    } else {
		skip_file = 0
		found [root] = 1

		print ""
		printf	"%s_depend	= -\n", root
	    }
	    lastfilename = FILENAME
	}

	#
	# If this file was already seen, don't scan it again.
	#
	if (skip_file) next

	#
	# chop off quotes if they exist
	#
	if (substr (name, 1, 1) == "\"") {
	    name = substr (name, 2, length (name) - 2)
	}

	#
	# If there is no : in the name, prepend a "sys$Library:"
	# in front
	#
	if (index (name, ":") == 0) {
	    name = "sys$library:" name
	}

	#
	# print the name
	#
	print "	" name ",-"
}

END {
	print " "
}
