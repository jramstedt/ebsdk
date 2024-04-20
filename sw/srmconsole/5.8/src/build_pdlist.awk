# file:	build_pdlist.awk
#
# Abstract:
#	Build the structure PDLIST from the link map.  If the link map doesn't
#	exist, or if the OPTIONAL flag is not set, then build a null list. 
#	The PDLIST is essentially a massaged symbol table.
#
# Usage:
#	awk -f build_pdlist.awk mapfile [outputfile]

BEGIN {
	infile = ARGV[1]

	psect = 0
}

/Program Section Synopsis/ {
	psect = 1
}
/Symbols By Name/ {
	psect = 0
}

#
# This pattern picks off linkage psects
#
/^[0-9_A-Z\$]+ +([0-9A-F]+ +)+\(/ && ($1 ~ /LINKAGE/) {
	psects [$2 " " $3 " " $1]++
}

#
# This pattern picks off all psects
#
/^[0-9_A-Z\$]+ +([0-9A-F]+ +)+\(/ {
	all_psects [$2 " " $3 " " $1]++
}


#
# This pattern picks off variable names
#
/^[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][ 	]+R-/ {
	name = substr ($2, 3)

	#
	# check to make sure name is in a code psect
	#
	inrange = 0
        for (i in psects) {
	    split (i, f)
	    if ((f[1] <= $1)  &&  ($1 <= f[2])) inrange = 1
	}
	if (!inrange) {
#	    printf ("%s excluded\n", name)
	} else {
	    if ("PDLIST" != name) {
		names [n++] = name
	    }
	}

}

END {
	infiles = substr(infiles, 2)

	printf ("/* file: pdlist.c -- procedure descriptor list */\n")
	printf ("#include	\"cp$src:platform.h\"\n")
	printf ("#include	\"cp$src:pd_def.h\"\n")
	printf ("\n")

	#
	# declare everything external
	#
	for (i=0; i<n ;i++) {
	    printf "extern int	%s ();\n", names [i]
	}
	printf ("\n")

	printf ("struct PDLIST pdlist [] = {\n")
	printf ("#if EXTRA\n")

	#
	# Define the structure
	#
	for (i=0; i<n; i++) {
	    printf "	{%s,	\"%s\"},\n", names [i], names [i]
	}

	#
	# terminate the structure
	#
	printf ("#endif\n")
	printf ("	{0, \"\"}\n")
	printf ("};\n")

	for (i in psects) {
	    printf ("/* %s */\n", i)
	}

	for (i in all_psects) {
	    printf ("/* %s */\n", i)
	}
}
