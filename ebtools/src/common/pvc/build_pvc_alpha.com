$! build_pvc_alpha.com - build PVC under VMS ALPHA
$ proc = f$environment ("PROCEDURE")
$ dire = f$parse (proc,,,"DIRECTORY")
$ dev  = f$parse (proc,,,"DEVICE")
$ dev2 = dev - ":"
$
$ set def 'dev''dire'
$ show default
$
$!
$! If  p1 = DEBUG then link with debug
$! If  p2 = NOHIDDEN then build PVC_NOHIDDEN.exe which defines NOHIDDEN to
$!		     ignore "hidden" EV5 errors.
$! If  p3 present treat it as a symbol defined (e.g. /define=rrc_compiled_code
$!
$ CC = "CC/DECC"	
$ if (P1 .NES. "DEBUG") then P1 = "NODEBUG"
$ if (P1 .EQS. "DEBUG") then op = "NOOPTIMIZE"
$ if (P1 .NES. "DEBUG") then op = "OPTIMIZE"
$ if (P3 .EQS. "")
$ THEN
$ CCSWITCHES = "/DEFINE=EV6/machine/OBJ=[.alpha]/list=[.alpha]/''P1'/''op'/G_FLOAT/SHOW=EXPANSION/STAND=VAXC"
$ ELSE
$ CCSWITCHES = "/DEFINE=EV6/DEFINE=''P3'/machine/OBJ=[.alpha]/list=[.alpha]/''P1'/''op'/G_FLOAT/SHOW=EXPANSION/LIST/STAND=VAXC"
$ ENDIF
$     LLSWITCHES = "/MAP/''p1'"
$!
$ write sys$output "Commencing build of PVC for VMS ALPHA"
$! show log evax$library
$ if (P2 .EQS. "NOHIDDEN") THEN goto nohidden
$!
$nodefine:
$ mms/cms/ignore=warning/override/descriptor = makefile.vms_alpha
$ goto leave
$nohidden:
$!
$! mms/ignore=warning/override/descriptor = makefile_nohidden.vms
$leave:
$ copy pvc_alpha.exe [.alpha]
$ exit
