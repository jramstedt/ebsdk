! Console setup, included by descrip.mms
!
cquals = /warn=enable=questcode/warn=disable=(nonmultalign,notcompat,protoscope)/standard=vax/assume=nowritable/mach/obj=cp$obj/extern=strict/show=brief
cc = cc/instruct=nofloat/noprefix/pointer_size=32
cc64 = cc/instruct=nofloat/noprefix/pointer_size=64
cc56 = cc/instruct=nofloat/noprefix
cquals56 = /warn=disable=(nonmultalign,notcompat,protoscope)/standard=vax/assume=nowritable/mach/obj=cp$obj/extern=strict
cc_fp = cc/noprefix/pointer_size=32
ots = sys$library:starlet
assemble = macro/alpha/mac/show=binary
p_assemble = macro/alpha/mac/show=binary
hal = hal_alpha
mapcvt = mapcvt_alpha
copt = /opt=(inline=none,unroll=1)
copt_inl = /opt=(inline=none,unroll=1)
list = /lis=cp$lis
bliss = bliss/a64
libr = libr/alpha
link = link/alpha/nosyslib/nouserlib/nosysshr
macro = macro/migration/mac
macro64 = macro/alpha/mac/show=binary
sdl = sdl/alpha/symbols=(ALPHA=1,VAX=0)

.first
	show time
	@cp$src:console_setup
