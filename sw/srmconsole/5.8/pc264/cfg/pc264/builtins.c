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
extern exdep_cmd ();
extern exdep_cmd ();
extern isacfg ();
extern isp1020_edit ();
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
extern info ();
extern dynamic ();
extern alloc_cmd ();
extern free_cmd ();
extern bpt ();
extern cat ();
extern check ();
extern chmod ();
extern cmp ();
extern element ();
extern echo ();
extern edit ();
extern eval_cmd ();
extern exer ();
extern find_field ();
extern grep ();
extern galaxy ();
extern galaxy ();
extern migrate ();
extern gct ();
extern gctverify ();
extern hd ();
extern help_cmd ();
extern help_cmd ();
extern line ();
extern login ();
extern ls ();
extern more ();
extern net ();
extern prcache ();
extern ps_command ();
extern set_affinity ();
extern set_priority ();
extern rm ();
extern semaphore ();
extern sleep ();
extern sort ();
extern sound ();
extern true_cmd ();
extern false_cmd ();
extern wc ();
extern memtest ();
extern nettest ();
extern lfu ();
extern ewrk3_config ();
extern wwidmgr ();
extern mc_diag ();
extern pcimc_cable ();
struct {
	char	*name;
	void	*addr;
	int	attributes;
	int	stacksize;
} builtin_files [] = {
	{"shell",           sh_cmd,          R|X|B|Z, 12*1024},
	{"kill",            kill,            R|X|B|Z, 0},
	{"exit",            exit_cmd,        R|X|B|Z, 0},
	{"break",           break_cmd,       R|X|B|Z, 0},
	{"boot",            boot,            R|X|B|Z|P, 6*1024},
	{"b",               boot,            R|X|B|Z|P, 6*1024},
	{"buildfru",        buildfru,        R|X|B|Z, 0},
	{"clear_error",     clear_error,     R|X|B|Z, 6*1024},
	{"d",               exdep_cmd,       R|X|B, 0},
	{"deposit",         exdep_cmd,       R|X|B, 0},
	{"e",               exdep_cmd,       R|X|B, 0},
	{"examine",         exdep_cmd,       R|X|B, 0},
	{"isacfg",          isacfg,          R|X|B, 0},
	{"isp1020_edit",    isp1020_edit,    R|X|B|Z, 0},
	{"set",             set,             R|X|B|Z, 0},
	{"se",              set,             R|X|B|Z, 0},
	{"show",            show,            R|X|B, 8*1024},
	{"sho",             show,            R|X|B, 8*1024},
	{"sh",              show,            R|X|B, 8*1024},
	{"clear",           clear,           R|X|B|Z, 0},
	{"crash",           stop,            R|X|B|Z, 0},
	{"halt",            stop,            R|X|B|Z, 0},
	{"h",               stop,            R|X|B|Z, 0},
	{"stop",            stop,            R|X|B|Z, 0},
	{"start",           start_cmd,       R|X|B|Z|P, 0},
	{"continue",        continue_cmd,    R|X|B|Z|P, 0},
	{"c",               continue_cmd,    R|X|B|Z|P, 0},
	{"init",            init,            R|X|B|Z, 0},
	{"tr",              tr_cmd,          R|X|B|Z, 0},
	{"uptime",          uptime,          R|X|B|Z, 0},
	{"arc",             arc,             R|X|B|Z, 0},
	{"nt",              arc,             R|X|B|Z, 0},
	{"alphabios",       arc,             R|X|B|Z, 0},
	{"rcu",             rcu,             R|X|B|Z, 0},
	{"jtopal",          jtopal,          R|X|B|Z, 0},
	{"info",            info,            R|X|B|Z, 0},
	{"dynamic",         dynamic,         R|X|B|Z, 0},
	{"alloc",           alloc_cmd,       R|X|B|Z, 0},
	{"free",            free_cmd,        R|X|B|Z, 0},
	{"bpt",             bpt,             R|X|B|Z, 0},
	{"cat",             cat,             R|X|B|Z, 0},
	{"check",           check,           R|X|B|Z, 0},
	{"chmod",           chmod,           R|X|B|Z, 0},
	{"cmp",             cmp,             R|X|B|Z, 0},
	{"element",         element,         R|X|B|Z, 0},
	{"echo",            echo,            R|X|B|Z, 0},
	{"edit",            edit,            R|X|B|Z, 0},
	{"eval",            eval_cmd,        R|X|B, 0},
	{"exer",            exer,            R|X|B|Z, 0},
	{"find_field",      find_field,      R|X|B|Z, 0},
	{"grep",            grep,            R|X|B|Z, 0},
	{"lpinit",          galaxy,          R|X|B|Z, 0},
	{"galaxy",          galaxy,          R|X|B|Z, 0},
	{"migrate",         migrate,         R|X|B|Z, 0},
	{"gct",             gct,             R|X|B|Z, 0},
	{"gctverify",       gctverify,       R|X|B|Z, 0},
	{"hd",              hd,              R|X|B|Z, 0},
	{"man",             help_cmd,        R|X|B, 0},
	{"help",            help_cmd,        R|X|B, 0},
	{"line",            line,            R|X|B|Z, 0},
	{"login",           login,           R|X|B|P, 0},
	{"ls",              ls,              R|X|B, 0},
	{"more",            more,            R|X|B|Z, 0},
	{"net",             net,             R|X|B|Z, 0},
	{"prcache",         prcache,         R|X|B|Z, 0},
	{"ps",              ps_command,      R|X|B|Z, 0},
	{"sa",              set_affinity,    R|X|B|Z, 0},
	{"sp",              set_priority,    R|X|B|Z, 0},
	{"rm",              rm,              R|X|B|Z, 0},
	{"semaphore",       semaphore,       R|X|B|Z, 0},
	{"sleep",           sleep,           R|X|B|Z, 0},
	{"sort",            sort,            R|X|B|Z, 0},
	{"sound",           sound,           R|X|B|Z, 0},
	{"true",            true_cmd,        R|X|B|Z, 0},
	{"false",           false_cmd,       R|X|B|Z, 0},
	{"wc",              wc,              R|X|B|Z, 0},
	{"memtest",         memtest,         R|X|B|Z, 0},
	{"nettest",         nettest,         R|X|B|Z, 0},
	{"lfu",             lfu,             R|X|B, 0},
	{"ewrk3_config",    ewrk3_config,    R|X|B, 0},
	{"wwidmgr",         wwidmgr,         R|X|B|Z|P, 8*1024},
	{"wwidmgr",         wwidmgr,         R|X|B|Z|P, 8*1024},
	{"mc_diag",         mc_diag,         R|X|B, 0},
	{"mc_cable",        pcimc_cable,     R|X|B, 0},
};
int num_builtins = sizeof (builtin_files) / sizeof (builtin_files [0]);
int install_builtin_commands () {
	int		i;
	struct INODE	*ip;
	extern struct DDB rd_ddb;

	for (i=0; i<num_builtins; i++) {
	    allocinode (builtin_files [i].name, 1, &ip);
	    ip->dva = & rd_ddb;
	    strcpy (ip->name, builtin_files [i].name);
	    ip->attr = builtin_files [i].attributes;
	    ip->loc = builtin_files [i].addr;
	    ip->misc = builtin_files [i].stacksize;
	    INODE_UNLOCK (ip);
	}

	return (int) msg_success;
}
