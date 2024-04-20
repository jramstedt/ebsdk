# file:	bldlm.awk
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
#	Output a Macro header file based on the contents of a build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f isort -f bldplm <input_file> > <output_file>
#
#  AUTHORS:
#
#	Peter H. Smith
#
#  CREATION DATE:
#
#	17-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	17-Jun-1993	Initial revision.
#--
END {
    opt = (opt != "")

    printf ("; Platform header file\n")

    printf("\n; Architecture:\n")
    printf("%s = 1\n", arch)

    printf("\n; Platform:\n")
    printf("%s = 1\n", platform)

    printf("\n; Enabled Options:\n")

    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
	if (optind[i] == arch) continue
	if (optind[i] == platform) continue
	if (options[optind[i]] == 0) continue
	printf("%s = %d\n", optind[i], options[optind[i]])
    }

    printf("\n; Disabled Options:\n")

    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
	if (optind[i] == arch) continue
	if (optind[i] == platform) continue
	if (options[optind[i]] != 0) continue
	printf("%s = %d\n", optind[i], options[optind[i]])
    }

    printf("\n; Values:\n")

    valind[0] = 0
    nvals = isort(values, valind)
    for (i = 0; i < nvals; i++) {
	printf("%s = %d\n", valind[i], values[valind[i]])
    }
}
