$! file: newbuild.com - command procedure to build the Console firmware
$goto begin
$!
$! Copyright (C) 1990,1991, 1992 by
$! Digital Equipment Corporation, Maynard, Massachusetts.
$! All rights reserved.
$!
$! This software is furnished under a license and may be used and copied
$! only  in  accordance  of  the  terms  of  such  license  and with the
$! inclusion of the above copyright notice. This software or  any  other
$! copies thereof may not be provided or otherwise made available to any
$! other person.  No title to and  ownership of the  software is  hereby
$! transferred.
$!
$! The information in this software is  subject to change without notice
$! and  should  not  be  construed  as a commitment by digital equipment
$! corporation.
$!
$! Digital assumes no responsibility for the use  or  reliability of its
$! software on equipment which is not supplied by digital.
$!
$!
$!
$!  FACILITY:
$!
$!      Common Console Development Environment
$!
$!  MODULE DESCRIPTION:
$!
$!      This command file initiates the build procedure for Alpha firmware
$!	development in the common console environment.
$!
$!    Usage:
$!	
$!	@newbuild <buildfile>
$!
$!    Parameters: 
$! 
$!       <buildfile> -  Name of the configuration file for the target
$!			to be built, normally of the form <target>_FILES.BLD.
$!			Abbreviation to just <target> is allowed.  If you have
$!			already executed CP_COMMON_LOGIN.COM for the target
$!			you wish to build, <buildfile> can be left blank.
$!
$!    Examples:
$!
$!	$ @cp$src:newbuild cp$src:cobra_files.bld
$!	$ @cp$src:newbuild cbload
$!
$!  AUTHORS:
$!
$!      AJ Beaverson
$!
$!  CREATION DATE:
$!  
$!      4-Feb-1992
$!
$!  MODIFICATION HISTORY:
$!
$!	phs	21-Jul-1993	Fix bug where it whines about needing to
$!				relogin when you don't have to.
$!	phs	12-Jul-1993	Gutted to use NEWBUILD.MMS
$!--------------------------------------------------------------------------
$ begin:
$
$ config = f$edit("''P1'","TRIM,UPCASE") ! P1 : Configuration file
$ others = f$edit("''p2' ''p3' ''p4' ''p5' ''p6' ''p7' ''p8'", "TRIM,UPCASE")
$
$ cpl = "CP_COMMON_LOGIN.COM"
$ newbuild_mms = "CP$SRC:NEWBUILD.MMS"
$
$ ! Warn people that the old NEWBUILD.COM parameters are being ignored.
$ if others .nes. ""
$ then
$     write sys$error "ignoring: ", others
$     write sys$error " (options are obtained from the build file)"
$ endif
$
$ !
$ ! Find a copy of CP_COMMON_LOGIN.COM.  Search order is:
$ !    1. CP$SRC (if defined)
$ !    2. The current directory.
$ !    3. The same directory NEWBUILD.COM is running from
$ !    4. The reference directory.
$
$ if f$trnlnm("CP$SRC") .nes. ""
$ then
$     f = f$search("CP$SRC:" + cpl)
$ endif
$ if f .eqs. ""
$ then
$     f = f$search(cpl)
$ endif
$ if f .eqs. ""
$ then
$     f = f$parse(cpl, f$environment("PROCEDURE"))
$ endif
$ if f .eqs. ""
$ then
$     f = f$parse(cpl, cmsref)
$ endif
$ if f .eqs. ""
$ then
$     write sys$error "could not find ", cpl
$     write sys$error "NEWBUILD.COM aborted"
$     exit %X10000000
$ endif
$ cpl = f
$ cpl = "''cpl'" ! debug
$ !
$ ! If CP$CFG is already defined, see if it looks reasonable and use it.
$ ! If not defined or not reasonable, abort.
$ x = f$trnlnm("CP$CFG")
$ if x .nes. ""
$ then
$     x = f$extract(f$locate("[", x) + 1, 255, x)
$     x = "''x'" ! debug
$     x = f$extract(0, f$locate("]", x), x)
$     x = "''x'" ! debug
$     last_1 = ""
$     last_2 = ""
$     dir_loop:
$         n = f$locate(".", x)
$	  n = 'n' ! debug
$         if n .ge. f$length(x) then goto dir_loop_end
$
$	  last_2 = last_1
$         last_1 = f$extract(0, n, x)
$         x = f$extract(n + 1, 255, x)
$	  x = "''x'" ! debug
$         goto dir_loop
$     dir_loop_end:
$     current_config = f$edit(x, "UPCASE")
$     current_platform = f$edit(last_2, "UPCASE")
$
$     ! Find the configuration name from config, if present.
$     if config .nes. ""
$     then
$	  config_name = config
$	  n = f$locate(":", config_name)
$	  n1 = f$locate("]", config_name)
$	  if n .gt. n1 then n = n1
$	  n1 = f$locate(">", config_name)
$	  if n .gt. n1 then n = n1
$	  if n .lt. f$length(config_name)
$	  then
$	      config_name = f$extract(n + 1, 255, config_name)
$	  endif
$	  config_name = "''config_name'" ! debug
$	  config_name = f$extract(0, f$locate("_", config_name), config_name)
$	  config_name = "''config_name'" ! debug
$	  if config_name .nes. current_config
$	  then
$	      cpl = f$extract(0, f$locate(";", cpl), cpl)
$	      write sys$error "CP$CFG is defined as ", f$trnlnm("CP$CFG")
$	      write sys$error "You asked to build ", config
$             write sys$error "To build ", config, ", you must first: @", -
			  cpl, " ", config
$	      write sys$error "NEWBUILD.COM aborted"
$	      exit %X10000000
$	  endif
$     endif
$     cp$config == current_config
$     cp$platform == current_platform
$     write sys$error "Platform = ""''cp$platform'"", ", -
		      "configuration = ""''cp$config'"""
$ else
$     write sys$error "Logging in to configuration ", config
$     @'cpl' 'config'
$ endif
$ if "''cp$config'" .eqs. "" .or. "''cp$platform'" .eqs. ""
$ then
$     write sys$error "Platform and configuration must be nonblank"
$     write sys$error "NEWBUILD.COM aborted"
$     exit %X10000000
$ endif
$ !
$ !
$ ! Invoke the build procedure.
$ !
$ f = f$search(newbuild_mms)
$ if f .eqs. ""
$ then
$     write sys$error "Could not find ", newbuild_mms
$     write sys$error "NEWBUILD.COM aborted"
$     exit %X10000000
$ endif
$ write sys$error "Executing ", f
$ mms := mms
$ mms/from/descrip='f' -
	/macro=("platform=''cp$platform'","config=''cp$config'")
$ exit $status
