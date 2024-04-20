/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:37:18 by OpenVMS SDL EV1-33     */
/* Source:   5-NOV-1996 11:20:54 AFW_USER$:[CONSOLE.V58.COMMON.SRC]PYXIS_MEM_CSR */
/********************************************************************************************************************************/
/*** MODULE pyxis_mem_csr_def IDENT x0.1 ***/
#ifndef	PYXIS_MEM_CSR_DEF
#define PYXIS_MEM_CSR_DEF 1
/*                                                                          */
/*                                                                          */
/* The following is the base address of the system configuration registers in */
/* the PYXIS.  This value must be shifted left 28 places to yield the proper */
/* value, thus simplifying the load of the address into registers.          */
/*                                                                          */
#define pyxis_k_mem_csr_base 2165       /* sysconfig base                   */
#define pyxis_v_mem_csr_base 28         /* sysconfig base extent            */
/*                                                                          */
/*                                                                          */
/*  Memory Configuration Register bitfields                                 */
/*                                                                          */
#define mcr_m_mode_req 0x1
#define mcr_m_server_mode 0x100
#define mcr_m_bcache_stat 0x200
#define mcr_m_bcache_enable 0x400
#define mcr_m_pipelined_cache 0x800
#define mcr_m_overlap_disable 0x1000
#define mcr_m_seq_trace 0x2000
#define mcr_m_cke_auto 0x4000
#define mcr_m_dram_clk_auto 0x8000
#define mcr_m_dram_burst_len 0x70000
#define mcr_m_dram_wrap_type 0x80000
#define mcr_m_dram_lat_mode 0x700000
#define mcr_m_dram_mode_unused 0x3F800000
union MCR {
    char mcr_b_displacement [64];       /* space used by this register      */
    int mcr_l_whole;
    struct  {
        unsigned mcr_v_mode_req : 1;    /* [0]                              */
        unsigned mcr_v_mcr_reserved1 : 7; /* [7:1]                          */
        unsigned mcr_v_server_mode : 1; /* [8]                              */
        unsigned mcr_v_bcache_stat : 1; /* [9]                              */
        unsigned mcr_v_bcache_enable : 1; /* [10]                           */
        unsigned mcr_v_pipelined_cache : 1; /* [11]                         */
        unsigned mcr_v_overlap_disable : 1; /* [12]                         */
        unsigned mcr_v_seq_trace : 1;   /* [13]                             */
        unsigned mcr_v_cke_auto : 1;    /* [14]                             */
        unsigned mcr_v_dram_clk_auto : 1; /* [15]                           */
        unsigned mcr_v_dram_burst_len : 3; /* [18:16]                       */
        unsigned mcr_v_dram_wrap_type : 1; /* [19]                          */
        unsigned mcr_v_dram_lat_mode : 3; /* [22:20]                        */
        unsigned mcr_v_dram_mode_unused : 7; /*[29:23]                      */
        unsigned mcr_v_mcr_reserved3 : 2; /* [31:30]                        */
        } mcr_r_field;
    } ;
/*                                                                          */
/*  Memory Base Address Register                                            */
/*                                                                          */
#define mcmr_m_dram_clk0 0x1
#define mcmr_m_dram_clk1 0x2
#define mcmr_m_dram_clk2 0x4
#define mcmr_m_dram_clk3 0x8
#define mcmr_m_dram_clk4 0x10
#define mcmr_m_dram_clk5 0x20
#define mcmr_m_dram_clk6 0x40
#define mcmr_m_dram_clk7 0x80
#define mcmr_m_dram_clk8 0x100
#define mcmr_m_dram_clk9 0x200
#define mcmr_m_dram_clk10 0x400
#define mcmr_m_dram_clk11 0x800
#define mcmr_m_dram_clk12 0x1000
#define mcmr_m_sram_clk1 0x2000
#define mcmr_m_sram_clk2 0x4000
#define mcmr_m_sram_clk3 0x8000
union MCMR {
    char mcmr_b_displacement [64];      /* space used by this register      */
    int mcmr_l_whole;
    struct  {
        unsigned mcmr_v_dram_clk0 : 1;
        unsigned mcmr_v_dram_clk1 : 1;
        unsigned mcmr_v_dram_clk2 : 1;
        unsigned mcmr_v_dram_clk3 : 1;
        unsigned mcmr_v_dram_clk4 : 1;
        unsigned mcmr_v_dram_clk5 : 1;
        unsigned mcmr_v_dram_clk6 : 1;
        unsigned mcmr_v_dram_clk7 : 1;
        unsigned mcmr_v_dram_clk8 : 1;
        unsigned mcmr_v_dram_clk9 : 1;
        unsigned mcmr_v_dram_clk10 : 1;
        unsigned mcmr_v_dram_clk11 : 1;
        unsigned mcmr_v_dram_clk12 : 1;
        unsigned mcmr_v_sram_clk1 : 1;
        unsigned mcmr_v_sram_clk2 : 1;
        unsigned mcmr_v_sram_clk3 : 1;
        } mcmr_r_field;
    } ;
/*                                                                          */
/*  Global Timing Register                                                  */
/*                                                                          */
#define gtr_m_min_ras_precharge 0x7
#define gtr_m_idle_bc_width 0x700
union GTR {
    char gtr_b_displacement [64];       /* space used by this register      */
    int gtr_l_whole;
    struct  {
        unsigned gtr_v_min_ras_precharge : 3; /* [2:0]                      */
        unsigned gtr_v_gtr_reserved1 : 1; /* [3]                            */
        unsigned gtr_v_cas_latency : 2; /* [5:4]                            */
        unsigned gtr_v_gtr_reserved2 : 2; /* [7:6]                          */
        unsigned gtr_v_idle_bc_width : 3; /* [10:8]                         */
        unsigned gtr_v_gtr_reserved3 : 21; /* [31:11]                       */
        } gtr_r_field;
    } ;
/*                                                                          */
/*  Refresh Timing Register                                                 */
/*                                                                          */
#define rtr_m_refresh_width 0x70
#define rtr_m_ref_interval 0x1F80
#define rtr_m_force_ref 0x8000
union RTR {
    char rtr_b_displacement [64];       /* space used by this register      */
    int rtr_l_whole;
    struct  {
        unsigned rtr_v_rtr_reserved1 : 4; /* [3:0]                          */
        unsigned rtr_v_refresh_width : 3; /* [6:4]                          */
        unsigned rtr_v_ref_interval : 6; /* [12:7]                          */
        unsigned rtr_v_rtr_reserved2 : 2; /* [14:13]                        */
        unsigned rtr_v_force_ref : 1;   /* [15]                             */
        unsigned rtr_v_rtr_reserved3 : 16; /* [31:16]                       */
        } rtr_r_field;
    } ;
/*                                                                          */
/*  Row History Policy Register                                             */
/*                                                                          */
#define rhpr_m_policy_mask 0xFFFF
union RHPR {
    char rhpr_b_displacement [64];      /* space used by this register      */
    int rhpr_l_whole;
    struct  {
        unsigned rhpr_v_policy_mask : 16; /* [15:0]                         */
        unsigned rhpr_v_rphr_reserved1 : 16; /* [31:16]                     */
        } rhpr_r_field;
    } ;
/*                                                                          */
/*  Memory debug 1 Register                                                 */
/*                                                                          */
#define mdr1_m_sel0 0x3F
#define mdr1_m_sel1 0x3F00
#define mdr1_m_sel2 0x3F0000
#define mdr1_m_sel3 0x3F000000
#define mdr1_m_enable 0x80000000
union MDR1 {
    char mdr1_b_displacement [64];      /* space used by this register      */
    int mdr1_l_whole;
    struct  {
        unsigned mdr1_v_sel0 : 6;       /* [5:0]                            */
        unsigned mdr1_v_mdr1_reserve1 : 2; /* [7:6]                         */
        unsigned mdr1_v_sel1 : 6;       /* [13:8]                           */
        unsigned mdr1_v_mdr1_reserve2 : 2; /* [15:14]                       */
        unsigned mdr1_v_sel2 : 6;       /* [21:16]                          */
        unsigned mdr1_v_mdr1_reserve3 : 2; /* [23:22]                       */
        unsigned mdr1_v_sel3 : 6;       /* [29:24]                          */
        unsigned mdr1_v_mdr1_reserve4 : 1; /* [30]                          */
        unsigned mdr1_v_enable : 1;     /* [31]                             */
        } mdr1_r_field;
    } ;
/*                                                                          */
/*  Memory debug 2 Register                                                 */
/*                                                                          */
#define mdr2_m_sel0 0x3F
#define mdr2_m_sel1 0x3F00
#define mdr2_m_sel2 0x3F0000
#define mdr2_m_sel3 0x3F000000
#define mdr2_m_enable 0x80000000
union MDR2 {
    char mdr2_b_displacement [64];      /* space used by this register      */
    int mdr2_l_whole;
    struct  {
        unsigned mdr2_v_sel0 : 6;       /* [5:0]                            */
        unsigned mdr2_v_mdr1_reserve1 : 2; /* [7:6]                         */
        unsigned mdr2_v_sel1 : 6;       /* [13:8]                           */
        unsigned mdr2_v_mdr1_reserve2 : 2; /* [15:14]                       */
        unsigned mdr2_v_sel2 : 6;       /* [21:16]                          */
        unsigned mdr2_v_mdr1_reserve3 : 2; /* [23:22]                       */
        unsigned mdr2_v_sel3 : 6;       /* [29:24]                          */
        unsigned mdr2_v_mdr1_reserve4 : 1; /* [30]                          */
        unsigned mdr2_v_enable : 1;     /* [31]                             */
        } mdr2_r_field;
    } ;
/*                                                                          */
/*  Base Bank Addr Register                                                 */
/*                                                                          */
#define bbar_m_base_addr 0xFFC0
union BBAR {
    char bbar_b_displacement [64];      /* space used by this register      */
    int bbar_l_whole;
    struct  {
        unsigned bbar_v_bbarx_reserved1 : 6; /* [5:0]                       */
        unsigned bbar_v_base_addr : 10; /* [15:6]                           */
        unsigned bbar_v_bbarx_reserved2 : 16; /* [31:16]                    */
        } bbar_r_field;
    } ;
/*                                                                          */
/*  Bank Config Register                                                    */
/*                                                                          */
#define bcr_m_bank_enable 0x1
#define bcr_m_bank_size 0x1E
#define bcr_m_subbank_enable 0x20
#define bcr_m_rowsel 0x40
#define bcr_m_k4bank 0x80
union BCR {
    char bcr_b_displacement [64];       /* space used by this register      */
    int bcr_l_whole;
    struct  {
        unsigned bcr_v_bank_enable : 1; /* [0]                              */
        unsigned bcr_v_bank_size : 4;   /* [4:1]                            */
        unsigned bcr_v_subbank_enable : 1; /* [5]                           */
        unsigned bcr_v_rowsel : 1;      /* [6]                              */
        unsigned bcr_v_k4bank : 1;      /* [7]                              */
        unsigned bcr_v_reserve1 : 24;   /* [31:8]                           */
        } bcr_r_field;
    } ;
/*                                                                          */
/*  Bank Timing Register                                                    */
/*                                                                          */
#define btr_m_row_addr_hold 0x7
#define btr_m_toshiba 0x10
#define btr_m_slow_precharge 0x20
union BTR {
    char btr_b_displacement [64];       /* space used by this register      */
    int btr_l_whole;
    struct  {
        unsigned btr_v_row_addr_hold : 3; /* [2:0]                          */
        unsigned btr_v_btrx_reserved1 : 1; /* [3]                           */
        unsigned btr_v_toshiba : 1;     /* [4]                              */
        unsigned btr_v_slow_precharge : 1; /* [5]                           */
        unsigned btr_v_btrx_reserved2 : 26; /* [31:6]                       */
        } btr_r_field;
    } ;
/* CACHE VALID MAP REGISTERS                                                */
struct CVM {
    unsigned int CACHE_VALID;
    } ;
/*                                                                          */
/*                                                                          */
/*  PYXIS Memory CSR Register Block                                         */
/*                                                                          */
/*  This block of registers contains at all of the registers modelled in the */
/*  structures above in the address space order in which they will appear on the */
/*  machine.                                                                */
/*                                                                          */
struct PYXIS_MEM_CSR {
    union MCR csr_r_mcr;                /* Memory Configuration Register 0x0 */
    union MCMR csr_r_mcmr;              /* Memory Clock mask register 0x40  */
    char csr_b_offset_0 [384];
    union GTR csr_r_gtr;                /* global timing register 0x200     */
    char csr_b_offset_1 [192];
    union RTR csr_r_rtr;                /* refresh timing register 0x300    */
    char csr_b_offset_2 [192];
    union RHPR csr_r_rhpr;              /* row history policy reg 0x400     */
    char csr_b_offset_3 [192];
    union MDR1 csr_r_mdr1;              /* mem ctlr debug reg 1 0x500       */
    union MDR2 csr_r_mdr2;              /* mem ctlr debug reg 2 0x540       */
    char csr_b_offset_4 [128];
    union BBAR csr_r_bbarx [8];         /* Bank base Addr reg 0  0x600      */
    union BCR csr_r_bcrx [8];           /* Bank Config reg 0  0x800         */
    union BTR csr_r_btrx [8];           /* Bank timing reg 0  0xa00         */
    struct CVM csr_r_cvm;               /* Cache valid MAP reg              */
    } ;
/*                                                                          */
/*                                                                          */
#endif
 
