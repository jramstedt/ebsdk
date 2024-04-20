#ifndef	_exc
#define	_exc

/* Module exc */

#include <mach/kern_return.h>
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(LINTLIBRARY)
#include <mach/port.h>
#include <mach/message.h>
#endif

#ifndef	mig_external
#define mig_external extern
#endif

mig_external void init_exc
#if	(defined(__STDC__) || defined(c_plusplus))
    (port_t rep_port);
#else
    ();
#endif
#include <mach/std_types.h>

/* Routine exception_raise */
mig_external kern_return_t exception_raise
#if	defined(LINTLIBRARY)
    (exception_port, thread, task, exception, code, subcode)
	port_t exception_port;
	port_t thread;
	port_t task;
	long exception;
	long code;
	long subcode;
{ return exception_raise(exception_port, thread, task, exception, code, subcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	port_t exception_port,
	port_t thread,
	port_t task,
	long exception,
	long code,
	long subcode
);
#else
    ();
#endif
#endif

#endif	/* _exc */
