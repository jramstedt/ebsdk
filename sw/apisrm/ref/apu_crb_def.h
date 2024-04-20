/********************************************************************************************************************************/
/* Created  2-FEB-1994 06:58:58 by VAX SDL V3.2-12     Source:  2-FEB-1994 06:58:57 MORGN:[PEACOCK.AVANTI.TOOLS]CRB_DEF.SDI;5 */
/********************************************************************************************************************************/
 
/*** MODULE crb_def IDENT V1.0 ***/
/*                                                                          */
/* Structure to define an I/O entry in the CRB                              */
/*                                                                          */
struct crb_io_entry {
    int64 entry$q_vir_addr;
    int64 entry$q_phy_addr;
    int64 entry$q_page_count;
    } ;
/*                                                                          */
/* CRB definition used in FLAMINGO                                          */
/*                                                                          */
/* NOTE !!! You must also add in a I/O entry to the file FLAM_CRB if you    */
/* add one here (and visa versa). Always add new entries to the end of the  */
/* table                                                                    */
/*                                                                          */
struct crb_def {
    int64 crb$q_dispatch_va;
    int64 crb$q_dispatch_pa;
    int64 crb$q_fixup_va;
    int64 crb$q_fixup_pa;
    int64 crb$q_nbr_of_entries;
    int64 crb$q_pages_to_map;
    struct crb_io_entry crb$r_callback_code;
    struct crb_io_entry crb$r_io_tc_asic;
    struct crb_io_entry crb$r_io_tc_int;
    struct crb_io_entry crb$r_io_tc_intrc;
    struct crb_io_entry crb$r_io_tc_sgmap;
    struct crb_io_entry crb$r_io_tc_reset;
    struct crb_io_entry crb$r_io_coreio;
    struct crb_io_entry crb$r_io_iorom;
    struct crb_io_entry crb$r_io_nirom;
    struct crb_io_entry crb$r_io_ni;
    struct crb_io_entry crb$r_io_scc0;
    struct crb_io_entry crb$r_io_scc1;
    struct crb_io_entry crb$r_io_nvr;
    struct crb_io_entry crb$r_io_isdn;
    struct crb_io_entry crb$r_io_scsi_ir;
    struct crb_io_entry crb$r_io_scsi_c94;
    struct crb_io_entry crb$r_io_scsi_dma;
    struct crb_io_entry crb$r_io_cxt_csr;
    struct crb_io_entry crb$r_io_cxt_gp0;
    struct crb_io_entry crb$r_io_cxt_gp1;
    struct crb_io_entry crb$r_io_cxt_ramdac;
    struct crb_io_entry crb$r_io_cxt_ram;
    struct crb_io_entry crb$r_io_cxt_rom;
    struct crb_io_entry crb$r_io_turbo_slot [6];
    struct crb_io_entry crb$r_io_cxt_ram_dense;
    struct crb_io_entry crb$r_rex_memory_regions;
    struct crb_io_entry crb$r_io_imr;
/*                                                                          */
/* Add new entries to the table here !!!!                                   */
/*                                                                          */
    } ;
