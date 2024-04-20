# file:	bldmsg.awk
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
#	Output a gawk file which initializes some message information.
#
# Usage:
#	gawk -f scnbld -f bldmsg <input_file> > <output_file>
#
#  AUTHORS:
#
#	Peter H. Smith
#
#  CREATION DATE:
#
#	21-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	21-Jun-1993	Initial revision.
#--
#
END {

    printf("# Message definitions\n")

    printf("BEGIN {\n")
    printf("    message_prefix = \"%s\"\n", message_prefix)
    printf("    message_file = \"%s\"\n", message_file)
    printf("\n")

    for (i = 0; i < fm; i++) {
	split(messages[i], x)

	name = x[1]
	value = x[2]

	if (!match(name, "^" message_prefix)) {
	    name = message_prefix name
	}

	printf("    message_values[\"%s\"] = %d\n", name, value)
	if (value in message_name && name != message_name[value]) {
	    printf("warning: both %s and %s have value %s\n",
		   name, message_name[value], value)
	}
	message_name[value] = name
	printf("    message_name[%s] = \"%s\"\n", value, name)
    }
    printf("}\n")
    printf("\n")
    printf("/%s[a-zA-Z0-9_$]/ { collect_messages($0) }\n", message_prefix)
}
