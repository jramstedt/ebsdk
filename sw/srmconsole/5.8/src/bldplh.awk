# file:	bldplh.awk
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
#	Output a C header file based on the contents of a build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f isort -f bldplh <input_file> > <output_file>
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
#
END {
    opt = (opt != "")

    include_switch = "PLATFORM_H"

    printf ("/*\n")
    printf ("/* Platform header file\n")
    printf (" */\n")

    printf("\n#ifndef %s\n", include_switch)
    printf("#define %s 1\n", include_switch)


    printf("\n/*\n * Architecture:\n */\n")
    printf("#define %s %s\n", arch, options[arch])
    delete options[arch]

    printf("\n/*\n * Platform:\n */\n")
    printf("#define %s %s\n", platform, options[platform])
    delete options[platform]

    printf("\n/*\n * Options (legal for compilation switches):\n */\n")

    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
	printf("#define %s %d\n", optind[i], options[optind[i]])
    }

    printf("\n/*\n * Values:\n */\n")

    valind[0] = 0
    nvals = isort(values, valind)
    for (i = 0; i < nvals; i++) {
	printf("#define %s %d\n", valind[i], values[valind[i]])
    }

    printf("\n/*\n * Strings:\n */\n")

    strind[0] = 0
    nstrs = isort(strings, strind)
    for (i = 0; i < nstrs; i++) {
	printf("#define %s %s\n", strind[i], strings[strind[i]])
    }


  #
  # This should go away once all files have been converted from using
  # c_header statements and include platform.h.
  #
  if (1) {
    printf("\n/*\n * C Header lines:\n */\n")

    for (i=0; i < clines; i++) {
	printf ("%s\n", c_header[i])
    }

    printf("\n#endif /* %s */\n", include_switch)
  }
}
