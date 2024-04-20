$ ! CONSOLE_TOOLS.COM
$ !
$ ! Set up the console cross-compilation tools.
$ !
$
$ ! ASSEMBLE, and P_ASSEMBLE commands.
$ @cp$kits:[alpha-assembler.new]setup
$
$ ! P_LINK command.
$ @cp$kits:[alpha-link.new]setup
$
$ ! HAL commands.
$ @cp$kits:[alpha-assembler.hal]setup
$ @cp$kits:[alpha-assembler.hal.mapcvt]setup
$
$ ! PSART command (for formatting line drawings).
$ !set command cp$cmd:psart.cld
$ if "''f$getsyi("node_hwtype")'"  .eqs. "ALPH" -
	then goto IS_ALPHA
$
$ ! GEMCC command.
$ @cp$kits:[alpha-cc.new]setup
$ goto DONE
$
$IS_ALPHA:
$ ! SDL command.
$ !@cp$kits:[alpha-SDL]setup
$
$ ! BLISS command.
$ @cp$kits:[ALPHA-BLISS]setup
$DONE:
$ exit
