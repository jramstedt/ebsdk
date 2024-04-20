# file:	bldenc.awk
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
#	Output the encapsulated file table (.c file) based on input from
#	a build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	gawk -f scnbld -f bldenc <input_file> > <output_file>
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

    #
    # Instantiate entries in the encapsulated file table.
    # This table is scanned by rd_init to build the entries
    # in the ram disk.
    #
    printf ("/* %s Encapsulated file table */\n", config)

    for (i = 0; i < en; i++) {
	split(enfiles[i], f)
	printf("extern int encap_%s;\n", f[2])
    }

    printf ("struct ENCAP {\n")
    printf ("\tint\t*address;\n")
    printf ("\tint\tattributes;\n")
    printf ("\tchar\t*name;\n")
    printf ("} encap_files [] = {\n")

    for (i = 0; i < en; i++) {
	split (enfiles [i], f)
	printf ("\t{&encap_%s,\t%s,\t\"%s\"},\n",
		f[2], f [3], f [2])
    }
    printf ("};\n")
    printf ("int num_encap = sizeof (encap_files) " \
	    " / sizeof (encap_files[0]);\n")
}
