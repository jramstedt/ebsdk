/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 *  9-Apr-92  Mary Thompson (mrt) at Carnegie-Mellon University
 *	Picked up the following changes from STUMP
 *	
 *	      brk() and _brk() now set errno too.
 *	      [91/01/02  18:16:14  bww]
 *	
 *	      Removed bogus printf(), and added _brk() interface.
 *	      [90/12/21  22:38:51  bww]
 *	
 *	      Switched ordering so that new platforms, like the MAC2,
 *	      get the default treatment.  brk() now returns an error.
 *	      [90/12/21  14:23:14  bww]
 *	
 *	      SUN3 loader does like ".text ... static int".
 *	      [90/08/02  20:33:39  bww]
 *
 * $Log: sbrk.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:28  paradis
 * Initial CVS checkin
 *
 * Revision 1.6  92/04/08  21:14:41  mrt
 * 	Picked up  support for Luna88k from Omron.
 * 	[92/04/02            mrt]
 * 
 * Revision 1.5  90/04/10  17:41:54  mrt
 * 	Added support for i386.
 * 	Added asm(".data") for all assembly cases
 * 	[90/04/10            mrt]
 * 
 * Revision 1.4  90/01/25  15:34:43  mrt
 * 	Delinted call to vm_allocate
 * 	[90/01/25            mrt]
 * 
 * Revision 1.3  89/08/01  18:08:41  mrt
 * 	Mips support.
 * 	[89/07/12            af]
 * 
 * Revision 1.2  89/05/05  18:46:50  mrt
 * 	Changes to use new include files and general cleanup.
 * 	[89/04/25            mrt]
 * 
 * 20-Oct-87  Mary Thompson (mrt) at Carnegie Mellon
 *	Changed sbrk to vm_allocate at curbrk if possible
 *	to be compatible with the Unix sbrk on rts.
 *
 *  6-Apr-87  Mary Thompson (mrt) at Carnegie Mellon
 *	Added conditional defintions of curbrk and minbrk for
 *	lint.
 *
 *
 *  1-Dec-86  David L. Black (dlb) at Carnegie-Mellon University
 *	Multimax bug fix; put minbrk and curbrk in .data section.
 *
 *
 * 16-Oct-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Fixed case where sbrk(0) was being mishandled; put in
 *	initialization to get break value right before first call.
 *
 *  2-Jun-86  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	File:	sbrk.c
 *
 *	Unix compatibility for sbrk system call.
 */

#define  EXPORT_BOOLEAN
#include <mach.h>		/* for vm_allocate, vm_offset_t */
#include <sys/errno.h>		/* for ENOMEM */
#include <mach_init.h>		/* for vm_page_size */

extern int errno;

#if defined(mips) || defined(__alpha)
/*
 * No need for  "assembler" forms of cubrk, minbrk.
 */
#if __osf__
#define end _end
#endif
extern char end;
#define curbrk _curbrk
#define minbrk _minbrk
vm_offset_t curbrk = (vm_offset_t)&end;
vm_offset_t minbrk = (vm_offset_t)&end;
#else /* mips || alpha */
#if	defined(luna88k)
#if     !defined(__GNUC__)
/* GHC isn't happy if asm statements are outside of function */
DEF_FUNC()
{
#endif  /* not defined(__GNUC__) */

asm("data                               "); /* make sure we have data segment */
asm("align      4                       "); /* guarantee word alignment */
asm("global     curbrk                  "); /* export curbrk */
asm("global     minbrk                  "); /* export minbrk */
asm("global     _curbrk                 "); /* export _curbrk */
asm("global     _minbrk                 "); /* export _minbrk */
asm("global     _end                    "); /* export _end */
asm("_minbrk:                           "); /* define _minbrk */
asm("minbrk:    word    _end            "); /* define minbrk = _minbrk */
asm("_curbrk:                           "); /* define _curbrk */
asm("curbrk:    word    _end            "); /* define curbrk = _curbrk */
asm("text                               "); /* compiler expects text segment */

#if     !defined(__GNUC__)
}
#endif  /* not defined(__GNUC__) */

#else /* luna88k */
 /* default - currently works for vax,ibmrt,ns32000,sun3,sun4. i386 and mac2 */
#if	multimax
DEF_FUNC()
{
#endif	/* multimax */
asm(".data");
asm(".globl	curbrk");
asm(".globl	minbrk");
asm(".globl	_curbrk");
asm(".globl	_minbrk");
asm(".globl	_end");
#if	multimax
asm("_minbrk:");
asm("minbrk:	.double	_end");
asm("_curbrk:");
asm("curbrk:	.double	_end");
asm(".text");
}
#else	/* multimax */
asm("_minbrk:");
asm("minbrk:	.long	_end");
asm("_curbrk:");
asm("curbrk:	.long	_end");
asm(".text");
#endif	/* multimax */
#endif /* luna88k */
#endif	/* mips */


#ifdef lint
/*
 * lint doesn't see asm stuff
 */
vm_offset_t	curbrk;
vm_offset_t	minbrk;
#else /* lint */
extern vm_offset_t curbrk;
extern vm_offset_t minbrk;
#endif /* lint */

#define	roundup(a,b)	((((a) + (b) - 1) / (b)) * (b))


vm_offset_t sbrk(size)
	int	size;
{
	vm_offset_t	addr;
	kern_return_t	ret;
	vm_offset_t		ocurbrk;
	
	if (size <= 0)
		return(curbrk);
	addr = (vm_offset_t) roundup((vm_offset_t)curbrk,vm_page_size);
	ocurbrk = curbrk;
	if ((curbrk+size) > addr)
	{	ret = vm_allocate(task_self(), &addr, 
			    (vm_size_t)size-((int)addr-(int)curbrk), FALSE);
		if (ret == KERN_NO_SPACE) {
			ret = vm_allocate(task_self(), &addr,
				    (vm_size_t) size, TRUE);
			ocurbrk = (vm_offset_t)addr;
		}
		if (ret != KERN_SUCCESS) 
			return((vm_offset_t) -1);
	}

	curbrk = (vm_offset_t)ocurbrk + size;
	return(ocurbrk);

}

vm_offset_t _brk(addr)
	vm_offset_t addr;
{
	vm_offset_t rnd_addr, rnd_brk;
	kern_return_t	ret;

	rnd_addr = roundup(addr, vm_page_size);
	rnd_brk = roundup(curbrk, vm_page_size);

	if (rnd_addr < rnd_brk)
		ret = vm_deallocate(task_self(), rnd_addr, rnd_brk - rnd_addr);
	else if (rnd_addr > rnd_brk)
		ret = vm_allocate(task_self(), &rnd_brk, rnd_addr - rnd_brk, FALSE);
	if (ret != KERN_SUCCESS) {
		errno = ENOMEM;
		return (vm_offset_t) -1L;
	}
	curbrk = addr;
	return (vm_offset_t) 0L;
}


vm_offset_t brk(addr)
	vm_offset_t addr;
{
	if (addr < minbrk)
		addr = minbrk;

	return _brk(addr);
}

