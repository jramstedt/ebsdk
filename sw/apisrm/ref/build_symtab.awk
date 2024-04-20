# file: build_symtab.awk
#
# Abstract:

#	Build a symbol table for the sym_driver in the firmware.  The build
#	file has commands that allow for symbol inclusion/exclusion so that
#	custom (and small) symbol tables may be built.  Exclusions/inclusions 
#	take the form of regular expressions (awk style: suitable for use
#	by match).
#
#	In the case where a symbol matches both and exclusions and inclusions
#	the inclusion takes priority.
#
#	The program gets the symbols from a file that is built by an obj
#	scanner.
#
# Usage:
#	gawk -f cp$src:build_symtab.awk symbol_file >outputfile
#	gawk -f cp$src:build_symtab.awk -f cp$cfg:sym_xxclusions.dat cp$src:ref_symbols.txt >cp$src:vlist.c
#

# generate a blank string such that the length of the argument string
# and the blank string are some value
#
function pad(s, l) {
	ps = ""
	while ((length (ps) + length (s))  < l) {
	    ps = ps " "
	}	
	return ps
}
#
BEGIN {
	printf ("/* file: vlist.c - firmware symbols */\n\n")
	name_count = 0
	char_count = 0
}

{
	for (i=1; i<=NF; i += 2) {

# apply the inclusions

	    add = 0
	    for (j in inclusions) {
		if (match ($i, j)) {
		    if (RLENGTH == length($i)) {
			add = 1
		    }
		}
	    }

# apply the exclusions

	    for (j in exclusions) {
		if (match ($i, j)) {
		    if (RLENGTH == length($i)) {
		        add = 0
		    }
		}
	    }

	    if (add) {
		names[name_count] = $0
		name_count++
	        char_count += length($0) + 1
	    }
	}
}

END {
	printf ("/* internal symbol table built from %s \n\n", FILENAME)

	printf (" Inclusions:\n")
	for (i in inclusions) {
	    printf ("	%s\n", i)
	}
	printf (" Exclusions:\n")
	for (i in exclusions) {
	    printf ("	%s\n", i)
	}
	printf (" */\n")

	printf ("#include \"cp$src:platform.h\"\n")
	printf ("#include \"cp$src:sym_def.h\"\n")
	printf ("\n")

	i = 0
	while (i < name_count) {
		printf ("extern %s ();\n", names[i])
		i++
	}

	printf ("\n")
	printf ("struct SYM vlist [] = {\n")
	printf ("#if SYMBOLS\n")

	i = 0
	while (i < name_count) {
	  printf ("  {%s,%s\"%s\"},\n", names[i], pad(names[i], 30), names[i])
	  i++
	}

	printf ("#endif\n")
	printf ("  {0,0}\n")
	printf ("};\n")

	printf ("\n")
	printf ("/* %5d symbols %d characters */\n", name_count, char_count)
}
