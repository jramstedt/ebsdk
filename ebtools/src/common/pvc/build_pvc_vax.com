$! build_pvc.com - build PVC under VMS
$!
$! If  p1 = DEBUG then link with debug
$! If  p2 = NOHIDDEN then build PVC_NOHIDDEN.exe which defines NOHIDDEN to
$!		     ignore "hidden" EV5 errors.
$! If  p3 present treat it as a symbol defined (e.g. /define=rrc_compiled_code
$!
$ CC = "CC/DECC"	
$ if (P1 .NES. "DEBUG") then P1 = "NODEBUG"
$ if (P3 .EQS. "")
$ THEN
$ CCSWITCHES = "/DEFINE=EV6/machine/OBJ/''P1'/NOOPTIMIZE/G_FLOAT/SHOW=EXPANSION/LIST/STAND=VAXC"
$ ELSE
$ CCSWITCHES = "/DEFINE=EV6/DEFINE=''P3'/machine/OBJ/''P1'/NOOPTIMIZE/G_FLOAT/SHOW=EXPANSION/LIST/STAND=VAXC"
$ ENDIF
$     LLSWITCHES = "/MAP/''p1'"
$!
$ write sys$output "Commencing build of PVC for VAXen"
$! show log evax$library
$ if (P2 .EQS. "NOHIDDEN") THEN goto nohidden
$!
$nodefine:
$ mms/cms/ignore=warning/override/descriptor = makefile.vms
$ goto leave
$nohidden:
$!
$ mms/ignore=warning/override/descriptor = makefile_nohidden.vms
$leave:
$ exit
