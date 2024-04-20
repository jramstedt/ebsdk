/* LX164 Driver startup table */
extern int nl_init ();
extern int rd_init ();
extern int toy_init ();
extern int pci_init ();
extern int from_init ();
extern int nvram_init ();
extern int tt_init ();
extern int tga_init ();
extern int vga_bios_init ();
extern int kbd_init ();
extern int combo_init ();
extern int ev_init ();
extern int el_init ();
extern int pmem_init ();
extern int vmem_init ();
extern int examine_init ();
extern int ev5_ipr_init ();
extern int gpr_init ();
extern int pt_init ();
extern int ps_init ();
extern int ndbr_init ();
extern int mop_init ();
extern int fpr_init ();
extern int pi_init ();
extern int decode_init ();
extern int tee_init ();
extern int para_init ();
extern int inet_init ();
extern int fat_init ();
extern int dq_init ();
extern int de205_init ();
extern int dv_init ();
extern int n810_init ();
extern int pci_nvram_init ();
extern int pdq_init ();
extern int dw_init ();
extern int dac960_init ();
extern int pks_init ();
extern int isp1020_init ();
extern int ew_init ();
extern int ei_init ();
extern int aic78xx_init ();
struct DST {
    int	  (*startup)();
    char  *name;
    int   phase;
} dst[] = {
  {nl_init,	"nl",	0},
  {rd_init,	"rd",	0},
  {toy_init,	"toy",	0},
  {pci_init,	"pci",	1},
  {from_init,	"from",	1},
  {nvram_init,	"nvram",	1},
  {tt_init,	"tt",	3},
  {tga_init,	"tga",	3},
  {vga_bios_init,	"vga_bios",	3},
  {kbd_init,	"kbd",	3},
  {combo_init,	"combo",	3},
  {ev_init,	"ev",	4},
  {el_init,	"el",	4},
  {pmem_init,	"pmem",	4},
  {vmem_init,	"vmem",	4},
  {examine_init,	"examine",	4},
  {ev5_ipr_init,	"ev5_ipr",	4},
  {gpr_init,	"gpr",	4},
  {pt_init,	"pt",	4},
  {ps_init,	"ps",	4},
  {ndbr_init,	"ndbr",	4},
  {mop_init,	"mop",	4},
  {fpr_init,	"fpr",	4},
  {pi_init,	"pi",	4},
  {decode_init,	"decode",	4},
  {tee_init,	"tee",	4},
  {para_init,	"para",	5},
  {inet_init,	"inet",	5},
  {fat_init,	"fat",	5},
  {dq_init,	"dq",	5},
  {de205_init,	"de205",	5},
  {dv_init,	"dv",	5},
  {n810_init,	"n810",	5},
  {pci_nvram_init,	"pci_nvram",	5},
  {pdq_init,	"pdq",	5},
  {dw_init,	"dw",	5},
  {dac960_init,	"dac960",	5},
  {pks_init,	"pks",	5},
  {isp1020_init,	"isp1020",	5},
  {ew_init,	"ew",	5},
  {ei_init,	"ei",	5},
  {aic78xx_init,	"aic78xx",	5},
};
int num_drivers = sizeof (dst) / sizeof (dst [0]);
