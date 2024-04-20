# file: psect.awk  --  summarize psections
#
#$CODE$                          00016028 00016028 00000000 (          0.) QUAD 3   PIC,USR,CON,REL,LCL,  SHR,  EXE,NORD
#                XDELTA          00016028 00016028 00000000 (          0.) QUAD 3


$0 ~ /Program Section Synopsis/ {
	within_psect = 1
}
$0 ~ /Symbols By Name/ {
	within_psect = 0
}

within_psect && $0 ~ /^[A-Z0-9\$_]+ +[A-F0-9]+ [A-F0-9]+ [A-F0-9]+ \( +/ {
	if ($4 != "00000000") {
	    psects [$2 " " $3 " " $4 " " $1]++
	}
}

END {
	for (i in psects) {
	    print i
	}
}
