# file: build_msg.awk
#
# Abstract:	Scan msgtxt.c for defined messages, and output a header
#		file which declares those messages external.  Also output
#		#define statements for special message values.
#		
#
function collect_messages(line) {
    if (substr(line, 1, 4) == "char") {
	while (match (line, message_prefix "[a-zA-Z0-9_$]+")) {
	    msgs[msg++] = substr(line, RSTART, RLENGTH)
	    sub (message_prefix "[a-zA-Z0-9_$]+", " ", line)
        }
    }
}

FILENAME != lastfilename {
	infiles = ", " FILENAME
	lastfilename = FILENAME
}

END {
	infiles = substr(infiles, 3)

	#
	# Generate the header file of all messages that were referenced and
	# defined
	#
	print
	printf ("#ifndef _msgnum_\n")
	printf ("#define _msgnum_\n")
	printf ("/*\n")
	printf (" * External message declarations\n")
	printf (" */\n")
	for (i = 0; i < msg; i++) {
	    printf ("extern char %s [];\n", msgs[i])
	}

	#
	# The following list of exclusions are names that are not to be
	# treated as message numbers, even though they look like the same.
	#
	ind[0] = 0
	nmsg = isort(message_name, ind)
	for (i = 0; i < nmsg; i++) {
	    printf("#define %s %s\n", message_name[ind[i]], ind[i])
	}
	printf ("#endif\n")
}
