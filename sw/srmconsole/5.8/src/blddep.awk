# file:	blddep.awk
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
#	gawk -f scnbld -f blddep <input_file> > <output_file>
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
#	pel	11-Aug-1993	remove mung
#	phs	20-Jul-1993	Clean up source file naming.
#	phs	08-Jun-1993	Initial revision, based on build_generic.awk.
#--
#
END {
    root = "generic"

    #
    # Write out mms objects macros
    #
    printf("! %s Target-specific dependencies\n", config)
    printf("%sobjects = -\n", gn > 0 ? "i_" : "")
    name = ""
    for (i = 0; i < fn; i++) {
	if (names[i] in group) continue
	if (name != "") printf("\tcp$obj:%s.obj,-\n", name)
	name = names[i]
    }
    for (i=0; i < en; i++) {
	split (enfiles[i], f)
	if (name != "") printf("\tcp$obj:%s.obj,-\n", name)
	name = "encap_" f[2]
    }
    if (name != "") printf ("\tcp$obj:%s.obj\n", name)
    printf("\n");

    #
    # If any files were placed in groups, output conditional dependencies.
    #
    if (gn > 0) {
	printf(".IFDEF opt\n")
	printf("objects = $(i_objects), -\n")
	name = ""
	for (i = 0; i < gn; i++) {
	    if (name != "") printf(" \tcp$obj:grp_%s.obj,-\n", name)
	    name = gnames[i]
	}
	if (name != "") printf(" \tcp$obj:grp_%s.obj\n", name)
	printf(".ELSE $(opt)\n")
	printf("objects = $(i_objects), -\n")
	name = ""
	for (i = 0; i < fn; i++) {
	    if (names[i] in group) {
		if (name != "") printf("\tcp$obj:%s.obj,-\n", name)
		name = names[i]
	    }
	}
	if (name != "") printf("\tcp$obj:%s.obj\n", name)
	printf(".ENDIF\n")
#
	#
	# Output dependencies/commands for each group.  The compiler is
	# limited to 94 "plus" files, and the command line interpreter is
	# limited to a command line length of 1019.
	#
	printf("\n.IFDEF opt\n")
	for (i = 0; i < gn; i++) {
	    printf ("!\n")
	    group_total = split( gfiles[i], f)
	    printf ("! Group \"%s\" (%d files)\n", gnames[i], group_total)
	    if (group_total > 94) {
		printf("warning: Group \"%s\" has %d files\n",
		       gnames[i], group_total) > "/dev/stderr"
		printf("\n!\n! warning: %d > 94 files\n", group_total)
	    }
	    printf ("!\n")
	    printf ("cp$obj:grp_%s.obj : -\n", gnames[i])
	    for(j = 1; j <= group_total - 1; j++) {
		printf("\tcp$src:%s.c, $(%s_depend),-\n", f[j], f[j])
	    }
	    printf("\tcp$src:%s.c, $(%s_depend)\n",
		   f[group_total], f[group_total])

	    cmd_len = 0
	    line = " $(cc) $(cdefs) $(cquals) $(copt) $(list) -\n"
	    printf(line)
	    cmd_len += length(line)
	    line = sprintf("\t/plus/obj=cp$obj:grp_%s.obj-\n", gnames[i])
	    printf(line)
	    cmd_len += length(line)
	    line = sprintf("\t/lis=cp$lis:grp_%s.lis-\n", gnames[i])
	    printf(line)
	    cmd_len += length(line)
	    first_name = 1
	    line_max = 240
	    line = ""
	    for(j in f) {
		if (length(line) + length(f[j]) > line_max) {
		    line = sprintf("\t%s-\n", line)
		    printf(line)
		    cmd_len += length(line)
		    line = "+" f[j]
		    line_count++
		} else {
		    if (first_name) {
			line = line "cp$src:" f[j] ".c"
			first_name = 0
		    } else {
			line = line "+" f[j]
		    }
		}
	    }
	    line = sprintf("\t%s\n", line)
	    printf(line)
	    cmd_len += length(line)
	    cmd_len += 11 # Fudge for expansion of macros.
	    if (cmd_len > 900) {
		printf("\t! Warning: Group \"%s\" command length was ~%d\n",
		       gnames[i], cmd_len)
		printf("warning: Group \"%s\" command length was ~%d\n\n",
		       gnames[i], cmd_len) > "/dev/stderr"
	    } else {
		printf("\t! Group \"%s\" command length was ~%d\n\n",
		       gnames[i], cmd_len)
	    }
	}
	printf(".ENDIF\n\n")
    }
#
    #
    # write all the objects for the vlist (exclude vlist)
    #
    printf ("%svlist_objects = -\n", gn > 0 ? "i_" : "")
    name = ""
    for (i=0; i<fn; i++) {
	if (names [i] in group) continue
	if (names [i] == "vlist") continue
	if (name != "") printf("\tcp$obj:%s.obj,-\n", name)
	name = names [i]
    }
    if (name != "") printf("\tcp$obj:%s.obj\n\n", name)
    printf("\n");

    if (gn > 0) {
	printf(".IFDEF opt\n")
	printf("vlist_objects = $(i_vlist_objects), -\n")
	name = ""
	for (i = 0; i < gn; i++) {
	    if (name != "") printf("\tcp$obj:grp_%s.obj,-\n", name)
	    name = gnames[i]
	}
	if (name != "") printf("\tcp$obj:grp_%s.obj\n", name)

	printf(".ELSE $(opt)\n")
	first = 1
	name = ""
	for (i = 0; i < fn; i++) {
	    if (names[i] in group) {
		if (name != "") {
		    if (first) {
			printf("vlist_objects = $(i_vlist_objects), -\n")
			first = 0
		    }
		    printf("\tcp$obj:%s.obj,-\n", name)
		}
		name = names[i]
	    }
	}
	if (name != "") {
	     if (first) printf("vlist_objects = $(i_vlist_objects), -\n")
	     printf("\tcp$obj:%s.obj\n", name)
	     first = 0
	}
	if (first) printf("vlist_objects = $(i_vlist_objects)\n")
	printf(".ENDIF\n")
    }
    

    #
    # Write out an MMS macro that defines .slis inclusions.  Have to
    # delay output in case modlist or vlist is last.
    #
    printf("\n")
    printf("slis = -\n")
    name = ""
    for (i = 0; i < fn; i++) {
	if (names [i] == "modlist") continue
	if (names [i] == "vlist") continue
	if (name != "") printf ("\tcp$lis:%s.slis,-\n", name)
	name = names [i]
    }
    if (name != "") printf ("\tcp$lis:%s.slis\n", name)
    
    #
    # Generate the encapsulation dependencies.  These are tacked
    # on to the end of the mms macro file.
    #
    printf ("\n")
    printf ("! Encapsulation dependencies\n")
    printf ("\n")
    for (i = 0; i < en; i++) {
	split (enfiles[i], f)
	printf("cp$obj:encap_%s.obj : %s\n", f[2], f[1])
	printf("\tmcr cp$kits:[encapsulate.new]encapsulate %s " \
	       "cp$src:encap_%s.mar encap_%s %s\n",
	       f[1], f[2], f[2], f[4])
	if (arch == "ALPHA") {
	    printf("\t$(macro64)/nolis/obj=cp$obj cp$src:encap_%s.mar\n", f[2])
	} else if (arch == "VAX") {
	    printf("\t$(macro)/nolis/obj=cp$obj cp$src:encap_%s.mar\n", f[2])
	} else {
	    printf("\t! unrecognized architecture (\"%s\")\n", arch)
	}
	printf ("\n")
    }

    #
    # Generate all the C file dependencies
    #
    printf ("\n")
    printf ("! C file dependencies\n")
    printf ("\n")
    for (i =0; i < sn; i++) {
	if (match (sfiles[i], /\.c$/)) {
	    full = sfiles[i]
	    root = full
	    sub(/^.*:/, "", root)
	    sub(/^.*]/, "", root)
	    sub(/^.*>/, "", root)
	    sub(/.c$/, "", root)
	    printf ("cp$obj:%s.obj : %s, $(%s_depend)\n",
		    root, full, root)
	}
    }
}
