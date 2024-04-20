$ set symbol/scope=(nolocal,noglobal)
$ set noon
$ stream1 = 1
$ pattern1 = f$parse(p1,"*.*;-1")
$next_file:
$ name = f$search(pattern1,stream1)
$ if name .eqs. "" then $ exit
$ stream2 = 2
$ pattern2 = f$extract(0,f$locate(";",name),name)
$ file = f$search(pattern2,stream2)
$ define/user sys$output nla0:
$ define/user sys$error nla0:
$ diff/mode=hex/maximum=1 'file'
$ if $status
$ then
$     write sys$output "''file' will be deleted"
$     delete/nolog 'file'
$ else
$     write sys$output "''file' must be kept"
$ endif
$ goto next_file
