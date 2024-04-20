# file: build_msg.awk
#
# Abstract:	Scan the sources for references to messages, and
#		then build a platform specific msgtxt.c and msgnum.h
#		that defines those messages and declares them external.
#
# Restrictions:
#		This convention requires that only message numbers may
#		begin with the message prefix (message_prefix).
#
#
#
#
# Find all referenced messages.  Handle the case where several
# messages may occur on the same line.
#
function collect_messages(line,      msg) {
	while (match (line, message_prefix "[a-zA-Z0-9_$]+")) {
	    msg = substr (line, RSTART, RLENGTH)
	    ref_msgs [msg]++
	    sub (message_prefix "[a-zA-Z0-9_$]+", " ", line)
	}
}

END {

	#
	# Walk down the generic source file and pick out messages that are
	# referenced
	#
	printf ("")
	printf ("/*\n")  
	printf (" * Message definitions\n")
	printf (" */\n") 

	while (getline < message_file > 0) {
	    if (match ($0, message_prefix "[a-zA-Z0-9_$]+")) {
		msg = substr ($0, RSTART, RLENGTH)
		def_msgs [msg]++
		if (msg in ref_msgs) {
		    res_msgs [msg]++
		    print $0 
		}
	    }
	}

	#
	# Complain about messages that were referenced and not
	# defined.
	#
	ind[0] = 0
	nmsg = isort(ref_msgs, ind)
	for (i = 0; i < nmsg; i++) {
	    msg = ind[i]

	    if (msg in def_msgs) {
		;
	    } else {
		if (msg in message_values) {
		    ;
		} else {
		    printf ("/* %s is referenced and not defined */\n", msg)
		}
	    }
	}
}
