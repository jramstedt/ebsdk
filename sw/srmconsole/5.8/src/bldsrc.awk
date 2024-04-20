# file:	bldsrc.awk
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
#	Output a list of source files based on the scan of a build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f bldsrc <input_file> > <output_file>
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
#	phs	20-JUL-1993	Move generation of cp$src: prefix to BLDSCN.
#	phs	08-Jun-1993	Initial revision, based on build_generic.awk.
#--
#
END {

    for (i = 0; i < sn; i++) {
	if (match(sfiles[i], /vlist\.c$/)) continue
	printf ("%s\n", sfiles[i])
    }
    for (i = 0; i < en; i++) {
	split (enfiles[i], f)
	if (f[4] == "-script") {
	    printf ("%s\n", f[1])
	}
    }
}
