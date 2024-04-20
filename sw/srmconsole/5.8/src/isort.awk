# file:	isort.awk
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
#	Indexed Sort function
#
#	This module contains a sort function which sorts an arbitrary gawk
#	array according to the order imposed by the > operator.
#
#	Ths sort is currently an insertion sort.  Should not be too bad for
#	small arrays.
#
#  AUTHORS:
#
#	Peter H. Smith
#
#  CREATION DATE:
#
#	10-JUN-1993
#
#  MODIFICATION HISTORY:
#
#	phs	07-Jun-1993	Initial revision.
#--
#
#
# isort
#
# Create an array, 'ind', which is indexed by the integers 0..n, that contains
# the indices of array 'input' in sorted order.  Return the number of entries
# in 'ind'.
#
# The entries are sorted according to the index names.
#
# This function uses an insertion sort.
#
# Variables:
#   input : Input array to sort
#   entry : Index of current entry in input
#     ind : Array to put indices into
#	i : Number of indices currently in ind
#       j : Position to insert next element at
#       k : Index for shifting indices to make space
#

function isort(input, ind,
	       key, entry, i, j, k) {

    i = 0
    for (entry in input) {
#d	if (isdbg) printf(" (%s:%d:%s) ", entry, entry, input[entry])
        for (j = 0; j < i && entry > ind[j]; j++) {}
#d	if (isdbg) printf("%d ", j)
	if (j < i) {
#d	    if (isdbg) printf(" shift %d..%d ", j, i - 1)
	    for(k = i - 1; k > j - 1; k--) {
		ind[k + 1] = ind[k]
	    }
	}
	ind[j] = entry
	i += 1
#d	if (isdbg) printf("\n")
    }
    return i
}
#
#
# vsort
#
# Create an array, 'ind', which is indexed by the integers 0..n, that contains
# the indices of array 'input' in sorted order.  Return the number of entries
# in 'ind'.
#
# The entries are sorted according to the values in 'input'.
#
# This function uses an insertion sort.
#
# Variables:
#   input : Input array to sort
#   entry : Index of current entry in input
#     ind : Array to put indices into
#	i : Number of indices currently in ind
#       j : Position to insert next element at
#       k : Index for shifting indices to make space
#

function vsort(input, ind,
	       key, entry, i, j, k) {

    i = 0
    for (entry in input) {
#d	if (isdbg) printf(" (%s:%d:%s) ", entry, entry, input[entry])
        for (j = 0; j < i && input[entry] > input[ind[j]]; j++) {}
#d	if (isdbg) printf("%d ", j)
	if (j < i) {
#d	    if (isdbg) printf(" shift %d..%d ", j, i - 1)
	    for(k = i - 1; k > j - 1; k--) {
		ind[k + 1] = ind[k]
	    }
	}
	ind[j] = entry
	i += 1
#d	if isdbg printf("\n")
    }
    return i
}
#
END {
    if (isdbg) {
	s[0] = ""
	l = isort(lines, s)
	for (i = 0; i < l; i++) {
	    printf("%d -> %d -> %s\n", i, s[i], lines[s[i]])
	}
    }
}

isdbg { lines[i++] = $0 }
