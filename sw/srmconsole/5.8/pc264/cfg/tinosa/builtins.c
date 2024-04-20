/* file: builtins.c - firmware built in commands */
#include	"cp$src:platform.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:common.h"
#include	"cp$src:msg_def.h"

/*
 * Define a data structure that is used to initialize the built in files on the
 * ram disk.
 */     
#define R	ATTR$M_READ
#define W	ATTR$M_WRITE
#define X	ATTR$M_EXECUTE
#define B	ATTR$M_BINARY
#define Z	ATTR$M_EXPAND
#define P	ATTR$M_SECURE

extern sh_cmd ();
extern kill ();
extern exit_cmd ();
extern break_cmd ();
extern boot ();
extern buildfru ();
extern clear_error ();
extern date ();
extern exdep_cmd ();
extern exdep_cmd ();
extern isacfg ();
extern isp1020_edit ();
extern run ();
extern set ();
extern show ();
extern clear ();
extern stop ();
extern stop ();
extern stop ();
extern start_cmd ();
extern continue_cmd ();
extern init ();
extern tr_cmd ();
extern uptime ();
extern arc ();
extern rcu ();
extern jtopal ();
