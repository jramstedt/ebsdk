$ goto begin
$!
$! file: cp_common_login.com
$!
$! Copyright (C) 1992, 1993 by
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
$!++
$! FACILITY:
$!
$!	Common Console Firmware Development Environment
$!
$!  MODULE DESCRIPTION:
$!
$!	Set up logicals to allow building one console configuration.
$!	Also set the default working directory and the CMS library.
$!
$!	Provide support for backing against nightly builds.  Objects and
$!	listings from the backing trees are used where possible, to cut
$!	down build time.
$!
$! usage:
$!
$!	@alpha_fw:[cobra_fw.ref]cp_common_login [config] [root] [back]
$!
$!	@alpha_fw:[cobra_fw.ref]cp_common_login "/ROOT=root/CONFIG=config/BACK=list"
$!
$! parameters:
$!
$!      p1 - Specifies the name of the configuration file to use to set up
$!	     the platform logicals.  The platform and configuration name are
$!	     obtained from that file.  If omitted, only some basic logical
$!	     names are set up.
$!	p2 - Specifies the location of the working directory for the build.
$!           If omitted, the root is derived from the current directory.
$!	p3 - A list of one or more backing trees for the specified target.
$!	     Elements of the list should be separated with commas.
$!
$! examples:
$!
$!	@alpha_fw:[cobra_fw.cmd]cp_common_login COBRA [user.FW]
$!
$! Using a Backing Tree:
$!
$!	@alpha_fw:[cobra_fw.ref]cp_common_login COBRA [user.FW] MONDAY
$!
$! Ending use of a backing tree:
$!
$!	@alpha_fw:[alpha_fw.cmd]cp_common_login cobra [.fw] ref
$!
$!	If the reserved word "REF" is used for P3, the use of a sticky backing
$!	tree is discontinued; the logicals are defined as in the first example,
$!	and userdisk:[user.FW.COBRA]CP$BACK.DAT is deleted.
$!
$! Changing Platform directory attributes:
$!
$! Using sparse backing trees (this is still untested!):
$!
$!	$ define tuesday	tuesday,monday
$!      $ define wednesday	wednesday,tuesday
$!	$ define thursday	thursday,wednesday
$!	$ define friday		friday,thursday
$!      $ cobra == "@alpha_fw:[alpha_fw.cmd]cp_common_login cobra me:[fw]"
$!	$ cobra ref
$!	$ ! build monday
$!	$ cobra tuesday
$!	$ ! build tuesday
$!	...
$!	$ cobra friday
$!	$ ! build friday
$!
$! (You need to look at CP$BACK.DAT in the backing tree you intend to back to,
$!  in order to determine whether it is sparse.  Room for automation here...).
$!
$!  AUTHORS:
$!
$!      AJ Beaverson
$!
$!  CREATION DATE:
$!
$!      ?
$!
$!  MODIFICATION HISTORY:
$!
$!	jsz	19-apr-1996	Fixed incorrect handling of non-existent
$!				backing trees.	
$!
$!      djl     23-JAN-1996     Back against LATENITE:[CONSOLE...] if 
$!                              building on an Alpha system.
$!
$!	jsz	4-apr-1995	Back against NIGHTRIDER:[CONSOLE...] if 
$!				building on an Alpha system.
$!
$!	dtr	20-apr-1994	Added a flag to indicate host alpha vms system.
$!				This elminates the need to do it manually.
$!
$!	phs	20-JUL-1993	Remove outdated comments.  Note when the base
$!				configuration is being selected.  Change the
$!				CPL*OGIN command to look up this file in
$!				CP$SRC, so that logging in twice in a row
$!				guarantees that the logicals are set up the
$!				way the backing tree expects.
$!	phs	18-JUN-1993	Add CP$ROOT, CP$CFG, CP$OBJL to prepare for
$!				sharing of objects accross configurations.
$!	phs	13-JUN-1993	Control "set verify" with symbol cp$v_cpl.
$!				Expand each backing tree into a search list
$!				of three places: .platform.SRC], .SRC], and
$!				.COMMON.SRC] (build.com will fetch sources
$!				to .COMMON.SRC]).
$!	phs	08-JUN-1993	[.OBJ] and [.LIS] directories should have
$!				version_limit=1
$!	phs	07-JUN-1993	Fix definitions of CP$TMP, CP$DOC to accomodate
$!				null platform parameter.
$!	phs	04-JUN-1993	Do more second guessing if the build root was
$!				not specified.  Works around build problems.
$!	phs	28-APR-1993	Make backing trees a bit more flexible, by
$!				allowing backing to a search list.  Handy for
$!				debugging build.com without fetching the world,
$!				and useful for sparse nightlies.
$!      phs	22-APR-1993	Add history to header.
$!				Modify directory creation to default to a
$!				version limit of 2 if the platform directory
$!				does not already exist (otherwise, default to
$!				the attributes of the platform directory).
$!				Also, make all platform subdirectories deletable
$!				by owner.
$!	phs	22-APR-1993	Fix behavior when no platform is specified.
$!--
$ begin:
$! To turn on verification, set the symbol cp$v_cpl to 1.
$ verify_save = f$verify(f$integer("''cp$v_cpl'"))
$ !set on
$ !on control_y then goto abort
$ !on warning then goto unexpected_error
$ if f$getsyi("cluster_member","apsled") .nes. "TRUE"
$ then
$      if f$getsyi("hw_model") .lt. 1000
$      then
$          define/nolog cp$kits alpha_fw:[kits.] /trans=conceal
$      else
$          define/nolog cp$kits alpha_fw:[kits.axp.] /trans=conceal
$          avms_host :== 1	! force the rest of the build
$      endif
$ else
$      if f$getsyi("hw_model") .lt. 1000
$      then
$          define/nolog cp$kits user4:[alpha_fw.kits.] /trans=conceal
$      else
$          define/nolog cp$kits user4:[alpha_fw.kits.axp.] /trans=conceal
$          avms_host :== 1	! force the rest of the build
$ endif
$ endif
$!
$!
$! Input Parameters
$!
$ configfile = f$edit("''P1'","COLLAPSE,UPCASE,TRIM") ! P1
$ default = "''P2'" ! P2 : Default Directory
$ back = f$edit("''P3'","COLLAPSE,UPCASE,TRIM") ! P3
$ back = "''back'" ! debug
$
$ query = f$mode() .eqs. "INTERACTIVE"
$!
$! Name of this procedure
$!
$ thisproc = f$environment("PROCEDURE")
$!
$! Location of CMS Reference Directory.  Have to get it into a canonical form
$! for the backing tree stuff to work right.
$!
$ cmsref := ALPHA_FW:[COBRA_FW.REF]
$ cmsref = f$parse(cmsref,,,,"SYNTAX_ONLY")
$ cmsref = f$extract(0, f$locate("]", cmsref) + 1, cmsref)
$ cmsref = "''cmsref'" ! debug
$
$ ! Prefix for default backing trees.
$ if f$getsyi("hw_model") .lt. 1000
$ then 
$     back_prefix = "CFW:[CONSOLE."		! if building on a Vax
$ else 
$     back_prefix = "LATENITE:[CONSOLE."      ! if building on an Alpha
$ endif
$ back_prefix_len = f$length(back_prefix)
$!
$!
$! If the default directory wasn't specified, make some educated guesses as
$! to where we should be.
$!
$ if default .nes. "" 
$ then
$     !
$     ! Default was specified.  Make sure it exists.
$     !
$     temp = f$parse(default)
$     x = f$locate("]", temp)
$     x1 = f$length(temp)
$     temp = "''temp'" ! debug
$     x = 'x' ! debug
$     x1 = 'x1' ! debug
$     temp = f$extract(0, x + 1, temp)
$     temp = "''temp'" ! debug
$     if f$parse(temp) .eqs. "" .or. x .ge. x1 .or. x .lt. x1 - 3
$     then
$	  write sys$error "Could not find working directory ", default
$	  goto error
$     endif
$     if temp .nes. "" then default = temp
$     default = "''default'" ! debug
$ else
$     !
$     ! Default was not specified.  Use current default.
$     !
$     default = f$environment("default")
$     default = "''default'" ! debug
$ endif
$ !
$ ! If the default or requested default is named "SRC", go up one level.
$ !
$ default = "''default'" ! debug
$ default_len = f$length(default)
$ if default_len .gt. 3
$ then
$     if f$extract(default_len - 4, 3, default) .eqs. "SRC"
$     then
$	  default = f$extract(0, default_len - 5, default)
$	  default = default + "]"
$	  default = "''default'" ! debug
$     endif
$ endif
$ !
$ !
$ ! Set up root directory name.
$ !
$ root = f$extract(0,f$locate("]",default),default)
$ root = "''root'" ! debug
$ !
$ ! Set up literal name of root, for definition of concealed logicals.
$ !
$ litroot = default
$ litroot = "''litroot'" ! debug
$ colon = f$locate(":", litroot)
$ colon = 'colon' ! debug
$ prefix = f$extract(0, colon, default)
$ prefix = "''prefix'" ! debug
$ if f$trnlnm(prefix,,,,,"CONCEALED")
$ then
$     trans = f$trnlnm(prefix)
$     trans = "''trans'" ! debug
$     len = f$length(trans)
$     if f$extract(len - 2, 2, trans) .eqs. ".]"
$     then
$	  litroot = f$extract(0, len - 1, trans) + -
		    f$extract(colon + 2, 255, litroot)
$     endif
$ endif
$ litroot = f$extract(0, f$length(litroot) - 1, litroot)
$ litroot = "''litroot'" ! debug
$ !
$ ! If no configuration file was specified, clear cp$platform.
$ if configfile .eqs. ""
$ then
$     cp$architecture == ""
$     cp$platform == ""
$     cp$configuration == ""
$     backfile = root + "]CP$BACK.DAT"
$ else
$     ! See if the configuration file was named explicitly.  If it wasn',
$     ! fill it out according to some defaulting rules.  Search for the
$     ! file and put the result in x.
$     configfile = "''configfile'" ! debug
$     configfile_len = f$length(configfile)
$     configfile_len = 'configfile_len' ! debug
$     x = f$locate(":", configfile)
$     x = 'x' ! debug
$     x = f$locate("]", configfile)
$     x = 'x' ! debug
$     x = f$locate(">", configfile)
$     x = 'x' ! debug
$     explicit = ((f$locate(":", configfile) .lt. configfile_len) -
		  .or. (f$locate("]", configfile) .lt. configfile_len) -
		  .or. (f$locate(">", configfile) .lt. configfile_len))
$
$     if explicit
$     then
$	  x = f$search(configfile)
$     else
$	  ! Provide _FILES if not there.
$	  if f$locate("_", configfile) .ge. configfile_len
$	  then
$	      x = f$locate(".", configfile)
$	      configfile =  f$extract(0, x, configfile) + -
			    "_FILES" + -
			    f$extract(x, 255, configfile)
$	      configfile = "''configfile'" ! debug
$	  endif
$	
$	  ! Default the file type to .BLD.
$	  configfile = f$parse(configfile,,,"NAME") -
		       + f$parse(configfile,".BLD",,"TYPE")
$	  configfile = "''configfile'" ! debug
$
$	  ! Search for the file in a variety of places.
$         x = f$search("CP$SRC:" + configfile)
$	  if x .eqs. "" then x = f$search(root + ".SRC]" + configfile)
$	  if x .eqs. "" then x = f$search(cmsref + configfile)
$     endif
$
$     ! Complain if the configuration file was not found.
$     if x .eqs. ""
$     then
$	  write sys$error "Could not find configuration file: ", configfile
$	  !
$	  ! To support half uses of the build environment, such as
$	  ! medulla_srom, take the configfile name as the platform name if
$	  ! if the configuration was not found.
$	  !
$	  configfile = configfile - ".BLD"
$	  write sys$error "Using ", configfile, " as platform name"
$	  cp$platform == configfile
$	  cp$config == configfile
$	  cp$arch == "ALPHA"
$	  goto skip_read
$     endif
$
$     configfile = x
$     !
$     !
$     ! Look for bldcpl.awk
$     !
$     bldcpl = "BLDCPL.AWK"
$     x = f$search("CP$SRC:" + bldcpl)
$     if x .eqs. "" then x = f$search(root + ".SRC]" + bldcpl)
$     if x .eqs. "" then x = f$search(cmsref + bldcpl)
$     if (x .eqs. "")
$     then
$	  write sys$error "Could not find file: ", bldcpl
$	  goto error
$     endif
$     bldcpl = x
$
$     ! Generate a scratch file name.
$     scratch = "SYS$SCRATCH:CPL_" + f$getjpi(0,"PID") + ".COM"
$
$     !
$     ! If cp$src is undefined, define it to be something.  This is needed
$     ! for processing of include directives in the .bld file.
$     !
$     if f$trnlnm("CP$SRC") .eqs. ""
$     then
$	  define CP$SRC 'root'.SRC],'cmsref'
$     endif
$
$     !
$     ! Parse the configuration file, and use a gawk output script to generate
$     ! a DCL stub which defines global symbols for the architecture, platform,
$     ! and configuration.  Have to define the gawk symbol now in order for this
$     ! to work.
$     !
$     write sys$error "Reading configuration file ", configfile
$     gawk := $cp$kits:[gawk]gawk
$     on error then goto error_scratch
$     on control_y then goto abort_scratch
$         gawk -f 'bldcpl' < 'configfile' > 'scratch'
$         @'scratch'
$     on control_y then goto abort
$     on warning then goto unexpected_error
$     delete 'scratch';
$     !
$     ! Provide backward compatability with older .bld files.
$     !
$     configfile_len = f$length(configfile)
$     n = f$locate("]", configfile)
$     if n .ge. configfile_len
$     then
$	  n = f$locate(">", configfile)
$	  if (n .ge. configfile_len)
$	  then
$	      n = f$locate(":", configfile)
$             if (n .ge. configfile_len) then n = 0
$         endif
$     endif
$     x = f$extract(n + 1, 255, configfile)
$     x = f$extract(0, f$locate("_", x), x)
$
$     if cp$platform .eqs. "" then cp$platform == x
$     if cp$config .eqs. "" then cp$config == x
$     if cp$arch .eqs. "" then cp$arch == "ALPHA"
$     !
$     ! Announce the platform.
$     !
$  skip_read:
$     if cp$platform .eqs. cp$config
$     then
$	  write sys$error "Platform is ", cp$platform, -
			  " (base configuration), architecture is ", cp$arch
$     else
$	  write sys$error "Platform is ", cp$platform, ", configuration is ", -
			  cp$config, ", architecture is ", cp$arch
$     endif
$     !
$     !
$     ! Do some more work on guessing where we should be.
$     !
$     !
$     ! If the current directory has the platform name, go up one level.
$     !
$     platform_len = f$length(cp$platform)
$     platform_len = 'platform_len' ! debug
$     default_len = f$length(default)
$     default_len = 'default_len' ! debug
$     if platform_len .gt. 0 .and. default_len .gt. platform_len
$     then
$	  tail = f$extract(default_len - platform_len - 1, platform_len, -
			   default)
$	  tail = "''tail'" ! debug
$	  if tail .eqs. cp$platform
$	  then
$	      default = f$extract(0, default_len - platform_len - 2, default)
$	      default = default + "]"
$	      default = "''default'" ! debug
$	      root = f$extract(0, f$length(root) - platform_len - 1, root)
$	      root = "''root'" ! debug
$	      litroot = f$extract(0, f$length(litroot) - platform_len - 1, litroot)
$	      litroot = "''litroot'" ! debug
$	  endif
$     endif
$     rootp = root + "." + cp$platform
$     rootp = "''rootp'" ! debug
$     backfile = rootp + "]CP$BACK.DAT"
$     !
$     ! If the platform work directory does not already exist, then ask if it
$     ! should be created.  If the answer is "No", then the user specified the
$     ! wrong work directory (or set default to the wrong place), so just abort.
$     !
$     workdir = rootp + "]"
$     workdirfile = root + "]" + cp$platform + ".DIR"
$     call mkdir 'workdir' "/version_limit=2/protection=(owner:rwed)" 'query'
$     if f$parse(workdir) .eqs. ""
$     then
$	  write sys$error "Aborted ''thisproc'"
$	  goto error
$     endif
$ endif
$ !
$ !
$ ! If a list of backing trees was specified, parse it.  Otherwise, look for
$ ! a file in the work directory that holds the list.  If none is present,
$ ! back against CP$REF.
$ !
$ if back .nes. "" 
$ then
$     ! Parse the backing tree list
$     backlist = ""
$     back = "''back'" ! debug
$     pbl_element_loop:
$	  if back .eqs. "" then goto pbl_element_loop_end
$
$	  ! Get current element
$	  p = f$locate(",", back)
$	  p = 'p' ! debug
$
$	  ! Remove current element
$	  x = f$extract(0, p, back)
$	  back = f$extract(p + 1, 255, back)
$	  x = "''x'" ! debug
$	  back = "''back'" ! debug
$
$	  ! Process the current element
$	  x = f$extract(0, f$locate("]", x) + 1, x)
$	  x = f$edit(x,"TRIM,UPCASE")
$	  x = "''x'" ! debug
$
$	  if x .eqs. "REF" .or. x .eqs. "CP$REF"
$	  then
$	      x = "CP$REF"
$	      goto pbl_add
$	  endif
$	  !
$	  ! If it's a logical name, translate it.
$	  ! Names with a leading "_" are treated as terminal (the "_" is
$	  ! removed, though).
$    	  !
$	  terminal = (f$extract(0, 1, x) .eqs. "_")
$	  if terminal then x = f$extract(1, 255, x)
$
$	  ! See if it's a translatable logical name.
$	  if .not. terminal .and. f$trnlnm(x) .nes. ""
$	  then
$	      ! If it's a search list, expand it and loop.
$	      terminal = f$trnlnm(x,,,,,"TERMINAL") .eqs. "TRUE"
$	      terminal = 'terminal' ! debug
$	      if f$trnlnm(x,,,,,"MAX_INDEX") .gt. 0
$	      then
$		  !
$	          ! To allow self-referential simple names, use the convention
$		  ! of prefixing self-referential names with "_".
$	          !
$	          n1 = 0
$	          x1 = ""
$		  pbl_subelement_loop:
$		      x2 = f$trnlnm(x,,n1)
$		      if terminal .or. x2 .eqs. x then x2 = "_" + x2
$		      x1 = x1 + x2 + ","
$		      if n1 .ge. f$trnlnm(x,,,,,"MAX_INDEX")
$		      then
$			  goto pbl_subelement_loop_end
$		      endif
$
$		      pbl_next_subelement:
$		      n1 = n1 + 1
$		      goto pbl_subelement_loop
$		  pbl_subelement_loop_end:
$	          x1 = "''x1'" ! debug
$	          back = x1 + back
$	          back = "''back'" ! debug
$	      else ! Translation is not a search list
$		  x2 = f$trnlnm(x)
$		  if terminal .or. x2 .eqs. x then x2 = "_" + x2
$		  back = x2 + "," + back
$	          back = "''back'" ! debug
$	      endif
$	      goto pbl_next_element
$         endif ! translatable logical name.
$         !
$         ! Allow use of simple names to specify standard backing trees.
$         !
$	  x1 = f$parse(back_prefix + x + "]")
$	  if x1 .nes. ""
$	  then
$	      x = f$extract(x1, f$locate("]", x1) + 1, x1)
$	  else
$	      ! See if it's a literal path
$             if f$logical(x) .eqs. "" .and. f$locate("[",x) .eq. f$length(x)
$             then
$		  write sys$error "unrecognized backing tree ", x, -
				  " ignored"
$		  goto pbl_next_element
$             else
$	          x1 = f$parse(x)
$	          x = x1 - ".;"
$             endif
$         endif
$	  x = "''x'" ! debug
$
$	  ! Catch literal references to cp$ref
$	  if x .eqs. cmsref then x = "CP$REF"
$
$	  pbl_add:
$	  backlist = backlist + "," + x
$	  pbl_next_element:
$         goto pbl_element_loop
$     pbl_element_loop_end:
$     backlist = f$extract(1, 255, backlist)
$     if backlist .eqs. ""
$     ! If we got here, then the backing tree name that was entered does
$     ! not exist. Use the previous backing tree if it exists in CP$BACK.DAT
$     ! Otherwise, back against cp$ref.
$     then
$         if f$search("''backfile'") .nes. ""
$         then
$             open/read bf 'backfile'
$	      read bf backlist
$	      close bf
$             if backlist .eqs. ""
$                 then backlist = "CP$REF"
$             endif
$         else
$	      backlist = "CP$REF"
$         endif
$     endif
$     !
$     !
$     ! Compare the current backing tree list to what was there before.    
$     !	  
$     if f$search(backfile) .nes. ""
$     then
$	  !
$	  ! Read the file containing the name of the previous list.
$	  !
$	  open/read bf 'backfile'
$	  read bf temp
$	  close bf
$	  temp = "''temp'" ! debug
$
$	  if temp .nes. backlist
$	  then
$             write sys$error "backed against ", backlist
$             write sys$error "         ( was ", temp, " )"
$	      delete 'backfile';
$	      if backlist .eqs. "CP$REF" .or. backlist .eqs. cmsref
$	      then
$		  backfile = ""
$	      endif
$	  else
$	      backfile = "" ! No need to write the file
$	  endif
$     else ! Backfile doesn't exist
$	  if backlist .nes. cmsref .and. backlist .nes. "CP$REF"
$	  then
$             write sys$error "backed against ", backlist
$             write sys$error "         ( was CP$REF )"
$	  else
$	      backfile = "" ! No need to write the file.
$	  endif
$     endif
$     !
$     ! Write the new list to the file.
$     !
$     if backfile .nes. ""
$     then
$	  if backlist .nes. cmsref .and. backlist .nes. "CP$REF"
$	      then
$	      backfile = f$parse(backfile,,,,"SYNTAX_ONLY")
$	      backfile = "''backfile'" ! debug
$	      open/write bf 'backfile'
$	      write bf "''backlist'"
$	      close bf
$	  endif
$     endif
$ !
$ else
$     !
$     ! No backing tree list was specified.  Try to find the file that holds
$     ! the list.  If it's not there, use CP$REF as the backing tree.
$     !
$     if f$search("''backfile'") .nes. ""
$     then
$         open/read bf 'backfile'
$	  read bf backlist
$	  close bf
$         if backlist .eqs. ""
$             then backlist = "CP$REF"
$         endif
$	  write sys$error "backed against ", backlist
$     else
$	  backlist = "CP$REF"
$     endif
$ endif
$ !
$ !
$ ! Create the platform subdirectories.
$ !
$ if cp$platform .nes. "" 
$ then 
$     diratts = f$file_attributes(workdirfile, "PRO")
$     diratts = "/PROTECTION=(''diratts')"
$     call mkdir 'rootp'.src] "''diratts'"
$     call mkdir 'rootp'.obj] "''diratts'/version_limit=1"
$     call mkdir 'rootp'.lis] "''diratts'/version_limit=1"
$     call mkdir 'rootp'.exe] "''diratts'"
$     call mkdir 'rootp'.log] "''diratts'"
$     call mkdir 'rootp'.cfg] "''diratts'"
$     call mkdir 'rootp'.cfg.'cp$config'] "''diratts'/version_limit=3"
$     call mkdir 'rootp'.cfg.'cp$config'.inc] "''diratts'/version_limit=3"
$     call mkdir 'rootp'.cfg.'cp$config'.log] "''diratts'/version_limit=1"
$     !
$     ! The base configuration drops its files in the old place, for
$     ! compatability with the old scheme of things.  Don't create config
$     ! directories which won't be used.
$     !
$     if cp$config .nes. cp$platform
$     then
$	  call mkdir 'rootp'.cfg.'cp$config'.exe] "''diratts'/version_limit=1"
$	  call mkdir 'rootp'.cfg.'cp$config'.lis] "''diratts'/version_limit=1"
$	  call mkdir 'rootp'.cfg.'cp$config'.obj] "''diratts'/version_limit=1"
$     endif
$ endif
$ !
$ !
$ ! defsrch -- Define a search list logical
$ !
$ defsrch: subroutine
$     lnm = "''P1'"	! P1: Logical name to define
$     head = "''P2'"	! P2: Head of search list
$     back = "''P3'"	! P3: Backing list to expand
$     nref = "''P4'"	! P4: List of FAO control strings
$     ref = "''P5'"	! P5: Alternate list of FAO control strings for CP$REF
$     args = P6		! P6: List of FAO arguments 2..n
$
$     exp = ""		! Expanded backing list
$     expn = 0		! Position in backing list
$     defsrch_back_loop:
$	  b = f$element(expn, ",", back)
$	  if b .eqs. "," then goto defsrch_back_loop_end
$
$	  if b .eqs. "CP$REF"
$	  then
$	      ctl = ref
$	  else
$	      ctl = nref
$	      b = b - "]"
$	  endif
$
$	  cn = 0
$	  defsrch_ctl_loop:
$	      c = f$element(cn, ",", ctl) 
$	      if c .eqs. "," then goto defsrch_ctl_loop_end
$
$	      if ctl .nes. ""
$	      then
$		  exp = exp + "," + f$fao(c, b, 'args')
$	      endif
$	      cn = cn + 1
$	      goto defsrch_ctl_loop
$	  defsrch_ctl_loop_end:
$
$     expn = expn + 1
$     goto defsrch_back_loop
$ defsrch_back_loop_end:
$ exp = "''exp'" ! debug
$ if head .eqs. "" then exp = f$extract(1, 255, exp)
$ exp = "''exp'" ! debug
$ define/nolog 'lnm' 'head' 'exp'
$ endsubroutine
$ !
$ !
$ ! Define build logicals.
$ !
$
$ ! Locations of common stuff
$ define/nolog cp$cmd		alpha_fw:[alpha_fw.cmd]
$ define/nolog cp$cms		alpha_fw:[cobra_fw.cms]
$ define/nolog cp$common	alpha_fw:[alpha_fw.cp]
$ define/nolog cp$mop		wrk:[mopload]
$ define/nolog cp$ref		alpha_fw:[cobra_fw.ref]
$ define/nolog cp$sdml		alpha_fw:[alpha_fw.sdml]
$ define/nolog cp$sync		alpha_fw:[alpha_fw.sync]
$
$ ! Determined by root location.
$ define/nolog cp$		'root']
$ define/nolog cp$doc		'root'.doc]
$ define/nolog cp$root		'litroot'.]		/trans=conceal
$ define/nolog cp$specific	'root']
$ define/nolog cp$doc		'root'.doc]
$
$ ! Backward compatability.
$ define/nolog cp$tmp		sys$scratch
$
$ ! Different depending on whether a platform was specified.
$ if cp$platform .eqs. ""
$ then
$     ! No platform -- just common logicals plus CP$SRC are defined.
$     call defsrch CP$SRC "''root'.SRC]" -
		   "''backlist'" -
		   "!AS.SRC],!AS.COMMON.SRC]" -
		   "!AS" """''cp$platform'"",""''cp$config'""" -
		   
$ else
$
$     ! Platform specified.  The reference to CP$SRC is a hack to allow
$     ! building even though some include files have moved to CP$SRC.
$     ! Those includes should be weeded out with a gawk script.  The
$     ! known problems are:
$     !   msgnum.h builtins.c dst.c encap.c help_test.h msgtext.c
$     !   platform.h (which should go away)
$     !
$     call defsrch CP$SRC "''rootp'.SRC],''root'.SRC],CP$CFG" -
		   "''backlist'" -
		   "!AS.!AS.SRC],!AS.SRC],!AS.COMMON.SRC]" -
		   "!AS" """''cp$platform'"",""''cp$config'"""
$     call defsrch CP$CFG "''rootp'.CFG.''cp$config']" -
		   "''backlist'" -
		   "!AS.!AS.CFG.!AS]" -
		   "" """''cp$platform'"",""''cp$config'"""
$     call defsrch CP$LOG "''rootp'.LOG]" -
		   "''backlist'" -
		   "!AS.!AS.LOG]" -
		   "" """''cp$platform'"""
$
$     ! Different depending on whether this is the base configuration.
$     if cp$config .eqs. cp$platform
$     then
$
$	  ! This is the base configuration
$	  define/nolog cp$cfgl 'rootp'.CFG.'cp$config']
$	  define/nolog cp$exel 'rootp'.EXE]
$	  define/nolog cp$incl 'rootp'.CFG.'cp$config'.INC]
$	  define/nolog cp$lisl 'rootp'.LIS]
$	  define/nolog cp$objl 'rootp'.OBJ]
$	  define/nolog cp$srcl 'rootp'.SRC]
$	  call defsrch CP$INC "''rootp'.CFG.''cp$platform'.INC]" -
		       "''backlist'" -
		       "!AS.!AS.CFG.!-!AS.INC]" -
		       "" """''cp$platform'"""
$	  call defsrch CP$EXE "''rootp'.EXE]" -
		       "''backlist'" -
		       "!AS.!AS.EXE]" -
		       "" """''cp$platform'"""
$	  call defsrch CP$LIS "''rootp'.LIS]" -
		       "''backlist'" -
		       "!AS.!AS.LIS]" -
		       "" """''cp$platform'"""
$	  call defsrch CP$OBJ "''rootp'.OBJ]" -
		       "''backlist'" -
		       "!AS.!AS.OBJ]" -
		       "" """''cp$platform'"""
$     else
$
$	  ! This is not the base configuration
$	  rc = rootp + ".CFG." + cp$config
$	  rp = rootp + ".CFG." + cp$platform
$	  define/nolog cp$cfgl 'rc']
$	  define/nolog cp$exel 'rc'.EXE]
$	  define/nolog cp$incl 'rc'.INC]
$	  define/nolog cp$lisl 'rc'.LIS]
$	  define/nolog cp$objl 'rc'.OBJ]
$	  define/nolog cp$srcl 'rc'.INC] ! Compatability only
$	  call defsrch CP$INC "''rc'.INC],''rp'.INC]" -
		       "''backlist'" -
		       "!AS.!AS.CFG.!AS.INC],!AS.!AS.CFG.!-!AS.INC]" -
		       "" """''cp$platform'"",""''cp$config'"""
$	  call defsrch CP$EXE "''rc'.EXE],''rootp'.EXE]" -
		       "''backlist'" -
		       "!AS.!AS.CFG.!AS.EXE],!AS.!AS.EXE]" -
		       "" """''cp$platform'"",""''cp$config'"""
$	  call defsrch CP$LIS "''rc'.LIS],''rootp'.LIS]" -
		       "''backlist'" -
		       "!AS.!AS.CFG.!AS.LIS],!AS.!AS.LIS]" -
		       "" """''cp$platform'"",""''cp$config'"""
$	  call defsrch CP$OBJ "''rc'.OBJ],''rootp'.OBJ]" -
		       "''backlist'" -
		       "!AS.!AS.CFG.!AS.OBJ],!AS.!AS.OBJ]" -
		       "" """''cp$platform'"",""''cp$config'"""
$     endif
$ endif
$ !
$ !
$ ! ********************   TOOL DEFINITIONS   ************************
$ !
$ awk :== @cp$cmd:awk.com
$ adh :== $cp$cmd:add_header
$ !
$ ! Define the symbol cplogin to invoke this procedure.
$ !
$ cpl*ogin == "@CP$SRC:" + -
	      f$parse(thisproc,,,"NAME") + f$parse(thisproc,,,"TYPE")
$ !
$ ! Define the symbol cpsetup to invoke a procedure to set up the cross-
$ ! compilation tools.
$ !
$ cps*etup :== @cp$src:console_tools.com
$ !
$ ! Symbol to clean up generated files from the current build tree.
$ !
$ cpc*leanup :== @cp$src:cp_common_cleanup.com
$ !
$ ! Set up the gawk command.
$ !
$ gawk :== $cp$kits:[gawk]gawk
$ !
$ ! Set the CMS library.
$ !
$ cms set lib cp$cms
$ !
$ ! Go to the user source directory, if it exists.  Otherwise, go to the
$ ! platform source directory.
$ !
$ srcdir = root + ".SRC]"
$ srcdir = "''srcdir'" ! debug
$ call mkdir 'srcdir' "" 'query'
$ if f$search("''root']SRC.DIR") .nes. ""
$ then
$     set default 'srcdir'
$ else
$     srcdir = rootp + ".SRC]"
$     if f$search("''rootp']SRC.DIR") .nes. ""
$     then
$         set default 'srcdir'
$     else
$         write sys$error "Directory ''srcdir' does not exist -- ", -
		          "default unchanged"
$     endif
$ endif
$ show default
$!
$! Normal exit
$!
$ verify_save = f$verify(verify_save)
$ exit
$ !
$ ! mkdir -- See if a directory exists, and create it if it doesn't.
$ !	    If prompt is on, ask before creating the directory.
$ !
$ mkdir: subroutine
$ dir = "''P1'" ! P1
$ modifiers = "''P2'" ! P2
$ prompt = f$integer("''P3'") ! P3
$
$ if f$parse(dir) .eqs. ""
$ then
$     if prompt
$     then
$       reprompt:
$	  inquire ans "Create directory ''dir' ? (Y/[N]) "
$	  doit = f$integer(ans)
$     else
$     doit = 1
$     endif
$     if doit
$     then
$         create/directory'modifiers' 'dir'
$         if .not. prompt then write sys$error "created directory ", dir
$     endif
$   quit:
$ endif
$ endsubroutine
$ !
$ !
$ ! Error handlers
$ !
$ abort_scratch:
$     sts = $status
$     set noon
$     if f$search(scratch) .nes. "" then delete 'scratch';
$     goto abort_common
$ abort:
$     sts = $status
$     set noon
$ abort_common:
$     write sys$error thisproc, " aborted by control-y"
$     goto error_common
$
$ error_scratch:
$     sts = $status
$     set noon
$     if f$search(scratch) .nes. "" then delete 'scratch';
$     goto error_common
$ unexpected_error:
$     sts = $status
$     set noon
$     write sys$error thisproc, " aborted due to unexpected error"
$     goto error_common
$
$ error:
$     sts = $status
$     set noon
$ error_common:
$     verify_save = f$verify(verify_save)
$     if sts then sts = %X10000000
$ exit 'sts'
