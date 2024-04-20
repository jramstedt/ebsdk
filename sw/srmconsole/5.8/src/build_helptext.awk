# file: build_helptext.awk
#
# abstract:
#
#	Create an online help C text file (HELP_TEXT.H).
#
#	This awk program creates the help text data structure, helpbuf []:
#
#	struct { char *topic;
#		 char *text;
#		 char *syn;
#	       } helpbuf [] = {
#                                { topic_001, text_001, syn_001 },
#				 ...
#                                { topic_nnn, text_nnn, syn_nnn }
#			      }
#
#	The awk program scans the source files for shell command headers.
#	It grabs any routine topic line, then if it finds a COMMNAND FORM line
#	all the command form lines are saved.  If a subsequent COMMAND TAG line
#	is found, the topic count is incremented and the following table
#	entries are created:
#
#		char topic_nnn [] = { "command" };
#		char  text_nnn [] = { "command one line description" };
#		char   syn_nnn [] = { "command form lines" };
#
#	Note that since the input file list determines what will be built
#	into the online help.  Only help for builtin commands is included.
#
#  Revision History:
#
#     al   15-dec-1995     Have gone back to the original with one mod,
#                          which is to make one check for number of fields,
#                          so that there is some constraint on looping.
#
#     al   14-sep-1995     Initialize cmd_desc between files.
#
#     al   13-sep-1995     Change search for topic header so it can't hang
#                          forever.  Don't search on "-" and ".", just
#                          concatenate fields to build topic header.  This
#                          isn't perfect, but hopefully it will clear up
#                          some problems.  There's still stuff that's
#                          hardcoded, but I don't imagine we'll be able to
#                          change it anytime soon, since this whole thing
#                          is rather arbitrary.
#
#
#

BEGIN   {
	topic_count = 0                 # Initialize the topic count.
	topic[0, 1] = "zzzzzzzz"	# Create a sort terminator string..
	sort[1] = 0			# ..and point first sort index to it.
	ignore = 1  			# Start out ignoring lines.
	}
#
#	Skip dead code.
#
($1 == "#if" && $2 == "0"), $1 == "#endif" {
	next
	}
#
#	Look for header sections only, ignore all else (this speeds things up).
#
$1 == "/*+" || $1 == "#+" {		# Found a routine header, start looking.
	ignore = 0
	next
	}
$1 == "-*/" || $1 == "#-" {		# Found end of header, ignore the rest.
	ignore = 1
	next
	}
ignore == 1 {                           # Skip lines, til next header.
	next
	}
#
# Save any topic line in a header, just in case this is a command.
#
($1 == "*" || $1 == "#") && $2 == "="  {
	# printf "found equals sign %s %s\n", $1, $2
	cmd_line = $0;  name=$3;  i=4;
	while ($i != "-" && i <= NF){ name=name " " $i; i++ }
	nlen = length(name)
	cmd_desc = substr( $0, dlen=index($0,"-")+2, index($0,".")-dlen+1 )
	next
	}
#
# Look for a COMMAND FORM section in the header.
#
/COMMAND[ ]+FORM/ {			# Found COMMAND FORM.
	# printf "found form %s %s\n", $1, $2
	form_lines = 0
	form = 1 
	next
	}
form != 0 {				# Save COMMAND FORM lines for help.
	# printf "found form line %s %s\n", $1, $2
	if (form == 2 ) {
	        form_lines++;
		topic[topic_count+1, form_lines+3] = $0
		if ( index($0,")") ) { form = 0 }
		}
	else
		form += 1
	}
#
# Close the COMMAND FORM, if we find a COMMAND TAG or COMMAND ARGUMENT(S).
# Then build a help topic descriptor with the form_lines saved.
#
/COMMAND[ ]+TAG/ || /COMMAND[ ]+ARGUMENT/ {   
	# printf "found end of form %s %s\n", $1, $2
	topic_count += 1
	topic[topic_count, 1] = name
	topic[topic_count, 2] = cmd_desc
	topic[topic_count, 3] = form_lines
	#
	# Sort the help text alphabetically using insertion.
	#
	# printf "start pass %02d\n", topic_count
	j = topic_count
	while ( (topic[sort[j],1] > name) && (j > 0) ) {
	    sort[j+1] = sort[j]
	    # printf "debug %2d - %s - %s\n", j, name, topic[sort[j], 1]
	    sort[j] = topic_count
	    j = j - 1
	    }
	ignore = 1
	next
	}
#
# Done searching files, so print the table of help strings.
#
END {
	infiles = substr(infiles, 3) # Get rid of leading ", "

	print "/* This file contains the console help text data structures."
	print " * It is arranged as an array of topic text strings,"
	print " * referenced through pointer triples in the array helpbuf."
	print " */"

	for ( tc=1; tc<=topic_count; tc++) {
		print ""
		printf "char topic_%03d [] = { \"%s\" };\n", tc, topic[tc,1]
		printf "char  text_%03d [] = { \"%s\" };\n", tc, topic[tc,2]
		printf "char   syn_%03d [] = { \"", tc
		for (i=1; i<=topic[tc,3]; i++) {
		    if ( i > 1 ) {
			printf "\\n\\\n\\t"
			for (k=1; k<=nlen+2; k++) printf " "
			}
		    form_words = split(topic[tc,i+3],form_word," ")
		    for (j=2; j<=form_words; j++)
			if ( form_word[j] != "(" && form_word[j] != ")" )
			    printf "%s ", form_word[j]
		    }
		printf "\" };\n"
	    }
	print ""		
	print ""
	#
	# Print help string list terminator.
	#
	topic_count += 1
	printf "char topic_%03d [] = { \"\" };\n", topic_count
	printf "char  text_%03d [] = { \"\" };\n", topic_count
	printf "char   syn_%03d [] = { \"\" };\n", topic_count
	print ""
	print ""
	#
	# Print the sorted help message descriptor table, helpbuf.
	#
	print "struct {"
	print "        char *topic;"
	print "        char *text;"
	print "        char *syn;"
	print "} helpbuf [] = {"
	for (i=1; i<topic_count; i++) {
	       	printf "    { topic_%03d, text_%03d, syn_%03d },\n", sort[i], sort[i], sort[i]
		}
	i = topic_count
	printf "    { topic_%03d, text_%03d, syn_%03d },\n", i, i, i
	printf "    { 0, 0, 0 }\n"
	print "};"
	}
