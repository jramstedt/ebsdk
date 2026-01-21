# file:	bldls.awk
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
#	gawk -f scnbld -f isort -f bldpls <input_file> > <output_file>
#
#--
END {
    opt = (opt != "")

    printf ("# %s Platform header file\n", platform)

    printf("\n# Architecture:\n")
    printf("%s = 1\n", arch)

    printf("\n# Platform:\n")
    printf("%s = 1\n", platform)

    printf("\n# Enabled Options:\n")

    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
        if (optind[i] == arch) continue
        if (optind[i] == platform) continue
        if (options[optind[i]] == 0) continue
            printf("%s = %d\n", optind[i], options[optind[i]])
    }

    printf("\n# Disabled Options:\n")

    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
        if (optind[i] == arch) continue
        if (optind[i] == platform) continue
        if (options[optind[i]] != 0) continue
            printf("%s = %d\n", optind[i], options[optind[i]])
    }

    printf("\n# Values:\n")

    valind[0] = 0
    nvals = isort(values, valind)
    for (i = 0; i < nvals; i++) {
        printf("%s = %d\n", valind[i], values[valind[i]])
    }
}
