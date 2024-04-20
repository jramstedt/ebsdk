/* LXUPDATE Encapsulated file table */
extern int encap_fwupdate;
extern int encap_add_de205;
extern int encap_test;
extern int encap_memexer;
extern int encap_sys_exer;
extern int encap_floppy;
extern int encap_floppy_write;
extern int encap_network;
extern int encap_exer_read;
extern int encap_exer_write;
extern int encap_tgax;
extern int encap_show_status;
extern int encap_kill_diags;
extern int encap_showit;
extern int encap_dqtest;
extern int encap_readme;
extern int encap_save_nvram;
extern int encap_restore_nvram;
struct ENCAP {
	int	*address;
	int	attributes;
	char	*name;
} encap_files [] = {
	{&encap_fwupdate,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"fwupdate"},
	{&encap_add_de205,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"add_de205"},
	{&encap_test,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"test"},
	{&encap_memexer,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"memexer"},
	{&encap_sys_exer,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"sys_exer"},
	{&encap_floppy,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"floppy"},
	{&encap_floppy_write,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"floppy_write"},
	{&encap_network,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"network"},
	{&encap_exer_read,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"exer_read"},
	{&encap_exer_write,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"exer_write"},
	{&encap_tgax,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"tgax"},
	{&encap_show_status,	ATTR$M_READ|ATTR$M_EXECUTE,	"show_status"},
	{&encap_kill_diags,	ATTR$M_READ|ATTR$M_EXECUTE,	"kill_diags"},
	{&encap_showit,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"showit"},
	{&encap_dqtest,	ATTR$M_READ|ATTR$M_WRITE|ATTR$M_EXECUTE,	"dqtest"},
	{&encap_readme,	ATTR$M_READ|ATTR$M_EXECUTE,	"readme"},
	{&encap_save_nvram,	ATTR$M_READ|ATTR$M_EXECUTE,	"save_nvram"},
	{&encap_restore_nvram,	ATTR$M_READ|ATTR$M_EXECUTE,	"restore_nvram"},
};
int num_encap = sizeof (encap_files)  / sizeof (encap_files[0]);
