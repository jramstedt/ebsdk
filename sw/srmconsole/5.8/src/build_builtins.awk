# file:	build_builtins.awk - generate a table of built in console commands.
#
# Abstract:	The console firmware has several dozen built in commands. 
# 	Commands are added to the firmware simply by including them in the
#	build list.  This awk program scans the sources in the build list
#	looking for command tags.  The command tag associates subroutines
#	with the actual string typed in at the console.  Additionally, the tag
#	indicates the stack size and other inode attributes of the command.
#
#	This command searchs its input for these tags, and generates a
#	file "cp$src:builtins.c" that instantiates all the built in files.
#

#
# generate a blank string such that the length of the argument string
# and the blank string are some value
#
#
function pad(s, l) {
	ps = ""
	while ((length (ps) + length (s))  < l) {
	    ps = ps " "
	}	
	return ps
}

#
# Insert a break bar between each character in a string
#
function ibreak(s) {
	splits = substr (s, 1, 1)
	for (xi=2; xi<=length(s); xi++) {
	    splits = splits "|" substr (s, xi, 1)
	}
	return splits
}
#
BEGIN {
	printf ("/* file: builtins.c - firmware built in commands */\n")
	printf ("#include	\"cp$src:platform.h\"\n")
	printf ("#include	\"cp$src:kernel_def.h\"\n")
	printf ("#include	\"cp$src:common.h\"\n")
	printf ("#include	\"cp$src:msg_def.h\"\n")
	printf ("\n")
	printf ("/*\n")
	printf (" * Define a data structure that is used to initialize the built in files on the\n")
	printf (" * ram disk.\n")
	printf (" */     \n")
	printf ("#define R	ATTR$M_READ\n")
	printf ("#define W	ATTR$M_WRITE\n")
	printf ("#define X	ATTR$M_EXECUTE\n")
	printf ("#define B	ATTR$M_BINARY\n")
	printf ("#define Z	ATTR$M_EXPAND\n")
	printf ("#define P	ATTR$M_SECURE\n")
	printf ("\n")

	n = 0.0
}
#
(FILENAME != filename_save) {
    filename_save = FILENAME
}

/COMMAND[ ]+TAG/ {

	# keep track of the different stack sizes and attributes that we
	# see.
	#
	stacksize [$5] += 1.0
	attrs [$6]++

	# generate an external name
	#
	printf ("extern %s ();\n", $4)

	# remember the command for later on
	commands [n] = $0
	n++
	if (tag [$7])
	    printf ("duplicate: %s\n", $7);
	tag [$7] = 1
}
#
END {

	printf ("struct {\n")
	printf ("	char	*name;\n")
	printf ("	void	*addr;\n")
	printf ("	int	attributes;\n")
	printf ("	int	stacksize;\n")
	printf ("} builtin_files [] = {\n")

	for (i=0; i<n; i++) {
	    nf = split (commands [i], f)
	    for (j=7; j<=nf; j++) {
		printf ("	{\"%s\",%s%s,%s%s, %s},\n", \
		    f [j], pad(f[j], 16),\
		    f [4], pad(f[4], 16),\
		    ibreak(f [6]), f [5])
	    }
	}


	#
	# terminate the structure
	#
	printf ("};\n")


	#
	# generate the routine that installs the commands
	#
	printf ("int num_builtins = sizeof (builtin_files) / sizeof (builtin_files [0]);\n");
	printf ("int install_builtin_commands () {\n")
	printf ("	int		i;\n")
	printf ("	struct INODE	*ip;\n")
	printf ("	extern struct DDB rd_ddb;\n")
	printf ("\n")
	printf ("	for (i=0; i<num_builtins; i++) {\n")
	printf ("	    allocinode (builtin_files [i].name, 1, &ip);\n")
	printf ("	    ip->dva = & rd_ddb;\n")
	printf ("	    strcpy (ip->name, builtin_files [i].name);\n")
	printf ("	    ip->attr = builtin_files [i].attributes;\n")
	printf ("	    ip->loc = builtin_files [i].addr;\n")
	printf ("	    ip->misc = builtin_files [i].stacksize;\n")
	printf ("	    INODE_UNLOCK (ip);\n")
	printf ("	}\n")
	printf ("\n")
	printf ("	return (int) msg_success;\n")
	printf ("}\n")

}
