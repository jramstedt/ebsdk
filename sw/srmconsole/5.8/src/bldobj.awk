# file:	bldobj.awk
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
#	Output a list of object files based on the scan of a build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f bldobj <input_file> > <output_file>
#
#  AUTHORS:
#
#	Peter H. Smith, based on A.J. Beaverson's build_generic.awk
#
#  CREATION DATE:
#
#	10-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	10-Jun-1993	Initial revision, based on build_generic.awk.
#--
#
END {
    opt = (opt != "")

    name = ""
    for (i=0; i <= fn; i++) {
	if (opt && names[i] in group) continue
	if (name != "") printf ("cp$obj:%s.obj\n", name)
	name = names[i]
    }

    if (opt) {
	for (i = 0; i < gn; i++) {
	    if (name != "") printf ("cp$obj:%s.obj\n", name)
	    name = "grp_" gnames[i]
	}
    }

    for (i=0; i < en; i++) {
	split (enfiles[i], f)
	if (name != "") printf ("cp$obj:%s.obj\n", name)
	name = "encap_" f[2]
    }
    if (name != "") printf("cp$obj:%s.obj\n", name)
}
