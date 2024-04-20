# file:	bldsym.awk
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
#	Output the symbol table inclusion and exclusion regular expressions.
#	These are qualified against the symbols that scansym generates, so 
#	that the set of symbols in the symbol table driver can be
#	customized.
#
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f bldsym <input_file> > <output_file>
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
    by = "bldsym.awk"

    #
    # Instantiate entries in the encapsulated file table.
    # This table is scanned by rd_init to build the entries
    # in the ram disk.
    #
    printf ("# symbol table inclusion/exclusion regular expressions \n")

    # 
    printf ("BEGIN {\n")
    for (i in sym_inclusions) {
	printf ("\tinclusions [\"%s\"]++\n", i)
    }
    printf ("\n")

    for (i in sym_exclusions) {
	printf ("\texclusions [\"%s\"]++\n", i)
    }
    printf ("}\n")
}
