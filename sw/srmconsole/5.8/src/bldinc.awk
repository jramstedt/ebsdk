# file:	bldinc.awk
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
#	Output a DCL command procedure which will create the necessary header
#	files for the build.
#
# Usage:
#	gawk -f scnbld -f isort -f bldinc <input_file> > <output_file>
#
#  AUTHORS:
#
#	Peter H. Smith
#
#  CREATION DATE:
#
#	12-JUL-1993
#
#  MODIFICATION HISTORY:
#
#	phs	12-JUL-1993	Initial revision.
#--
#
END {

    # Sort the options into alphabetical order.
    optind[0] = 0
    nopts = isort(options, optind)

    # Output the header
    printf("$ ! DCL Procedure to set up option include files\n")
    printf("$ ! platform = %s, configuration = %s, architecture = %s\n",
	   platform, config, arch)
    printf("$\n")
    printf("$\n")

    # Output the list of options and values
    for (i = 0; i < nopts; i++) {
	optnam = toupper(optind[i])
	optval = options[optind[i]]

	printf("$ call optinc \"%s\" \"%s\"\n", optnam, optval)
    }
    printf("$\n")
    printf("$\n")

    valind[0] = 0
    nvals = isort(values, valind)
    for (i = 0; i < nvals; i++) {
	valnam = toupper(valind[i])
	val = values[valind[i]]

	printf("$ call valinc \"%s\" \"%s\"\n", valnam, val)
    }
    printf("$\n")
    printf("$\n")
    #
    #
    # Output the subroutine which checks and creates option header files.
    printf("$ optinc: subroutine\n")
    printf("$     optnam = \"''P1'\" ! P1 : Option Name\n")
    printf("$     optval = \"''P2'\" ! P2 : Option Value\n")
    printf("$\n")
    printf("$     create = 1\n")
    printf("$     file = \"CP$INC:OPT_\" + optnam + \".H\"\n")
    printf("$     if f$search(file) .nes. \"\"\n")
    printf("$     then\n")
    printf("$         open/read/error=rfail_1 in 'file'\n")
    printf("$         lno = 1\n")
    printf("$         read/end=rfail_2/error=rfail_2 in line\n")
    printf("$         if f$extract(0, 8, line) .nes. \"#define \"\n")
    printf("$         then\n")
    printf("$             lno = 2\n")
    printf("$             read/end=rfail_2/error=rfail_2 in line\n")
    printf("$         endif\n")
    printf("$         close in\n")
    printf("$         if f$extract(0, 8, line) .eqs. \"#define \"\n")
    printf("$         then\n")
    printf("$             toptnam = f$extract(8, f$length(optnam), line)\n")
    printf("$             toptnam = \"''toptnam'\" ! debug\n")
    printf("$             if toptnam .nes. optnam\n")
    printf("$             then\n")
    printf("$                 write sys$error \"option mismatch: \", -\n" \
	   "                        file, \" \", optnam, \" \", toptnam\n")
    printf("$             endif\n")
    printf("$             toptval = f$extract(9 + f$length(optnam), -\n" \
	   "                                  255, line)\n")
    printf("$             toptval = \"''toptval'\" ! debug\n")
    printf("$             if toptval .eqs. optval\n")
    printf("$             then\n")
    printf("$                 create = 0\n")
    printf("$             else\n")
    printf("$                 ! Option was different.\n")
    printf("$             endif\n")
    printf("$         else\n")
    printf("$             write sys$error \"line \", lno, \" of \", file, -\n" \
	   "                    \" is not a #define statement\"\n")
    printf("$         endif\n")
    printf("$         goto rdone\n")
    printf("$     rfail_2:\n")
    printf("$         write sys$error \"read of \", file, \" failed\"\n")
    printf("$         close in\n")
    printf("$         goto rdone\n")
    printf("$     rfail_1:\n")
    printf("$         write sys$error \"could not open \", file\n")
    printf("$     rdone:\n")
    printf("$     endif\n")
    printf("$\n")
    printf("$     if create\n")
    printf("$     then\n")
    printf("$         open/write/error=wfail_1 out 'file'\n")
    printf("$         write/error=wfail_2 out \"#ifndef \", optnam\n")
    printf("$         write/error=wfail_2 out \"#define \", optnam, -\n" \
	   "                                  \" \", optval\n")
    printf("$         write/error=wfail_2 out \"#endif /* \", optnam, -\n" \
	   "                                  \" */\"\n")
    printf("$         close out\n")
    printf("$         goto wdone\n")
    printf("$     wfail_2:\n")
    printf("$         write sys$error \"could not write to \", file\n")
    printf("$         close out\n")
    printf("$         goto wdone\n")
    printf("$     wfail_1:\n")
    printf("$         write sys$error \"could not create \", file\n")
    printf("$     wdone:\n")
    printf("$     endif\n")
    printf("$\n")
    printf("$     exit\n")
    printf("$ endsubroutine\n")
    #
    #
    # Output the subroutine which checks and creates value header files.
    printf("$ valinc: subroutine\n")
    printf("$     valnam = \"''P1'\" ! P1 : Value Name\n")
    printf("$     valval = \"''P2'\" ! P2 : Value\n")
    printf("$\n")
    printf("$     create = 1\n")
    printf("$     file = \"CP$INC:VAL_\" + valnam + \".H\"\n")
    printf("$     if f$search(file) .nes. \"\"\n")
    printf("$     then\n")
    printf("$         open/read/error=rfail_1 in 'file'\n")
    printf("$         lno = 1\n")
    printf("$         read/end=rfail_2/error=rfail_2 in line\n")
    printf("$         if f$extract(0, 8, line) .nes. \"#define \"\n")
    printf("$         then\n")
    printf("$             lno = 2\n")
    printf("$             read/end=rfail_2/error=rfail_2 in line\n")
    printf("$         endif\n")
    printf("$         close in\n")
    printf("$         if f$extract(0, 8, line) .eqs. \"#define \"\n")
    printf("$         then\n")
    printf("$             tvalnam = f$extract(8, f$length(valnam), line)\n")
    printf("$             tvalnam = \"''tvalnam'\" ! debug\n")
    printf("$             if tvalnam .nes. valnam\n")
    printf("$             then\n")
    printf("$                 write sys$error \"valion mismatch: \", -\n" \
	   "                        file, \" \", valnam, \" \", tvalnam\n")
    printf("$             endif\n")
    printf("$             tvalval = f$extract(9 + f$length(valnam), -\n" \
	   "                                  255, line)\n")
    printf("$             tvalval = \"''tvalval'\" ! debug\n")
    printf("$             if tvalval .eqs. valval\n")
    printf("$             then\n")
    printf("$                 create = 0\n")
    printf("$             else\n")
    printf("$                 ! Value was different.\n")
    printf("$             endif\n")
    printf("$         else\n")
    printf("$             write sys$error \"line \", lno, \" of \", file, -\n" \
	   "                    \" is not a #define statement\"\n")
    printf("$         endif\n")
    printf("$         goto rdone\n")
    printf("$     rfail_2:\n")
    printf("$         write sys$error \"read of \", file, \" failed\"\n")
    printf("$         close in\n")
    printf("$         goto rdone\n")
    printf("$     rfail_1:\n")
    printf("$         write sys$error \"could not open \", file\n")
    printf("$     rdone:\n")
    printf("$     endif\n")
    printf("$\n")
    printf("$     if create\n")
    printf("$     then\n")
    printf("$         open/write/error=wfail_1 out 'file'\n")
    printf("$         write/error=wfail_2 out \"#ifndef \", valnam\n")
    printf("$         write/error=wfail_2 out \"#define \", valnam, -\n" \
	   "                                  \" \", valval\n")
    printf("$         write/error=wfail_2 out \"#endif /* \", valnam, -\n" \
	   "                                  \" */\"\n")
    printf("$         close out\n")
    printf("$         goto wdone\n")
    printf("$     wfail_2:\n")
    printf("$         write sys$error \"could not write to \", file\n")
    printf("$         close out\n")
    printf("$         goto wdone\n")
    printf("$     wfail_1:\n")
    printf("$         write sys$error \"could not create \", file\n")
    printf("$     wdone:\n")
    printf("$     endif\n")
    printf("$\n")
    printf("$     exit\n")
    printf("$ endsubroutine\n")
    printf("$ exit\n")
}
