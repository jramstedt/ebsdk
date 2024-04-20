/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:36:55 by OpenVMS SDL EV1-33     */
/* Source:   5-NOV-1996 11:04:02 AFW_USER$:[CONSOLE.V58.COMMON.SRC]PYXIS_MAIN_CS */
/********************************************************************************************************************************/
/*** MODULE pyxis_main_csr_def IDENT x0.1 ***/
#ifndef	PYXIS_MAIN_CSR_DEF
#define PYXIS_MAIN_CSR_DEF 1
/*                                                                          */
/* The following is the base address of the system configuration registers in */
/* the PYXIS.  This value must be shifted left 28 places to yield the proper */
/* value, thus simplifying the load of the address into registers.          */
/*                                                                          */
#define pyxis_k_main_csr_base 2164      /* sysconfig base                   */
#define pyxis_v_main_csr_base 28        /* sysconfig base extent            */
/*                                                                          */
/* PCI configuration register                                               */
/*                                                                          */
#define pci_k_cfgtype0 0
#define pci_k_cfgtype1 1
#define cfg_m_cfg_type 0x3
#define cfg_m_unused_0 0xFFFFFFFC
union PYXIS_PCI_CFG {
    struct  {
        unsigned cfg_v_cfg_type : 2;
        unsigned cfg_v_unused_0 : 30;
        } cfg_r_by_field;
    int cfg_l_whole;
    char cfg_b_displacement [64];
    } ;
/*                                                                          */
/*  PYXIS flash ctrl                                                        */
/*                                                                          */
/* FLASH CONTROL REGISTER                                                   */
#define pyxis_flash_m_wp_width 0xF
#define pyxis_flash_m_disable_time 0xF0
#define pyxis_flash_m_access_time 0xF00
#define pyxis_flash_m_low_enable 0x1000
#define pyxis_flash_m_high_enable 0x2000
#define pyxis_flash_m_ctrl_reserved1 0xFFFFC000
union PYXIS_FLASH {
    char pyxis_flash_b_displacement [64]; /* space used by this register    */
    int pyxis_flash_l_whole;
    struct  {
        unsigned pyxis_flash_v_wp_width : 4; /* [3:0]                       */
        unsigned pyxis_flash_v_disable_time : 4; /* [7:4]                   */
        unsigned pyxis_flash_v_access_time : 4; /* [11:8]                   */
        unsigned pyxis_flash_v_low_enable : 1; /* [12]                      */
        unsigned pyxis_flash_v_high_enable : 1; /* [13]                     */
        unsigned pyxis_flash_v_ctrl_reserved1 : 18; /* [31:14]              */
        } pyxis_flash_r_field;
    } ;
/*                                                                          */
/* PYXIS Control Register                                                   */
/*                                                                          */
/* suggested value is ??                                                    */
/*                                                                          */
#define pyxis_ctrl_m_pci_en 0x1
#define pyxis_ctrl_m_reserved_1 0x2
#define pyxis_ctrl_m_pci_loop_en 0x4
#define pyxis_ctrl_m_fst_bb_en 0x8
#define pyxis_ctrl_m_pci_mst_en 0x10
#define pyxis_ctrl_m_pci_mem_en 0x20
#define pyxis_ctrl_m_pci_req64_en 0x40
#define pyxis_ctrl_m_pci_ack64_en 0x80
#define pyxis_ctrl_m_addr_pe_en 0x100
#define pyxis_ctrl_m_perr_en 0x200
#define pyxis_ctrl_m_fillerr_en 0x400
#define pyxis_ctrl_m_mchkerr_en 0x800
#define pyxis_ctrl_m_ecc_chk_en 0x1000
#define pyxis_ctrl_m_assert_idle_bc 0x2000
#define pyxis_ctrl_m_reserved_2 0xFC000
#define pyxis_ctrl_m_rd_type 0x300000
#define pyxis_ctrl_m_rd_use_history 0x400000
#define pyxis_ctrl_m_reserved_3 0x800000
#define pyxis_ctrl_m_rl_type 0x3000000
#define pyxis_ctrl_m_rl_use_history 0x4000000
#define pyxis_ctrl_m_reserved_4 0x8000000
#define pyxis_ctrl_m_rm_type 0x30000000
#define pyxis_ctrl_m_rm_use_history 0x40000000
#define pyxis_ctrl_m_reserved_5 0x80000000
union PYXIS_CTRL {
    char pyxis_ctrl_b_displacement [64]; /* space used by this register     */
    int pyxis_ctrl_l_whole;
    struct  {
        unsigned pyxis_ctrl_v_pci_en : 1; /* [0]                            */
        unsigned pyxis_ctrl_v_reserved_1 : 1; /* [1]                        */
        unsigned pyxis_ctrl_v_pci_loop_en : 1; /* [2]                       */
        unsigned pyxis_ctrl_v_fst_bb_en : 1; /* [3]                         */
        unsigned pyxis_ctrl_v_pci_mst_en : 1; /* [4]                        */
        unsigned pyxis_ctrl_v_pci_mem_en : 1; /* [5]                        */
        unsigned pyxis_ctrl_v_pci_req64_en : 1; /* [6]                      */
        unsigned pyxis_ctrl_v_pci_ack64_en : 1; /* [7]                      */
        unsigned pyxis_ctrl_v_addr_pe_en : 1; /* [8]                        */
        unsigned pyxis_ctrl_v_perr_en : 1; /* [9]                           */
        unsigned pyxis_ctrl_v_fillerr_en : 1; /* [10]                       */
        unsigned pyxis_ctrl_v_mchkerr_en : 1; /* [11]                       */
        unsigned pyxis_ctrl_v_ecc_chk_en : 1; /* [12]                       */
        unsigned pyxis_ctrl_v_assert_idle_bc : 1; /* [13]                   */
        unsigned pyxis_ctrl_v_reserved_2 : 6; /* [14-19]                    */
        unsigned pyxis_ctrl_v_rd_type : 2; /* [20:21]                       */
        unsigned pyxis_ctrl_v_rd_use_history : 1; /* [22]                   */
        unsigned pyxis_ctrl_v_reserved_3 : 1; /* [23]                       */
        unsigned pyxis_ctrl_v_rl_type : 2; /* [24:25]                       */
        unsigned pyxis_ctrl_v_rl_use_history : 1; /* [26]                   */
        unsigned pyxis_ctrl_v_reserved_4 : 1; /* [27]                       */
        unsigned pyxis_ctrl_v_rm_type : 2; /* [28:29]                       */
        unsigned pyxis_ctrl_v_rm_use_history : 1; /* [30]                   */
        unsigned pyxis_ctrl_v_reserved_5 : 1; /* [31]                       */
        } pyxis_ctrl_r_field;
    } ;
/* PYXIS Control Register                                                   */
/*                                                                          */
/* suggested value is ??                                                    */
/*                                                                          */
#define pyxis_ctrl1_m_ioa_ben 0x1
#define pyxis_ctrl1_m_reserved_1 0xE
#define pyxis_ctrl1_m_pci_mwin_en 0x10
#define pyxis_ctrl1_m_reserved_2 0xE0
#define pyxis_ctrl1_m_pci_link_en 0x100
#define pyxis_ctrl1_m_reserved_3 0xE00
#define pyxis_ctrl1_m_lw_par_mode 0x1000
#define pyxis_ctrl1_m_reserved_4 0xFFFFE000
union PYXIS_CTRL1 {
    char pyxis_ctrl1_b_displacement [64]; /* space used by this register    */
    int pyxis_ctrl1_l_whole;
    struct  {
        unsigned pyxis_ctrl1_v_ioa_ben : 1; /* [0]                          */
        unsigned pyxis_ctrl1_v_reserved_1 : 3; /* [3:1]                     */
        unsigned pyxis_ctrl1_v_pci_mwin_en : 1; /* [4]                      */
        unsigned pyxis_ctrl1_v_reserved_2 : 3; /* [7:5]                     */
        unsigned pyxis_ctrl1_v_pci_link_en : 1; /* [8]                      */
        unsigned pyxis_ctrl1_v_reserved_3 : 3; /* [9:11]                    */
        unsigned pyxis_ctrl1_v_lw_par_mode : 1; /* [12]                     */
        unsigned pyxis_ctrl1_v_reserved_4 : 19; /* [31]                     */
        } pyxis_ctrl1_r_field;
    } ;
/*                                                                          */
/* Hardware Address extension register for memory                           */
/*                                                                          */
#define haemem_m_unused_0 0x3
#define haemem_m_addr_ext_3 0xFC
#define haemem_m_unused_1 0x700
#define haemem_m_addr_ext_2 0xF800
#define haemem_m_unused_2 0x1FFF0000
#define haemem_m_addr_ext_1 0xE0000000
union PCI_HAE_MEM {
    struct  {
        unsigned haemem_v_unused_0 : 2; /* [1:0]                            */
        unsigned haemem_v_addr_ext_3 : 6; /* [7:2]                          */
        unsigned haemem_v_unused_1 : 3; /* [10:8]                           */
        unsigned haemem_v_addr_ext_2 : 5; /* [15:11]                        */
        unsigned haemem_v_unused_2 : 13; /* [28:16]                         */
        unsigned haemem_v_addr_ext_1 : 3; /* [31:29]                        */
        } haemem_r_by_field;
    int haemem_l_whole;
    char haemem_b_displacement [64];
    } ;
/*                                                                          */
/* Hardware Address extension register for I/O                              */
/*                                                                          */
#define haeio_m_unused_0 0x1FFFFFF
#define haeio_m_addr_ext 0xFE000000
union PCI_HAE_IO {
    struct  {
        unsigned haeio_v_unused_0 : 25; /* [24:0]                           */
        unsigned haeio_v_addr_ext : 7;  /* [31:25]                          */
        } haeio_r_by_field;
    int haeio_l_whole;
    char haeio_b_displacement [64];
    } ;
/*                                                                          */
/* PCI latency register                                                     */
/*                                                                          */
#define pci_lat_m_unused_0 0xFF
#define pci_lat_m_latency 0xFF00
#define pci_lat_m_unused_1 0xFFFF0000
union PCI_LAT {
    struct  {
        unsigned pci_lat_v_unused_0 : 8;
        unsigned pci_lat_v_latency : 8;
        unsigned pci_lat_v_unused_1 : 16;
        } pci_lat_r_by_field;
    int pci_lat_l_whole;
    char pci_lat_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS diagnostic Force Error register                                    */
/*                                                                          */
#define pci_err_k_none 0
#define pci_err_k_lo_lwrd 268435456
#define pci_err_k_hi_lwrd 536870912
#define pci_err_k_addr 805306368
#define pyxis_diag_m_unused_0 0x1
#define pyxis_diag_m_use_check 0x2
#define pyxis_diag_m_unused_1 0xFFFFFFC
#define pyxis_diag_m_fpe_pci_low 0x10000000
#define pyxis_diag_m_fpe_pci_hi 0x20000000
#define pyxis_diag_m_unused_2 0x40000000
#define pyxis_diag_m_fpe_to_ev5 0x80000000
union PYXIS_DIAG {
    struct  {
        unsigned pyxis_diag_v_unused_0 : 1; /* [0]                          */
        unsigned pyxis_diag_v_use_check : 1; /* [1]                         */
        unsigned pyxis_diag_v_unused_1 : 26; /* [27:2]                      */
        unsigned pyxis_diag_v_fpe_pci_low : 1; /* [28]                      */
        unsigned pyxis_diag_v_fpe_pci_hi : 1; /* [29]                       */
        unsigned pyxis_diag_v_unused_2 : 1; /* [30]                         */
        unsigned pyxis_diag_v_fpe_to_ev5 : 1; /* [31]                       */
        } pyxis_diag_r_by_field;
    int pyxis_diag_l_whole;
    char pyxis_diag_b_displacement [64];
    } ;
/*                                                                          */
/* Diagnostic Check bit register                                            */
/*                                                                          */
#define diag_check_m_ecc 0xFF
#define diag_check_m_unused_0 0xFFFFFF00
union DIAG_CHECK {
    struct  {
        unsigned diag_check_v_ecc : 8;
        unsigned diag_check_v_unused_0 : 24;
        } diag_check_r_by_field;
    int diag_check_l_whole;
    char diag_check_b_displacement [64];
    } ;
/*                                                                          */
/* Performance Monitor register - 0x8740004000                              */
/*                                                                          */
#define perfm_m_low_cnt 0xFFFF
#define perfm_m_high_cnt 0xFFFF0000
union PYXIS_PERFMON {
    struct  {
        unsigned perfm_v_low_cnt : 16;
        unsigned perfm_v_high_cnt : 16;
        } perfm_r_by_field;
    int perfm_l_whole;
    char perfm_b_displacement [64];
    } ;
/*                                                                          */
/* Performance Monitor Control register - 0x8740004040                      */
/*                                                                          */
#define perfc_m_low_sel 0xFF
#define perfc_m_unused_0 0xF00
#define perfc_m_low_cnt_cyc 0x1000
#define perfc_m_low_cnt_clr 0x2000
#define perfc_m_low_err_stop 0x4000
#define perfc_m_low_cnt_strt 0x8000
#define perfc_m_high_sel 0x70000
#define perfc_m_unused_1 0xFF80000
#define perfc_m_high_cnt_cyc 0x10000000
#define perfc_m_high_cnt_clr 0x20000000
#define perfc_m_high_err_stop 0x40000000
#define perfc_m_high_cnt_strt 0x80000000
union PYXIS_PERFCTL {
    struct  {
        unsigned perfc_v_low_sel : 8;
        unsigned perfc_v_unused_0 : 4;
        unsigned perfc_v_low_cnt_cyc : 1;
        unsigned perfc_v_low_cnt_clr : 1; /* write only bit                 */
        unsigned perfc_v_low_err_stop : 1;
        unsigned perfc_v_low_cnt_strt : 1;
        unsigned perfc_v_high_sel : 3;
        unsigned perfc_v_unused_1 : 9;
        unsigned perfc_v_high_cnt_cyc : 1;
        unsigned perfc_v_high_cnt_clr : 1; /* write only bit                */
        unsigned perfc_v_high_err_stop : 1;
        unsigned perfc_v_high_cnt_strt : 1;
        } perfc_r_by_field;
    int perfc_l_whole;
    char perfc_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS Revision register                                                  */
/*                                                                          */
#define pyxis_rev_m_revision 0xFF
#define pyxis_rev_m_is_pyxis 0x100
#define pyxis_rev_m_unused_1 0xFFFFFE00
union PYXIS_REV {
    struct  {
        unsigned pyxis_rev_v_revision : 8;
        unsigned pyxis_rev_v_is_pyxis : 1;
        unsigned pyxis_rev_v_unused_1 : 23;
        } pyxis_rev_r_by_field;
    int pyxis_rev_l_whole;
    char pyxis_rev_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS error register                                                     */
/*                                                                          */
#define pyxis_err_m_cor_err 0x1
#define pyxis_err_m_un_cor_err 0x2
#define pyxis_err_m_cpu_pe 0x4
#define pyxis_err_m_mem_nem 0x8
#define pyxis_err_m_pci_serr 0x10
#define pyxis_err_m_pci_perr 0x20
#define pyxis_err_m_pci_addr_pe 0x40
#define pyxis_err_m_rcvd_mas_abt 0x80
#define pyxis_err_m_rcvd_tar_abt 0x100
#define pyxis_err_m_pa_pte_inv 0x200
#define pyxis_err_m_ioa_timeout 0x800
#define pyxis_err_m_lost_cor_err 0x10000
#define pyxis_err_m_lost_un_cor_err 0x20000
#define pyxis_err_m_lost_cpu_pe 0x40000
#define pyxis_err_m_lost_mem_nem 0x80000
#define pyxis_err_m_lost_perr 0x200000
#define pyxis_err_m_lost_pci_addr_pe 0x400000
#define pyxis_err_m_lost_rcvd_mas_abt 0x800000
#define pyxis_err_m_lost_rcvd_tar_abt 0x1000000
#define pyxis_err_m_lost_pa_pte_inv 0x2000000
#define pyxis_err_m_lost_ioa_timeout 0x8000000
#define pyxis_err_m_err_valid 0x80000000
union PYXIS_ERR {
    struct  {
        unsigned pyxis_err_v_cor_err : 1;
        unsigned pyxis_err_v_un_cor_err : 1;
        unsigned pyxis_err_v_cpu_pe : 1;
        unsigned pyxis_err_v_mem_nem : 1;
        unsigned pyxis_err_v_pci_serr : 1;
        unsigned pyxis_err_v_pci_perr : 1;
        unsigned pyxis_err_v_pci_addr_pe : 1;
        unsigned pyxis_err_v_rcvd_mas_abt : 1;
        unsigned pyxis_err_v_rcvd_tar_abt : 1;
        unsigned pyxis_err_v_pa_pte_inv : 1;
        unsigned pyxis_err_v_pyxis_err_reserved1 : 1;
        unsigned pyxis_err_v_ioa_timeout : 1;
        unsigned pyxis_err_v_pyxis_err_reserved2 : 4;
        unsigned pyxis_err_v_lost_cor_err : 1;
        unsigned pyxis_err_v_lost_un_cor_err : 1;
        unsigned pyxis_err_v_lost_cpu_pe : 1;
        unsigned pyxis_err_v_lost_mem_nem : 1;
        unsigned pyxis_err_v_pyxis_err_reserved3 : 1;
        unsigned pyxis_err_v_lost_perr : 1;
        unsigned pyxis_err_v_lost_pci_addr_pe : 1;
        unsigned pyxis_err_v_lost_rcvd_mas_abt : 1;
        unsigned pyxis_err_v_lost_rcvd_tar_abt : 1;
        unsigned pyxis_err_v_lost_pa_pte_inv : 1;
        unsigned pyxis_err_v_pyxis_err_reserved4 : 1;
        unsigned pyxis_err_v_lost_ioa_timeout : 1;
        unsigned pyxis_err_v_pyxis_err_reserved5 : 3;
        unsigned pyxis_err_v_err_valid : 1;
        } pyxis_err_r_by_field;
    int pyxis_err_l_whole;
    char pyxis_err_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS stat register                                                      */
/*                                                                          */
#define pyxis_stat_m_pci_status_0 0x1
#define pyxis_stat_m_pci_status_1 0x2
#define pyxis_stat_m_ioa_valid_3_0 0xF0
#define pyxis_stat_m_tlb_miss 0x800
union PYXIS_STAT {
    struct  {
        unsigned pyxis_stat_v_pci_status_0 : 1;
        unsigned pyxis_stat_v_pci_status_1 : 1;
        unsigned pyxis_stat_v_reserved1 : 2;
        unsigned pyxis_stat_v_ioa_valid_3_0 : 4;
        unsigned pyxis_stat_v_reserved2 : 3;
        unsigned pyxis_stat_v_tlb_miss : 1;
        unsigned pyxis_stat_v_reserved3 : 20;
        } pyxis_stat_r_by_field;
    int pyxis_stat_l_whole;
    char pyxis_stat_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS err mask register                                                  */
/*                                                                          */
#define pyxis_err_mask_m_cor_err 0x1
#define pyxis_err_mask_m_un_cor_err 0x2
#define pyxis_err_mask_m_cpu_pe 0x4
#define pyxis_err_mask_m_mem_nem 0x8
#define pyxis_err_mask_m_pci_serr 0x10
#define pyxis_err_mask_m_perr 0x20
#define pyxis_err_mask_m_pci_addr_pe 0x40
#define pyxis_err_mask_m_rcvd_mas_abt 0x80
#define pyxis_err_mask_m_rcvd_tar_abt 0x100
#define pyxis_err_mask_m_pa_pte_inv 0x200
#define pyxis_err_mask_m_ioa_timeout 0x800
union PYXIS_ERR_MASK {
    struct  {
        unsigned pyxis_err_mask_v_cor_err : 1;
        unsigned pyxis_err_mask_v_un_cor_err : 1;
        unsigned pyxis_err_mask_v_cpu_pe : 1;
        unsigned pyxis_err_mask_v_mem_nem : 1;
        unsigned pyxis_err_mask_v_pci_serr : 1;
        unsigned pyxis_err_mask_v_perr : 1;
        unsigned pyxis_err_mask_v_pci_addr_pe : 1;
        unsigned pyxis_err_mask_v_rcvd_mas_abt : 1;
        unsigned pyxis_err_mask_v_rcvd_tar_abt : 1;
        unsigned pyxis_err_mask_v_pa_pte_inv : 1;
        unsigned pyxis_err_mask_v_reserved1 : 1;
        unsigned pyxis_err_mask_v_ioa_timeout : 1;
        unsigned pyxis_err_mask_v_reserved2 : 20;
        } pyxis_err_mask_r_by_field;
    int pyxis_err_mask_l_whole;
    char pyxis_err_mask_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS Syndrome register                                                  */
/*                                                                          */
#define pyxis_synd_m_err_synd0 0xFF
#define pyxis_synd_m_err_synd1 0xFF00
#define pyxis_synd_m_raw_check_bits 0xFF0000
#define pyxis_synd_m_corr_err0 0x1000000
#define pyxis_synd_m_corr_err1 0x2000000
#define pyxis_synd_m_uncorr_err0 0x4000000
#define pyxis_synd_m_uncorr_err1 0x8000000
union PYXIS_SYND {
    struct  {
        unsigned pyxis_synd_v_err_synd0 : 8;
        unsigned pyxis_synd_v_err_synd1 : 8;
        unsigned pyxis_synd_v_raw_check_bits : 8;
        unsigned pyxis_synd_v_corr_err0 : 1;
        unsigned pyxis_synd_v_corr_err1 : 1;
        unsigned pyxis_synd_v_uncorr_err0 : 1;
        unsigned pyxis_synd_v_uncorr_err1 : 1;
        unsigned pyxis_synd_v_reserved1 : 4;
        } pyxis_synd_r_by_field;
    int pyxis_synd_l_whole;
    char pyxis_synd_b_displacement [8];
    } ;
/*                                                                          */
/* PYXIS error data register                                                */
/*                                                                          */
#define pyxis_err_data_m_data 0xFFFFFFFF
union PYXIS_ERR_DATA {
    struct  {
        unsigned pyxis_err_data_v_data : 32;
        } pyxis_err_data_r_by_field;
    int pyxis_err_data_l_whole;
    char pyxis_err_data_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS mem err addr register                                              */
/*                                                                          */
#define pyxis_mear_m_mem_port_addr_31_4 0xFFFFFFF0
union PYXIS_MEAR {
    struct  {
        unsigned pyxis_mear_v_mear_reserved : 4;
        unsigned pyxis_mear_v_mem_port_addr_31_4 : 28;
        } pyxis_mear_r_by_field;
    int pyxis_mear_l_whole;
    char pyxis_mear_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS mem err stat register                                              */
/*                                                                          */
#define pyxis_mesr_m_mem_adr_39_32 0xFF
#define pyxis_mesr_m_dma_rd_nxm 0x100
#define pyxis_mesr_m_dma_wr_nxm 0x200
#define pyxis_mesr_m_cpu_rd_nxm 0x400
#define pyxis_mesr_m_cpu_wr_nxm 0x800
#define pyxis_mesr_m_io_rd_nxm 0x1000
#define pyxis_mesr_m_io_wr_nxm 0x2000
#define pyxis_mesr_m_victim_nxm 0x4000
#define pyxis_mesr_m_tlbfill_nxm 0x8000
#define pyxis_mesr_m_oword_index 0x30000
#define pyxis_mesr_m_data_cycle_type 0x1F00000
#define pyxis_mesr_m_seq_state 0xFE000000
union PYXIS_MESR {
    struct  {
        unsigned pyxis_mesr_v_mem_adr_39_32 : 8;
        unsigned pyxis_mesr_v_dma_rd_nxm : 1;
        unsigned pyxis_mesr_v_dma_wr_nxm : 1;
        unsigned pyxis_mesr_v_cpu_rd_nxm : 1;
        unsigned pyxis_mesr_v_cpu_wr_nxm : 1;
        unsigned pyxis_mesr_v_io_rd_nxm : 1;
        unsigned pyxis_mesr_v_io_wr_nxm : 1;
        unsigned pyxis_mesr_v_victim_nxm : 1;
        unsigned pyxis_mesr_v_tlbfill_nxm : 1;
        unsigned pyxis_mesr_v_oword_index : 2;
        unsigned pyxis_mesr_v_reserved1 : 2;
        unsigned pyxis_mesr_v_data_cycle_type : 5;
        unsigned pyxis_mesr_v_seq_state : 7;
        } pyxis_mesr_r_by_field;
    int pyxis_mesr_l_whole;
    char pyxis_mesr_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS PCI_ERR0 register                                                  */
/*                                                                          */
#define pyxis_pci_err0_m_dma_cmd 0xF
#define pyxis_pci_err0_m_dma_dac 0x20
#define pyxis_pci_err0_m_window 0xF00
#define pyxis_pci_err0_m_master_state 0xF0000
#define pyxis_pci_err0_m_target_state 0xF00000
#define pyxis_pci_err0_m_pci_cmd 0xE000000
#define pyxis_pci_err0_m_pci_dac 0x10000000
union PYXIS_PCI_ERR0 {
    struct  {
        unsigned pyxis_pci_err0_v_dma_cmd : 4;
        unsigned pyxis_pci_err0_v_reserved1 : 1;
        unsigned pyxis_pci_err0_v_dma_dac : 1;
        unsigned pyxis_pci_err0_v_reserved2 : 2;
        unsigned pyxis_pci_err0_v_window : 4;
        unsigned pyxis_pci_err0_v_reserved3 : 4;
        unsigned pyxis_pci_err0_v_master_state : 4;
        unsigned pyxis_pci_err0_v_target_state : 4;
        unsigned pyxis_pci_err0_v_reserved4 : 1;
        unsigned pyxis_pci_err0_v_pci_cmd : 3;
        unsigned pyxis_pci_err0_v_pci_dac : 1;
        unsigned pyxis_pci_err0_v_reserved5 : 3;
        } pyxis_pci_err0_r_by_field;
    int pyxis_pci_err0_l_whole;
    char pyxis_pci_err0_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS PCI error 1 register                                               */
/*                                                                          */
#define pyxis_pci_err1_m_data 0xFFFFFFFF
union PYXIS_PCI_ERR1 {
    struct  {
        unsigned pyxis_pci_err1_v_data : 32;
        } pyxis_pci_err1_r_by_field;
    int pyxis_pci_err1_l_whole;
    char pyxis_pci_err1_b_displacement [64];
    } ;
/*                                                                          */
/* PYXIS PCI error 2 register                                               */
/*                                                                          */
#define pyxis_pci_err2_m_data 0xFFFFFFFF
union PYXIS_PCI_ERR2 {
    struct  {
        unsigned pyxis_pci_err2_v_data : 32;
        } pyxis_pci_err2_r_by_field;
    int pyxis_pci_err2_l_whole;
    char pyxis_pci_err2_b_displacement [64];
    } ;
/*                                                                          */
/*  PYXIS Main CSR .....                                                    */
/*                                                                          */
/*  This block of registers contains at all of the registers modelled in the */
/*  structures above in the address space order in which they will appear on the */
/*  machine.                                                                */
/*                                                                          */
struct PYXIS_MAIN_CSR {
    char csr_b_offset [128];
    union PYXIS_REV csr_r_pyxis_rev;    /* PYXIS revision - 0x80            */
    union PCI_LAT csr_r_pci_lat;        /* PCI latency - 0xc0               */
    union PYXIS_CTRL csr_r_pyxis_ctrl;  /* PYXIS control - 0x100            */
    union PYXIS_CTRL1 csr_r_pyxis_ctrl1; /* PYXIS control - 0x100           */
    char csr_b_offset_1 [128];
    union PYXIS_FLASH csr_r_pyxis_flash; /* PYXIS config - 0x200            */
    char csr_b_offset_2 [448];
    union PCI_HAE_MEM csr_r_hae_mem;    /* MEM HAE - 0x400                  */
    union PCI_HAE_IO csr_r_hae_io;      /* IO HAE -  0x440                  */
    union PYXIS_PCI_CFG csr_r_cfg;      /* PCI Config - 0x480               */
    char csr_b_offset_4 [6976];
    union PYXIS_DIAG csr_r_pyxis_diag;  /* PYXIS Force error register - 0x2000 */
    char csr_b_offset_5 [4032];
    union DIAG_CHECK csr_r_diag_check;  /* Diagnostic Check bit - 0x3000    */
    char csr_b_offset_6 [4032];
    union PYXIS_PERFMON csr_r_pyxis_perf_mon; /* Performance Monitor Ctr - 0x4000 */
    union PYXIS_PERFCTL csr_r_pyxis_perf_cntl; /* Performance monitor Ctl - 0x4040 */
    char csr_b_offset_7 [16768];
    union PYXIS_ERR csr_r_pyxis_err;    /*pyxis err 0x8200                  */
    union PYXIS_STAT csr_r_pyxis_stat;  /*pyxis stat 0x8240                 */
    union PYXIS_ERR_MASK csr_r_pyxis_err_mask; /*pyxis err mask 0x8280      */
    char csr_b_offset_8 [64];
    union PYXIS_SYND csr_r_pyxis_synd;  /*pyxis synd 0x8300                 */
    union PYXIS_ERR_DATA csr_r_pyxis_err_data; /*pyxis err_data 0x8308      */
    char csr_b_offset_9 [184];
    union PYXIS_MEAR csr_r_pyxis_MEAR;  /*pyxis mear 0x8400                 */
    union PYXIS_MESR csr_r_pyxis_mesr;  /*pyxis MESR 0x8440                 */
    char csr_b_offset_B [896];
    union PYXIS_PCI_ERR0 csr_r_pyxis_pci_err0; /*pyxis_pci_err0  0x8800     */
    union PYXIS_PCI_ERR1 csr_r_pyxis_pci_err1; /*pyxis_pci_err0  0x8840     */
    union PYXIS_PCI_ERR2 csr_r_pyxis_pci_err2; /*pyxis_pci_err0  0x8880     */
    } ;
#endif
 
