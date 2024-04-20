struct crb_io_entry {
    long entry$q_vir_addr;
    long entry$q_phy_addr;
    long entry$q_page_count;
    } ;

typedef struct crb_io_entry CRB_ENTRY;

struct crb_def {
    long      crb$q_dispatch_va;
    long      crb$q_dispatch_pa;
    long      crb$q_fixup_va;
    long      crb$q_fixup_pa;
    long      crb$q_nbr_of_entries;
    long      crb$q_pages_to_map;
    CRB_ENTRY crb$r_callback_code;
    CRB_ENTRY crb$r_io_tc_asic;
    CRB_ENTRY crb$r_io_tc_int;
    CRB_ENTRY crb$r_io_tc_intrc;
    CRB_ENTRY crb$r_io_tc_sgmap;
    CRB_ENTRY crb$r_io_tc_reset;
    CRB_ENTRY crb$r_io_coreio;
    CRB_ENTRY crb$r_io_iorom;
    CRB_ENTRY crb$r_io_nirom;
    CRB_ENTRY crb$r_io_ni;
    CRB_ENTRY crb$r_io_scc0;
    CRB_ENTRY crb$r_io_scc1;
    CRB_ENTRY crb$r_io_nvr;
    CRB_ENTRY crb$r_io_isdn;
    CRB_ENTRY crb$r_io_scsi_ir;
    CRB_ENTRY crb$r_io_scsi_c94;
    CRB_ENTRY crb$r_io_scsi_dma;
    CRB_ENTRY crb$r_io_cxt_csr;
    CRB_ENTRY crb$r_io_cxt_gp0;
    CRB_ENTRY crb$r_io_cxt_gp1;
    CRB_ENTRY crb$r_io_cxt_ramdac;
    CRB_ENTRY crb$r_io_cxt_ram;
    CRB_ENTRY crb$r_io_cxt_rom;
    CRB_ENTRY crb$r_io_turbo_slot [6];
    CRB_ENTRY crb$r_io_cxt_ram_dense;
    CRB_ENTRY crb$r_rex_memory_regions;
    CRB_ENTRY crb$r_io_imr;
    } ;

typedef struct crb_def CRB;
