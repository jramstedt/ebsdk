# file:	build_depend.awk

# Abstract:	Build an MMS macro file that lists C include file dependencies.
#		Scan down the list of C files, picking out all the inlcude
#		files.   Files that are to be excluded from the dependency list
#		have a " /* exclude */ " comment on the same line.
#
#	gawk -f $(awk)srcexp -f $(awk)build_depend -
#	     "exc=^($(cfg)|$(src))(modlist|msgtxt|cmds|cmdlist|dst|encap)\.c$" -
#	     "exch=(stdio|platform)\.h$" -
#	     'f$search("$(mms$source)")' > $(mms$target)

BEGIN {
	skip = 0
	opt_index = 1
	if_count = 0
	if_cnt = 0

	full_file = "cp$cfg:full_depend.mms"

	printf("! File dependencies for the common console firmware\n") > full_file

	# There are certain header files that we always ignore (some are built automaticaly).

	n = split ("stdio platform", x)
	for (i in x) {
	    exclusions [x [i]] = 1
	}
}

/^\#if/ {
	if_count++
	if_cnt++
	if_name[if_cnt] = $0
	if_seen = 1
}

/^\#else/ {
	if (if_printed) {
		printf("#else\n") > full_file
#		printf("%d#else\n", if_count) > "/dev/stderr"
	}
}

/^\#endif/ {
	if_count--
	if_cnt--
	if (if_printed) {
		printf("#endif\n") > full_file
#		printf("%d#endif\n", if_count) > "/dev/stderr"
	}
	if (if_count == 0) {
		if_cnt = 0
		if_seen = 0
		if_printed = 0
	}
}

/^\#include/ {
	name = $2

	# Ignore this inclusion if it has been tagged

	if ($0 ~ /exclude/)
	    next

	# Ignore it if it's on the exclusion list

	if (match(name, exch)) {
#	    printf("%s excluded\n", name) > full_file
	    next				# skip this line
	}

	# Start a new macro if the filename has changed.

	if (FILENAME != lastfilename)
	    process_new_file()

	# If this file was already seen, don't scan it again.

	if (skip_file)
		next

	# chop off quotes if they exist

	if (substr (name, 1, 1) == "\"")
	    name = substr (name, 2, length (name) - 2)

	# If there is no : in the name, prepend a "sys$Library:" in front

	if (index (name, ":") == 0)
	    name = "sys$library:" name

	# print the name

	if (if_seen)
		process_if_name()

	printf("	%s,-\n", name) > full_file
}

END {
	printf("\n") > full_file
	close(full_file)

	process_full_file()
}

# process_new_file
# part of 1st part, create full file

function process_new_file() {

	match (FILENAME, /[A-Za-z0-9_]*\./)
	root = substr(FILENAME, RSTART, RLENGTH-1)

	if (found [root]) {
		skip_file = 1
	} else {
		skip_file = 0
		found [root] = 1

		if_count = 0				# clear counters
		if_cnt = 0
		if_printed = 0

		printf("\n") > full_file
		printf("%s_depend	= -\n", root) > full_file
#		printf("File: %s\n", root) > "/dev/stderr"
	}
	lastfilename = FILENAME
}

# process_if_name
# part of 1st part, create full file

function process_if_name() {
	while (if_cnt > 0) {
		printf("%s\n", if_name[if_cnt]) > full_file
#		printf("%s\n", if_name[if_cnt]) > "/dev/stderr"
		if_cnt--
	}
	if_seen = 0
	if_printed = 1
}

# process_platform
# part of 1st part, create full file

function process_platform() {

	opt_array[opt_index] = "1"
	opt_val[opt_index] = 1
	opt_index++

	opt_array[opt_index] = "TRUE"
	opt_val[opt_index] = 1
	opt_index++

	opt_array[opt_index] = "FALSE"
	opt_val[opt_index] = 0
	opt_index++

	while (status = getline line < platform_spec) {
		if (status != 1)
			break
		split(line, a_array, " ")
		if (a_array[1] == "#define") {
			opt_array[opt_index] = a_array[2]
			if (a_array[3] == "1")
				opt_val[opt_index] = 1
			else
				opt_val[opt_index] = 0
#			printf("%d %s %s\n", opt_index, a_array[2], a_array[3]) > "/dev/stderr"
			opt_index++
		}
	}
	if (opt_index == 1) {
		printf("? failure - Unable to obtain platform.h\n") > "/dev/stderr"
		exit 1
	}

	opt_array[opt_index] = "0"
	opt_val[opt_index] = -1
}

# Abstract:	Create an MMS macro file that lists C include file dependencies.
#		This further processes the "full_depend.mms" file created by
#		the above code..
# example:
#
#	entry_depend	= -
#		cp$src:platform.h,-
#		...
#		cp$inc:prototypes.h,-
#	#if GAMMA
#	#if SABLE
#		cp$src:gamma_cpu.h,-
#	#else
#		cp$src:cbr_sbl_cpu.h,-
#	#endif
#	#endif
#	#if TURBO
#		cp$src:emulate.h,-
#	#endif

# process_full_file()
# part of 2nd part, create depend file

function process_full_file() {
	if_out = 0;
	if_count = 0
	opt_index = 1
	if_processed = 0
	any_parens = 0;
	any_ands = 0
	any_negs = 0
	if_satisfied = 1;

	platform_spec = "cp$cfg:platform.h"
	process_platform()

	while (status = getline full_line < full_file) {
		if (status != 1)
			break
		split(full_line, full_array, " ")

		if (index(full_line, "= -"))				# depend header
			if_header = full_line

		if (full_array[1] == "#if") {
			if_count++

			gsub(/&&/, " @% ", full_line)			# spread out &&
			gsub(/\@/, "\\&", full_line)
			gsub(/\%/, "\\&", full_line)

			gsub(/\|\|/, " @% ", full_line)			# spread out ||
			gsub(/@%/, "||", full_line)

			gsub(/\(/, " @ ", full_line)			# spread out open paren
			gsub(/@/, "\(", full_line)

			gsub(/\)/, " @ ", full_line)			# spread out close paren
			gsub(/@/, "\)", full_line)

			if_name[if_count] = full_line
#			printf("%dif %s\n", if_count, if_name[if_count]) > "/dev/stderr"
			process_if()
			continue
		}

		if (full_array[1] == "#else") {
#			printf("#else\n") > "/dev/stderr"
			if_out = !if_out
			continue
		}

		if (full_array[1] == "#endif") {
			if (!if_count)				# processed all satisfied conditionals?
				continue			# yes
			if_count--
#			printf("%d #endif\n", if_count) > "/dev/stderr"
			if (if_count == 0) {
				if_printed = 0
				if_satisfied = 1		# outside of if, allow output
			}
			continue
		}
		if (if_satisfied) {
			printf("%s\n", full_line)
			if_printed = 1
#			if (if_count)
#				printf("%s\n", full_line) > "/dev/stderr"
			if (index(full_line, "= -")) {		# print depend header
				if_header = full_line
#				printf("%s\n", full_line) > "/dev/stderr"
			}
		}
	}
}

# process_if
# part of 2nd part, create depend file

function process_if() {
	if_out = 0
	entire_neg = 0

	if (if_count > 1) {
		if (!if_satisfied) {
			return			# previous if's not satisfied
		}
	}

	if_satisfied = 0

# determine if there are any && conditionals

	any_ands = index(if_name[1], "&&")

# determine if the conditionals are enclosed by parens

	bparen = index(if_name[1], "(")
	if (bparen) {
		any_parens = 1;
		eparen = index(if_name[1], ")")
#		printf("parens %d %d %s\n", bparen, eparen, if_name[1]) > "/dev/stderr"
		if (!eparen) {
			printf("? failure - No close Paren, %s, %s\n", if_header, if_name[1]) > "/dev/stderr"
			if_satisfied = 1
			return
		}

# process conditionals contained within parens

		if (any_ands) {
			paren_name = substr(if_name[1], bparen+1, eparen - bparen - 1);
			paren_n_entries = split(paren_name, paren_array, " ")
#			printf("%d() %s %s %s %s %s %s\n",
#		  	  paren_n_entries, paren_array[1], paren_array[2], paren_array[3], paren_array[4], paren_array[5], paren_array[6]) > "/dev/stderr"
			if (paren_n_entries > 3) {
				if_satisfied = 1
				printf("? failure - Paren > 3, %s, %s\n", if_header, if_name[1]) > "/dev/stderr"
				return				# Can't do this yet, so output it anyway
			}

# Parenthesize conditionals are ANDed together
# determine if the conditional is negated

			for (i=1; i<=paren_n_entries; i++) {
				paren_neg[i] = index(paren_array[i],"!")
				if (paren_neg[i]) {
					paren_negs = 1
					paren_array[i] = substr(paren_array[i], 2)
#					printf("%dneg %s\n", i, paren_array[i]) > "/dev/stderr"
				}
			}
			paren_and_true = 0
			paren_and_false = 1
			process_one_conditional(paren_array[1])
			if (if_out)
				process_one_conditional(paren_array[3])
			if (if_out) {
#				printf("paren string %x\n", if_out) > "/dev/stderr"
				paren_and_true = 1
				paren_and_false = 0
			}
			paren_name = substr(if_name[1], bparen, eparen - bparen + 1);
#			printf("%s\n", paren_name) > "/dev/stderr"
			if (paren_and_true)
				sub(paren_name, "TRUE", if_name[1])
			else
				sub(paren_name, "FALSE", if_name[1])
			sub(/\(/, " ", if_name[1])
			sub(/\)/, " ", if_name[1])
#			printf("paren: %s\n", if_name[1]) > "/dev/stderr"
		}

# parenthesized conditionals are only ORed together so just remove parens

		if (!any_ands) {
			sub(/\(/, " ", if_name[1])
			sub(/\)/, " ", if_name[1])
#			printf("paren: %s\n", if_name[1]) > "/dev/stderr"
		}
	}
# end of parens

	n_entries = split(if_name[1], if_array, " ")
#	printf("%d %s %s %s %s %s %s\n", n_entries, if_array[1], if_array[2], if_array[3], if_array[4], if_array[5], if_array[6]) > "/dev/stderr"

# Is entire set of conditionals negated?

	if (if_array[2] == "!") {
		entire_neg = 1
#		printf("entire neg, %s, %s\n", if_header, if_name[1]) > "/dev/stderr"
	}

# determine if any of the conditionals are negated

	for (i=2; i<=n_entries; i++) {
		neg[i] = index(if_array[i],"!")
		if (neg[i]) {
			any_negs = 1
			if_array[i] = substr(if_array[i], 2)
#			printf("%dneg %s\n", i, if_array[i]) > "/dev/stderr"
		}
	}

# only one conditional
#if XXX
	if (n_entries == 2)
		process_one_conditional(if_array[2])

# two conditionals

	if (n_entries == 4) {
		if (if_array[3] == "||") {
#if XXX || YYY
			for (i=2; i<=n_entries; i+=2) {
				if_index = 1;
				while (opt_val[if_index] != -1) {
					val = opt_val[if_index]
					if (neg[i])
						val = !val
#			printf("%d|| if %s[%d] opt %s[%d] val %d\n", n_entries, if_array[i], i, opt_array[if_index], if_index, val) > "/dev/stderr"
					if ((if_array[i] == opt_array[if_index]) && val) {
						if_out = 1;
						break
					}
				if_index++
				}
			}
		if_processed = 1;
		}

		if (if_array[3] == "&&") {
#if XXX && YYY
			for (i=2; i<=n_entries; i+2) {
				if_index = 1;
				while (opt_val[if_index] != -1) {
					val = opt_val[if_index]
					if (neg[i])
						val = !val
#			printf("&& if %s[%d] opt %s[%d] val %d\n", if_array[i], i, opt_array[if_index], if_index, val) > "/dev/stderr"
					if ((if_array[i] == opt_array[if_index]) && val) {
						if_out++;
						break
					}
					if_index++
				}
			}
		if (if_out != 2)
			if_out = 0
		}
	if_processed = 1;
#	printf("if_out %x\n", if_out) > "/dev/stderr"
	}

# many conditionals but only simple ones
# if XXX || YYY || ZZZ

	if (n_entries > 4) {
		if (!any_ands) {
			for (i=2; i<=n_entries; i+=2) {
				if_index = 1;
				while (opt_val[if_index] != -1) {
					val = opt_val[if_index]
					if (neg[i])
						val = !val
#			printf("%d|| if %s[%d] opt %s[%d] val %d\n", n_entries, if_array[i], i, opt_array[if_index], if_index, val) > "/dev/stderr"
					if ((if_array[i] == opt_array[if_index]) && val) {
						if_out = 1;
						break
					}
				if_index++
				}
			}
		if_processed = 1;
		}
	}

# conditionals too complicated to process

	if (!if_processed) {
		if_out = 1;
		printf("%% warning - Too complicated, %s, %s\n", if_header, if_name[1]) > "/dev/stderr"
	}

# is entire set of conditionals negated?

	if (entire_neg) {
#		printf("entire neg, if_out %x %x, %s, %s\n", if_out, !if_out, if_header, if_name[1]) > "/dev/stderr"
		if (if_out == 0)
			if_out = 1;
		else
			if_out = 0
	}

# is IF condition satisfied ?

	if (if_out == 1) {
		if_satisfied = 1
	}
}

# process_one_conditional(conditional
# part of 2nd part, create depend file

function process_one_conditional(conditional) {
	if_index = 1;
	if_out = 0;

	while (opt_val[if_index] != -1) {
		val = opt_val[if_index]
		if (neg[2])
			val = !val
#		printf("%dif %s[%d] opt %s[%d] val %d\n", n_entries, conditional, 2, opt_array[if_index], if_index, val) > "/dev/stderr"
		if ((conditional == opt_array[if_index]) && val) {
			if_out = 1;
			break
		}
		if_index++
	}
	if_processed = 1;
}
