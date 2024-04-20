/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:37:07 by OpenVMS SDL EV1-33     */
/* Source:   5-NOV-1996 11:20:22 AFW_USER$:[CONSOLE.V58.COMMON.SRC]PYXIS_ADDR_TR */
/********************************************************************************************************************************/
/*** MODULE PYXIS_ADDR_TRANS_DEF IDENT V1.0 ***/
#ifndef	PYXIS_ADDR_TRANS_DEF
#define PYXIS_ADDR_TRANS_DEF 1
/*                                                                          */
/* Window base register 0-3                                                 */
/*                                                                          */
#define wbase_m_w_en 0x1
#define wbase_m_sg 0x2
#define wbase_m_memcs_enable 0x4
#define wbase_m_dac_enable 0x8
#define wbase_m_unused_0 0xFFFF0
#define wbase_m_w_base 0xFFF00000
union pci_win_base {
    struct  {
        unsigned wbase_v_w_en : 1;
        unsigned wbase_v_sg : 1;
        unsigned wbase_v_memcs_enable : 1;
        unsigned wbase_v_dac_enable : 1;
        unsigned wbase_v_unused_0 : 16;
        unsigned wbase_v_w_base : 12;
        } wbase_r_by_field;
    int wbase_l_whole;
    char wbase_b_displacement [64];
    } ;
/*                                                                          */
/* Window mask register 0-3                                                 */
/*                                                                          */
#define wmask_k_sz1mb 0
#define wmask_k_sz2mb 1048576
#define wmask_k_sz4mb 3145728
#define wmask_k_sz8mb 7340032
#define wmask_k_sz16mb 15728640
#define wmask_k_sz32mb 32505856
#define wmask_k_sz64mb 66060288
#define wmask_k_sz128mb 133169152
#define wmask_k_sz256mb 267386880
#define wmask_k_sz512mb 535822336
#define wmask_k_sz1gb 1072693248
#define wmask_k_sz2gb 2146435072
#define wmask_k_sz4gb -1048576
#define wmask_m_unused_0 0xFFFFF
#define wmask_m_w_mask 0xFFF00000
union pci_win_mask {
    struct  {
        unsigned wmask_v_unused_0 : 20;
        unsigned wmask_v_w_mask : 12;
        } wmask_r_by_field;
    int wmask_l_whole;
    char wmask_b_displacement [64];
    } ;
/*                                                                          */
/* Translation Base register 0-3                                            */
/*                                                                          */
#define tbase_m_unused_0 0xFF
#define tbase_m_t_base 0xFFFFFF00
union pci_trn_base {
    struct  {
        unsigned tbase_v_unused_0 : 8;
        unsigned tbase_v_t_base : 24;
        } tbase_r_by_field;
    int tbase_l_whole;
    char tbase_b_displacement [64];
    } ;
/*                                                                          */
/* Window DAC base register                                                 */
/*                                                                          */
#define wdac_m_base 0xFF
#define wdac_m_unused 0xFFFFFF00
union pci_wdac_base {
    struct  {
        unsigned wdac_v_base : 8;
        unsigned wdac_v_unused : 24;
        } wdac_r_by_field;
    int wdac_l_whole;
    char wdac_b_displacement [64];
    } ;
/*                                                                          */
/* Lockable Translation Buffer Tag 0-3                                      */
/*                                                                          */
#define locktb_tag_count 4
#define locktbtag_m_valid 0x1
#define locktbtag_m_locked 0x2
#define locktbtag_m_dac 0x4
#define locktbtag_m_unused_0 0x7FF8
#define locktbtag_m_tb_tag 0xFFFF8000
union pci_lock_tbtag {
    struct  {
        unsigned locktbtag_v_valid : 1;
        unsigned locktbtag_v_locked : 1;
        unsigned locktbtag_v_dac : 1;
        unsigned locktbtag_v_unused_0 : 12;
        unsigned locktbtag_v_tb_tag : 17;
        } locktbtag_r_by_field;
    int locktbtag_l_whole;
    char locktbtag_b_displacement [64];
    } ;
/*                                                                          */
/* Translation Buffer Tag 4-7                                               */
/*                                                                          */
#define tb_tag_count 4
#define tbtag_m_valid 0x1
#define tbtag_m_unused_0 0x2
#define tbtag_m_dac 0x4
#define tbtag_m_unused_1 0x7FF8
#define tbtag_m_tb_tag 0xFFFF8000
union pci_tbtag {
    struct  {
        unsigned tbtag_v_valid : 1;
        unsigned tbtag_v_unused_0 : 1;
        unsigned tbtag_v_dac : 1;
        unsigned tbtag_v_unused_1 : 12;
        unsigned tbtag_v_tb_tag : 17;
        } tbtag_r_by_field;
    int tbtag_l_whole;
    char tbtag_b_displacement [64];
    } ;
/*                                                                          */
/* Translation Buffer Page Register [0-7] [0-3]                             */
/*                                                                          */
#define tb_page_count 4
#define tbpage_m_valid 0x1
#define tbpage_m_page_address 0x3FFFFE
#define tbpage_m_unused 0xFFC00000
union pci_tbpage {
    struct  {
        unsigned tbpage_v_valid : 1;
        unsigned tbpage_v_page_address : 21;
        unsigned tbpage_v_unused : 10;
        } tbpage_r_by_field;
    int tbpage_l_whole;
    char tbpage_b_displacement [64];
    } ;
/* Define constants to build base address of 0x8760000000                   */
#define pyxis_a_addr_trans 2166
#define pyxis_v_addr_trans 28
/*                                                                          */
/* PCI Window management and Scatter/gather registers - 0x8760000000        */
/*                                                                          */
struct pyxis_addr_trans {
    char fill_0 [256];
    int tbia;                           /* 0x100 from base                  */
    char fill_1 [764];
    union pci_win_base w_base0;         /* 0x400 from base                  */
    union pci_win_mask w_mask0;         /* 0x440 from base                  */
    union pci_trn_base t_base0;         /* 0x480 from base                  */
    char fill_2 [64];
    union pci_win_base w_base1;         /* 0x500 from base                  */
    union pci_win_mask w_mask1;         /* 0x540 from base                  */
    union pci_trn_base t_base1;         /* 0x580 from base                  */
    char fill_3 [64];
    union pci_win_base w_base2;         /* 0x600 from base                  */
    union pci_win_mask w_mask2;         /* 0x640 from base                  */
    union pci_trn_base t_base2;         /* 0x680 from base                  */
    char fill_4 [64];
    union pci_win_base w_base3;         /* 0x700 from base                  */
    union pci_win_mask w_mask3;         /* 0x740 from base                  */
    union pci_trn_base t_base3;         /* 0x780 from base                  */
    union pci_wdac_base w_dac3;         /* 0x7c0 from base                  */
    union pci_lock_tbtag ltb_tag [4];   /* 0x800 from base                  */
    union pci_tbtag tb_tag [4];         /* 0x900 from base                  */
    char fill_6 [1536];
    union pci_tbpage tb0_page [4];      /* 0x1000 from base                 */
    union pci_tbpage tb1_page [4];      /* 0x1100 from base                 */
    union pci_tbpage tb2_page [4];      /* 0x1200 from base                 */
    union pci_tbpage tb3_page [4];      /* 0x1300 from base                 */
    union pci_tbpage tb4_page [4];      /* 0x1400 from base                 */
    union pci_tbpage tb5_page [4];      /* 0x1500 from base                 */
    union pci_tbpage tb6_page [4];      /* 0x1600 from base                 */
    union pci_tbpage tb7_page [4];      /* 0x1700 from base                 */
    } ;
#endif
 
