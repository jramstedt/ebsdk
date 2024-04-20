/* NAUTILUS Driver startup table */
extern int nl_init ();
extern int rd_init ();
extern int toy_init ();
extern int pci_init ();
extern int from_init ();
extern int nvram_init ();
extern int api_iic_init ();
extern int tt_init ();
extern int dv_init ();
extern int vga_bios_init ();
extern int kbd_init ();
extern int combo_init ();
extern int ev_init ();
extern int el_init ();
extern int pmem_init ();
extern int vmem_init ();
extern int examine_init ();
extern int ev6_ipr_init ();
extern int gpr_init ();
extern int ps_init ();
extern int ndbr_init ();
extern int mop_init ();
extern int fpr_init ();
extern int pi_init ();
extern int decode_init ();
extern int tee_init ();
extern int dup_init ();
extern int para_init ();
extern int inet_init ();
extern int fat_init ();
extern int m1543c_init ();
extern int dq_init ();
extern int de205_init ();
extern int n810_init ();
extern int pci_nvram_init ();
extern int pdq_init ();
extern int dac960_init ();
extern int pks_init ();
extern int isp1020_init ();
extern int ew_init ();
extern int ei_init ();
extern int mscp_init ();
extern int cipca_init ();
extern int kgpsa_init ();
extern int pue_init ();
extern int mc_init ();
extern int i2o_init ();
extern int iso9660_init ();
extern int aic78xx_init ();
extern int bios_disk_init ();
extern int lfu_nautilus_init ();
extern int lfu_kzpsa_init ();
extern int lfu_defpa_defea_init ();
extern int lfu_cipca_init ();
extern int lfu_mc2_init ();
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
  {api_iic_init,	"api_iic",	1},
  {tt_init,	"tt",	3},
  {dv_init,	"dv",	3},
  {vga_bios_init,	"vga_bios",	3},
  {kbd_init,	"kbd",	3},
  {combo_init,	"combo",	3},
  {ev_init,	"ev",	4},
  {el_init,	"el",	4},
  {pmem_init,	"pmem",	4},
  {vmem_init,	"vmem",	4},
  {examine_init,	"examine",	4},
  {ev6_ipr_init,	"ev6_ipr",	4},
  {gpr_init,	"gpr",	4},
  {ps_init,	"ps",	4},
  {ndbr_init,	"ndbr",	4},
  {mop_init,	"mop",	4},
  {fpr_init,	"fpr",	4},
  {pi_init,	"pi",	4},
  {decode_init,	"decode",	4},
  {tee_init,	"tee",	4},
  {dup_init,	"dup",	4},
  {para_init,	"para",	5},
  {inet_init,	"inet",	5},
  {fat_init,	"fat",	5},
  {m1543c_init,	"m1543c",	5},
  {dq_init,	"dq",	5},
  {de205_init,	"de205",	5},
  {n810_init,	"n810",	5},
  {pci_nvram_init,	"pci_nvram",	5},
  {pdq_init,	"pdq",	5},
  {dac960_init,	"dac960",	5},
  {pks_init,	"pks",	5},
  {isp1020_init,	"isp1020",	5},
  {ew_init,	"ew",	5},
  {ei_init,	"ei",	5},
  {mscp_init,	"mscp",	5},
  {cipca_init,	"cipca",	5},
  {kgpsa_init,	"kgpsa",	5},
  {pue_init,	"pue",	5},
  {mc_init,	"mc",	5},
  {i2o_init,	"i2o",	5},
  {iso9660_init,	"iso9660",	5},
  {aic78xx_init,	"aic78xx",	5},
  {bios_disk_init,	"bios_disk",	5},
  {lfu_nautilus_init,	"lfu_nautilus",	7},
  {lfu_kzpsa_init,	"lfu_kzpsa",	8},
  {lfu_defpa_defea_init,	"lfu_defpa_defea",	8},
  {lfu_cipca_init,	"lfu_cipca",	8},
  {lfu_mc2_init,	"lfu_mc2",	8},
};
int num_drivers = sizeof (dst) / sizeof (dst [0]);
