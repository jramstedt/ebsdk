! Architecture-specific setup, included by descrip.mms
!

.IFDEF ALPHA

! If this is a vms host then we need to for the linker and assembler for the
! proper state.

.IFDEF AVMS_HOST
.IFDEF EV6
cquals = /switch=small_memory/pointer=short/warn=disable=(longextern,nonmultalign,notcompat)/standard=vax/nomember/assume=nowritable/mach/obj=cp$obj/extern=strict
.ELSE
cquals = /pointer=short/warn=disable=(longextern,nonmultalign,notcompat)/standard=vax/nomember/assume=nowritable/mach/obj=cp$obj/extern=strict
.ENDIF
cxxquals = /mach/obj=cp$obj
cc = cc/instruct=nofloat/noprefix
cc_fp = cc/noprefix
ots = sys$library:starlet
assemble = macro/alpha/mac/show=binary
p_assemble = macro/alpha/mac/show=binary
hal = hal_alpha
mapcvt = mapcvt_alpha
.ELSE
cquals = /nowarn/standard=vax/nomemb/assume=noalign/mach/obj=cp$obj/switch=(space_critical,noalign_labels)/extern=strict
cxxquals = /mach/obj=cp$obj
not_native = /alpha
cc = gemcc/instruct=nofloat/noprefix
cc_fp = gemcc/noprefix
ots = cp$kits:[alpha-link.new]libots
hal = hal_vax
mapcvt = mapcvt_vax
.ENDIF

bliss = bliss/mach/check=noalign/notrace
cxx = cxx /noprefix
copt = /opt=(inline=none,unroll=1)
copt_inl = /opt=(inline=none,unroll=1)
libr = libr $(not_native)
link = link $(not_native)
list = /lis=cp$lis
macro = macro/migration/mac
macro64 = macro/alpha/mac/show=binary
sdl = sdl/alpha
.IFDEF EDEBUG
CXX_FLAGS = /include_dir=(cp$inc,cp$src)/define=(DIAG,_MSC_VER,IFACE,FIRMWARE,BIOS_EMULATION,PCI_PLATFORM,_DOS)/reentrancy=none/instr=nofloat/warn=disable=(nonmultalign)
.ELSE
CXX_FLAGS = /include_dir=(cp$inc,cp$src)/define=(_MSC_VER,IFACE,FIRMWARE,BIOS_EMULATION,PCI_PLATFORM,_DOS)/reentrancy=none/instr=nofloat/warn=disable=(nonmultalign)
.ENDIF

.first
	show time
	@cp$src:console_tools
.ENDIF

.IFDEF VAX

cquals = /mach/obj=cp$obj

bliss = bliss/mach=(obj,sym,bin,com)
cc = cc
copt = /opt=(noinline,disjoint)
copt_inl = /opt=(inline,disjoint)
libr = libr
link = link
list = /lis=cp$lis
macro = macro
sdl = sdl/vax
.first
	show time
	@cp$kits:[alpha-bliss]setup
	@cp$kits:[alpha-sdl]setup
.ENDIF
