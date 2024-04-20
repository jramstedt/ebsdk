# file:	bldcpl.awk
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
#	Define some global symbols used by the CPLOGIN.COM procedure.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f bldcpl <input_file>
#
#  AUTHORS:
#
#	Peter H. Smith, based on A.J. Beaverson's build_generic.awk
#
#  CREATION DATE:
#
#	08-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	08-Jul-1993	Initial revision, based on build_generic.awk.
#--
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
# Note:	 The MGLOAD nightly build was having trouble with this include
#	 statement when its body was "ARGV[ARGC++] = $2" -- garbage was showing
#	 up on the file name.  Splitting it up into multiple statements seems
#	 to have helped.
#
$1 == "include" {
    filename = $2
    ARGV[ARGC] = filename
    ARGC++
}
#
#
# architecture <archname>
#
($1 == "architecture") {

    archname = toupper($2)

    if (arch != "") {
	if (archname != arch && arch != "") {
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
	if (platformname != platform && platform != "") {
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
	if (configname != config && config != platform && config != "") {
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
END {
    printf("$ cp$platform == \"%s\"\n", platform)
    printf("$ cp$config == \"%s\"\n", config)
    printf("$ cp$arch == \"%s\"\n", arch)
}
