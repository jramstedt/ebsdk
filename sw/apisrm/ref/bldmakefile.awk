# file:	bldmakefile.awk
#
#++
# FACILITY:
#
#	Common Console Development Environment
#
# MODULE DESCRIPTION:
#
#	Output an makefile include file based on the contents of the build file.
#	The file scnbld.awk scans the build file.
#
# Usage:
#	awk -f scnbld -f isort -f bldmms <input_file> > <output_file>
#
#--
END {
    opt = (opt != "")

    printf ("# Makefile Macro Definitions\n")

    printf("\n# Architecture\n")
    printf("arch = %s\n", arch)
    printf("%s = 1\n", arch)

    printf("\n# Platform\n")
    printf("platform = %s\n", platform)
    printf("%s = 1\n", platform)

    printf("\n# Configuration\n")
    printf("config = %s\n", config)
    if (config != platform) printf("%s = 1\n", config)

    printf("\n# Rom Offset\n")
    printf("romoffset = %s\n", romoffset)

    printf("\n# Options\n")
    optind[0] = 0
    nopts = isort(options, optind)
    for (i = 0; i < nopts; i++) {
        if (!options[optind[i]]) continue
        if (options[optind[i]] == "0") continue
        if (optind[i] == arch) continue
        if (optind[i] == platform) continue
        if (optind[i] == config) continue
        printf("%s = %d\n", optind[i], options[optind[i]])
    }
}
