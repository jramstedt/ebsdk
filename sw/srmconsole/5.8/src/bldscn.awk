# file:	bldscn.awk
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
#	Parse the build list into several data structures.
#
#	This module is the input phase for a series of filters which
#	generate formatted lists of files based on a build list.
#
#     Format:
#
#	The build list is small keyword driven language.  Keywords
#	such as "file", "include", "driver", "symbol", "encapsulate"
#	"c_header", and "endc_header" are currently recognized commands.
#
#       Lines that don't have these keywords are ignored (and may used as
#	comments).  A # insures that the line will forever be recognized 
#	as a comment.  Trailing comments are not generally supported.
#
#
#     Usage:
#
#	gawk -f bldscn -f <output_phase> <input_file> > <output_file>
#
#
#     Ordering:
#
#	To facilitate taking differences between generated files, various
#	arrays built by bldscn.awk are indexed by a number.  This forces
#	these arrays to be ordered by the lexical order of the statements
#	in the input language.  This is cheaper than sorting the arrays.
#
#  AUTHORS:
#
#	Peter H. Smith, based on A.J. Beaverson's build_generic.awk
#
#  CREATION DATE:
#
#	07-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	21-Jul-1993	Add message processing.
#	phs	20-Jul-1993	Clean up source file naming.
#	phs	12-Jul-1993	Fix up handling of options.
#	phs	06-Jul-1993	Add "delete file" and "define option"
#				statements, to enhance configurability and
#				use of include statements.
#	phs	07-Jun-1993	Initial revision, based on build_generic.awk.
#--
#
#
# Initialization
#
BEGIN {
    # Keep track of list of input files which were parsed.
    infiles = ""
    infile_save = ""

    # names[fn] is the list of base names of all files encountered in
    # "file" or "driver" statements.
    fn = 0		# number of filenames in names

    # sfiles[sn] is list of all source file names from "file" or "driver"
    # statements.
    sn = 0		# number of filenames in sfiles

    # basename_to_name[basename] is an array, indexed by file basename,
    # of the full file name for the given basename.  This is used to
    # detect naming conflicts.

    # deleted[basename] is an array, indexed by file basename, of the
    # full file name for a deleted file.  This is used to keep track
    # of which files have been listed in "delete file" statements.

    # filecount[basename] is an array, indexed by file basename, of the
    # count of inclusions versus deletions of the file.  If the count is
    # positive, the file should be emitted.

    # basename_to_filename[basename] is an array, indexed by file basename,
    # containing the full filename last seen for that basename.  Configurations
    # may not build right if two different full filenames have the same
    # basename.

    # group[basename] is an array, indexed by source file basename, of
    # the group which the source file belongs to.

    # gfiles[gn] is an array of names of elements in a group.
    # gnames[gn] is an array of group names.
    # gind[grpname] is an array, indexed by group name, which in turn indexes
    # the gfiles and gnames arrays.
    gn = 0			# number of groups

    # c_header[clines] is the list of all lines between a "c_header" and
    # "endc_header" statement in the build file.
    clines = 0			# Number of c header lines

    # sym_inclusions[regexpstr] holds counts of occurrences of each regular
    # expression encountered in a sym_include statement.

    # sym_exclusions[regexpstr] holds counts of occurrences of each regular
    # expression encountered in a sym_exclude statement.

    # dnames[dn] is the list of all driver name prefixes encountered in
    # the "driver" directive.  The driver name prefix is everything up to
    # the first underscore (naming convention is foo_driver.c).
    # dphase[dn] is the driver phase number for driver dnames[dn].
    dn = 0		# number of drivers

    # enfiles[en] is the set of encapsulation files.
    # The data stored in each entry consists of source file name, 
    # destination file name, third and fourth parameter separated by spaces.
    en = 0		# number of enfiles

    # options[<optname>] has a value for each option which appeared in
    # an "option[s]" statement.
    # option_defined[<optname>] has an entry for each option which appeared
    # in a "define option" statement.
    # Warnings are emitted for options which were not defined.

    # values[<value names>] is the list of all values assigned in "value"
    # statements.

    groups_exist = 0 # Set to 1 if a group specifier is encountered.


    #
    # used for hex to decimal conversions
    #
    hexdigits ["0"] = 0
    hexdigits ["1"] = 1
    hexdigits ["2"] = 2
    hexdigits ["3"] = 3
    hexdigits ["4"] = 4
    hexdigits ["5"] = 5
    hexdigits ["6"] = 6
    hexdigits ["7"] = 7
    hexdigits ["8"] = 8
    hexdigits ["9"] = 9
    hexdigits ["A"] = 10
    hexdigits ["B"] = 11
    hexdigits ["C"] = 12
    hexdigits ["D"] = 13
    hexdigits ["E"] = 14
    hexdigits ["F"] = 15
    hexdigits ["a"] = 10
    hexdigits ["b"] = 11
    hexdigits ["c"] = 12
    hexdigits ["d"] = 13
    hexdigits ["e"] = 14
    hexdigits ["f"] = 15

}
#
#
# Post-Processing:  Check for errors and remove deleted files.
#
END {
    infiles = substr(infiles, 3) # Get rid of leading ", "
    groups_exist = (gn > 0)

    if (arch == "" && !nowarn) {
	printf("warning: arch is undefined\n") > "/dev/stderr"
    }        
    if (platform == "" && !nowarn) {
	printf("warning: platform is undefined\n") > "/dev/stderr"
    }        

    #
    # Run through the names array, compacting out deleted files.
    # Need to have spaces at both ends of each gfiles list for deletion of
    # groups to work right.
    #
    for (i = 0; i < gn; i++) gfiles[i] = gfiles[i] " "
    nfn = 0
    for (i = 0; i < fn; i++) {
	if (filecount[names[i]] <= 0) {
	    if ((basename_to_filename[names[i]] != deleted[names[i]]) \
	        && !nowarn) {
		printf("warning: \"delete %s\" doesn't match \"file %s\"\n" \
		       "         will not delete %s\n",
		       deleted[names[i]], basename_to_filename[names[i]],
		       deleted[names[i]]) > "/dev/stderr"
#		if (dbg) printf("names[%d] = names[%d] = %s\n",
#			        nfn, i, names[i]) > "/dev/stderr"
		if (nfn != i) names[nfn] = names[i]
		nfn++
	    } else {
#		if (dbg) printf("deleting names[%d] = %s\n",
#				i, names[i]) > "/dev/stderr"
		if (names[i] in group) {
		    sub(" " names[i] " ", " ", gfiles[gind[group[names[i]]]])
		    if (gfiles[gind[group[names[i]]]] == " ") {
			delete gfiles[gind[group[names[i]]]]
		    }
		    delete group[names[i]]
		}
	    }
	} else {
#	    if (dbg) printf("names[%d] = names[%d] = %s\n",
#			    nfn, i, names[i]) > "/dev/stderr"
	    if (nfn != i) names[nfn] = names[i]
	    nfn++
	    if ((filecount[names[i]] > 1) && !nowarn) {
		printf("warning: file %s defined %d times\n",
		       basename_to_filename[names[i]],
		       filecount[names[i]]) > "/dev/stderr"
	    }
	    filecount[names[i]] = 0
	    deleted[names[i]] = basename_to_filename[names[i]]
	}
    }
    fn = nfn

    #
    # Compact deleted files out of the sfiles array.  First, have to undo
    # the bookkeeping which was done above to handle repeat defines.
    #
    for(i = 0; i < fn; i++) {
	if (names[i] in deleted) {
	    filecount[names[i]]++
#	    if (dbg) printf("filecount[%s] = %d\n", names[i],
#			    filecount[names[i]]) > "/dev/stderr"
	    delete deleted[names[i]]
	}
    }
    nsn = 0
    for (i = 0; i < sn; i++) {
	bn = basename(sfiles[i])
	if (filecount[bn] > 0) {
#	    if (dbg) printf("sfiles[%d] = sfiles[%d] = %s\n",
#			    nsn, i, sfiles[i]) > "/dev/stderr"
	    if (nsn != i) sfiles[nsn] = sfiles[i]
	    nsn++
	    filecount[bn] = 0
	    deleted[bn] = sfiles[i]
	} else {
	    if (basename(sfiles[i]) == basename(deleted[bn])) {
#		if (dbg) printf("deleting sfiles[%d] = %s\n",
#				i, sfiles[i]) > "/dev/stderr"
	    } else {
#		if (dbg) printf("sfiles[%d] = sfiles[%d] = %s\n",
#				nsn, i, sfiles[i]) > "/dev/stderr"
		if (nsn != i) sfiles[nsn] = sfiles[i]
		nsn++
	    }
	}
    }
    sn = nsn

    #
    # Compact files out of the enfiles array.
    #
    nen = 0
    for (i = 0; i < en; i++) {
	split(enfiles[i], f)
	filename = f[1]
	inodename = f[2]
	if (encount[filename, inodename] > 0) {
	    if (dbg) printf("enfiles[%d] = enfiles[%d] = %s\n",
			    nen, i, enfiles[i]) > "/dev/stderr"
	    if (nen != i) enfiles[nen] = enfiles[i]
	    nen++
	    encount[bn] = 0
	} else {
	    if (dbg) printf("deleting enfiles[%d] = %s\n",
			    i, enfiles[i]) > "/dev/stderr"
	}
    }
    en = nen

    #
    # Clean up gfiles lists.
    #
    for (i = 0; i < gn; i++) {
	gfiles[i] = substr(gfiles[i], 2, length(gfiles[i]) - 2)
    }

    #
    # Warn about undefined options, and delete definitions for options which
    # have values already.  Then run through the undefined options putting
    # their default values in options.
    #
    for (i in options) {
	if (i in option_defined) {
	    delete option_defined[i]
	} else if (!nowarn) {
	    printf("warning: option %s was not defined\n", i) > "/dev/stderr"
	}
    }
    for (i in option_defined) {
	options[i] = "0"
    }

    #
    # Message processing.
    #
    if (fm) {
	if (message_file == "") {
	    printf("warning: message file was not defined\n") > "/dev/stderr"
	}
	if (message_prefix == "") {
	    printf("warning: message prefix was not defined\n") > "/dev/stderr"
	}
    }
}
#
#
# Track input files
#
FILENAME != infile_save {
    infile_save = FILENAME
    infiles = infiles ", " FILENAME
}
#
#
# include <filename>
#
# Note:  This is not totally satisfactory, since the included file is not read
#	 until the current file has been read completely.  However, the
#	 current implementation is not very order dependent, and this is better
#	 than not honoring includes at all.  NEWBUILD.COM takes advantage of
#	 the ability to process include statements inline from bldscn.awk.
#
$1 == "include" {
    ARGV[ARGC++] = $2
}
#
#
# define option <option name>
#
# This defines the option with a value of 0.  Used to initialize the array of
# all options.  See file options.bld.
#
($1 == "define") && ($2 == "option") {

    option_name = $3

    if ((option_name in option_defined) && !nowarn) {
	printf("warning: option \"%s\" redefined\n",
	       option_name) > "/dev/stderr"
    }
    option_defined[option_name] = "0"
    next
}
#
#
# option[s] <optname> [value]
#
$1 == "options" || $1 == "option" {

    optname = toupper($2)
    value = $3

    if (value == "" || substr(value, 1, 1) == "#") value = "1"
    options[optname] = value
    next
}
#
#
# architecture <archname>
#
($1 == "architecture") {

    archname = toupper($2)

    if (arch != "") {
	if ((archname != arch && arch != "") && !nowarn) {
	    printf("warning: architecture changed from %s to %s\n",
		   arch, archname) > "/dev/stderr"
	    options[arch] = "0"
	}
    }
    arch = archname
    options[archname] = "1"
    next
}
#
#
# platform <platformname>
#
($1 == "platform") {

    platformname = toupper($2)

    if (platform != "") {
	if ((platformname != platform && platform != "") && !nowarn) {
	    printf("warning: platform changed from %s to %s\n",
		   platform, platformname) > "/dev/stderr"
	    options[platform] = "0"
	}
    }
    platform = platformname
    if (config == "") config = platformname
    options[platformname] = "1"
    next
}
#
#
# configuration <configname>
#
($1 == "configuration") {

    configname = toupper($2)

    if (config != "") {
	if ((configname != config && config != platform && config != "") \
	    && !nowarn) {
	    printf("warning: configuration changed from %s to %s\n",
		   config, configname) > "/dev/stderr"
	    options[config] = "0"
	}
    }
    config = configname
    options[configname] = "1"
    next
}
#
#
# romoffset <romoffset>
#
($1 == "romoffset") {

    romoffset = $2
    next
}
#
#
# value <valname> [<value>]
#
($1 == "value") {
    valname = $2
    value = genvalue($3)

    # allow for C style hex constants

    if (value == "" || substr(value, 1, 1) == "#") value = "1"
    values[valname] = value
    next
}

#
# string <stringname> [<string>]
#
($1 == "string") {
    valname = $2
    value = $3
    strings[valname] = value
    next
}

#
# define value <valname> [<value>]
#
# This is identical in functionality to value <valname> [<value>].  It's just
# here for symmetry with [define] option.
#
($1 == "define") && ($2 == "value") {
    valname = $3
    value = genvalue($4)

    if (value == "" || substr(value, 1, 1) == "#") value = "1"
    values[valname] = value
    next
}
#
#
# c_header ... endc_header
#
$1 == "c_header" {
    while (getline x > 0) {
	split (x, f)
	if (f [1] == "endc_header") break
	c_header [clines++] = x
    }
    next
}
#
#
# Define regular expressions that are to be included/excluded
# in the symbol table driver.
#
# sym_include <regular_expression>...
# sym_exclude <regular_expression>...
#
($1 == "sym_include") {
    for (i=2; i<=NF; i++) {
	if (substr($i, 1, 1) == "#") break
	sym_inclusions [$i]++
    }
    next
}

($1 == "sym_exclude") {
    for (i=2; i<=NF; i++) {
	sym_exclusions [$i]++
    }
    next
}
#
#
# file <filename> [group <groupname>]
#
($1 == "file") {

    filename = tolower($2)
    if ($3 == "group") add_to_group(filename, $4)
    record_file(filename)
    next
}
#
#
# delete file <filename>
#
($1 == "delete") && ($2 == "file") {

    filename = tolower($3)
    bn = basename(filename)

    filecount[bn]--
#   if (dbg) printf("filecount[%s] = %d\n", bn, filecount[bn]) > "/dev/stderr"
    deleted[bn] = filename
    next
}
#
#
# delete driver <filename>
#
($1 == "delete") && ($2 == "driver") {

    filename = tolower($3)
    bn = basename(filename)

    filecount[bn]--
#   if (dbg) printf("filecount[%s] = %d\n", bn, filecount[bn]) > "/dev/stderr"
    deleted[bn] = filename

    #
    # Strip off "_driver" if it is present.
    #
    sub(/_driver$/, "", bn)    
#   if (dbg) printf("deleting driver %s\n", bn) > "/dev/stderr"

    for(i = 0; i < dn; i++) {
	if (dnames[i] == bn) {
#	    if (dbg) printf("deleted driver number is %d, name is %s, phase is %d\n", i, dnames[i], dphase[i]) > "/dev/stderr"
	    dn--;
	    for(j = i; j < dn; j++) {
		dnames[j] = dnames[j+1];
		dphase[j] = dphase[j+1];
	    }
	    break;
	}
    }

    next
}
#
#
# delete encapsulation <filename>
#
($1 == "delete") && ($2 == "encapsulation") {
    filename = tolower($3)
    inodename = tolower($4)

    encount[filename, inodename]--
    if (dbg) printf("encount[%s %s] = %d\n", filename, inodename,
		    encount[filename, inodename]) > "/dev/stderr"
    next
}
#
($1 == "delete") {
    printf("error: unrecognized statement\n\t%s\n\ttry delete file\n",
	   $0) > "/dev/stderr"
}
#
#
# driver <drivername> <startup_phase> [group <groupname>]
#
($1 == "driver") {

    filename = tolower($2)
    phase = $3

    if ($4 == "group") add_to_group(filename, $5)
    bn = record_file(filename)

    #
    # Strip off "_driver" if it is present.
    #
    sub(/_driver$/, "", bn)    

    dnames[dn] = bn
    dphase[dn] = phase
    dn++
    next
}
#
#
# Encapsulate <source_filename> <destination_filename>
#
($1 == "encapsulate") {
    filename = tolower($2)
    inodename = tolower($3)

    enfiles[en++] = filename " " inodename " " $4 " " $5
    encount[filename, inodename]++    
    next
}
#
#
# Message value <name> <value>
#
($1 == "message") && ($2 == "value") {
    name = $3
    value = $4

    if (value == "" || substr(value, 1, 1) == "#") {
	printf("warning: no message value present, ignoring:\n" \
	       "         %s\n", $0) > "/dev/stderr"
	skip
    }

    messages[fm++] = name " " value
}
#
# Message prefix <prefix>
#
($1 == "message") && ($2 == "prefix") {
    message_prefix = $3
}
#
#
# Message file <filename>
#
($1 == "message") && ($2 == "file") {
    filename = tolower($3)

    if (!(match(filename, /:/) || match(filename, /]/) || \
	match(filename, />/))) {
	filename = "cp$src:" filename
    }

    if (message_file != "" && message_file != filename) {
	printf("warning: message file redefined\n") > "/dev/stderr"
	printf("         (was %s, now %s)\n", 
	       message_file, filename) > "/dev/stderr"
    }

    message_file = filename
}
#
#
#
# Add a file to a group
#
function add_to_group(filename, grpname,     bn, g) {
    bn = basename(filename)

    match(grpname, /[a-zA-Z0-9\$_]+/)
    grpname = substr(grpname, RSTART, RLENGTH)

    /* Find the group, or allocate it if new. */
    if (grpname in gind) {
	g = gind[grpname]
    } else {
	gind[grpname] = gn
	gfiles[gn] = ""
	gnames[gn] = grpname
	g = gn
	gn++
    }

    gfiles[g] = gfiles[g] " " bn
    group[bn] = grpname
}
#
#
# Count the number of encapsulated files.
# Clone them into clenfiles.
# Return the count.
# (This is here only for compatability -- it is no longer needed).
#
function get_enfiles(clenfiles,     nen, i) {
    nen = 0
    for (i = 0; i < en; i++) {
	clenfiles [nen++] = enfiles [i]
    }
    return nen
}
#
#
# Record a full file name in the names and sfiles arrays, and also record
# it in the basename_to_filename array.
#
function record_file(filename,		bn) {
    filename = tolower(filename)
    bn = basename(filename)

    # watch for conflicting file names
    if (bn in basename_to_filename) {
	if ((basename_to_filename[bn] != filename) && !nowarn) {
	    printf("warning: files %s and %s conflict (basename %s)\n",
		   filename, basename_to_filename[bn], bn) > "/dev/stderr"
	}
    }
    basename_to_filename[bn] = filename

    # Prepend cp$src: if the file name isn't already qualified.
    qfname = filename
    if (!(match(qfname, /:/) || match(qfname, /]/) || match(qfname, />/))) {
	qfname = "cp$src:" qfname
    }

    sfiles[sn++] = qfname
    names[fn++] = bn
    filecount[bn]++
#   if (dbg) printf("filecount[%s] = %d\n", bn, filecount[bn]) > "/dev/stderr"

    return bn
}
#
#
# Return the basename of a file.
#
function basename(filename) {
    sub(/^.*:/, "", filename)
    sub(/^.*]/, "", filename)
    sub(/^.*>/, "", filename)
    match (filename, /[a-zA-Z0-9\$_]+/)
    return substr(filename, RSTART, RLENGTH)
}

#
# Generate a numeric value.  Allow for C style
# hex constants as well as decimal numbers
#
function genvalue(s,     i, n, buf) {
	if (length(s) == 0) return s

	if (match(s,/0[Xx].*/)) {
	    sub(/0[Xx]/, "0", s)
	    n = 0
	    for (i=1; i<=length(s); i++) {
		n *= 16;
		n += hexdigits [substr(s, i, 1)]
	    }
	    buf = sprintf ("%d", n)
	    return buf
	} else {
	    return s
	}
}


