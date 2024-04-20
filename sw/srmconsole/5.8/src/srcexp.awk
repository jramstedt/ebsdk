# file:	srcexp.awk
#
# +-----------------------------------------------------------------------+
# | Copyright (C) 1993 by						  |
# | Digital Equipment Corporation, Maynard, Massachusetts.		  |
# | All rights reserved.						  |
# |									  |
# | This software is furnished under a license and may be used and copied |
# | only  in  accordance  of  the  terms  of  such  license  and with the |
# | inclusion of the above copyright notice. This software or  any  other |
# | copies thereof may not be provided or otherwise made available to any |
# | other person.  No title to and  ownership of the  software is  hereby |
# | transferred.							  |
# |									  |
# | The information in this software is  subject to change without notice |
# | and  should  not  be  construed  as a commitment by digital equipment |
# | corporation.							  |
# |									  |
# | Digital assumes no responsibility for the use  or  reliability of its |
# | software on equipment which is not supplied by digital.		  |
# +-----------------------------------------------------------------------+
#
#++
# FACILITY:
#
#	Common Console Development Environment
#
# MODULE DESCRIPTION:
#
#	Expand the file name for a file containing a list of source modules
#	into a list of input files.  Used for running a gawk script on a
#	list of files, such as SOURCES.TXT.
#
#	The list may be tailored by setting the regular expression variables
#	inc and exc.
#
#     Usage:
#
#	gawk -f srcexp "inc=\.c" "exc=^foo$" <input_file> > <output_file> 
#
#  AUTHORS:
#
#	Peter H. Smith
#
#  CREATION DATE:
#
#	13-JUL-1993
#
#  MODIFICATION HISTORY:
#
#	dtr	31-Mar-1994	Remove "then" from lines 59 and 60.  This allows
#				this procedure to be used with the old and new
#				versions of gawk.exe
#	phs	22-Jul-1993	Allow "!" as comment delimiter, too.
#	phs	21-Jul-1993	Keep track of infiles, list_files.
#	phs	13-JUL-1993	Initial revision.
#--
#
BEGIN {

    # Use inc and exc to control which files to include/exclude.
    # if (inc == "") then inc = ".*"
    # if (exc == "") then exc = "^$"
    # removed the "then" from the lines so the old and new gawk could be used.
    if (inc == "") inc = ".*"
    if (exc == "") exc = "^$"

    # Put a marker in the argument list to tell when to switch from building
    # the list to parsing the files.
    building_list = 1
    ARGV[ARGC++] = "building_list=0"
}

END {

    # Trim the leading comma off the list of input files.
    list_files = substr(list_files, 2)
}

# Accumulate the list of input files.
building_list && FILENAME != infile_save {

    # Make the comparisons case-insensitive for VMS.  Need to do this here
    # rather than in the BEGIN statement, since the assignments occur before
    # a filename in the parameter list, and don't take effect until that
    # file is being read.

    if (infiles == "") {
        inc = tolower(inc)
	exc = tolower(exc)
    }

    # Collect list of input files.

    infile_save = FILENAME
    infiles = infiles ", " FILENAME

    if (dbg) printf("inc = \"%s\", exc = \"%s\"\n", inc, exc) > "/dev/stderr"
}

# While building the list, parse input files and add each line to the
# command line.

building_list {

    file = $1

    # Make comparisons case-insensitive for VMS.
    file = tolower(file)

    # Skip comments and blank lines
    if ($0 == "") next
    if (substr(file, 1, 1) == "#") next
    if (substr(file, 1, 1) == "!") next

    # Decide whether the file should go in the list.
    if (dbg) {
	printf("file: %s %d %d\n",
	       file, match(file, inc), match(file, exc)) > "/dev/stderr"
    }
    if (!match(file, inc) || match(file, exc)) next

    # Put the file in the list
    if (dbg) printf("ARGV[%d] = \"%s\"\n", ARGC, file) > "/dev/stderr"
    list_files = list_files ", " file
    ARGV[ARGC++] = file
    next
}
