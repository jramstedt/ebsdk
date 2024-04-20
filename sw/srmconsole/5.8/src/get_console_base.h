#if NONZERO_CONSOLE_BASE
unsigned __int64 get_console_base_pa();
unsigned __int64 get_console_base_pfn();
unsigned __int64 virt_to_phys(unsigned __int64 va);
unsigned __int64 phys_to_virt(unsigned __int64 pa);
unsigned __int64 pal_virt_to_phys(unsigned __int64 cpu, unsigned __int64 va);
#else
#define get_console_base_pa() (0)
#define get_console_base_pfn() (0)
#define virt_to_phys(va) (va)
#define phys_to_virt(pa) (pa)
#define pal_virt_to_phys(cpu,va) (va)
#endif
