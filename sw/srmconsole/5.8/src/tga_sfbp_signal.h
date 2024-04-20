#ifndef _H_SIGNAL
#define _H_SIGNAL

typedef int sig_atomic_t;

typedef void (*__sig_handler)();

#define SIG_DFL ((__sig_handler) 0)
#define SIG_ERR ((__sig_handler) -1)
#define SIG_IGN ((__sig_handler) 1)

#define SIGABRT 1
#define SIGFPE  2
#define SIGILL  3
#define SIGINT  4
#define SIGSEGV 5
#define SIGTERM 6
#define SIGBUS  7
#define _SIG_MAX 7

__sig_handler signal( int sig, __sig_handler);
int raise(int sig);

#endif
