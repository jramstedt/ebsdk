	.globl _DYNAMIC
	_DYNAMIC = 0

	.text
	.globl __istart
__istart:	ret
	.globl __fstart
__fstart:	ret

	.comm _fpdata 4096
