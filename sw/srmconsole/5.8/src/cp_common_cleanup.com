$ ! cp_common_cleanup.com
$ !
$ ! Delete the generated files for the platform which was logged in to.
$ !
$ delete := delete/exclude=(*.dir)
$
$ delete cp$cfgl:*.*;*
$ delete cp$exel:*.*;*
$ delete cp$incl:*.*;*
$ delete cp$lisl:*.*;*
$ delete cp$objl:*.*;*
$ delete cp$srcl:*.*;*
$
$ exit
