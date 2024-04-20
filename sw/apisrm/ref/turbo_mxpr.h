/* file:	turbo_mxpr.h
 *
 * Make the C calls "transparent"
*/

extern int cserve();                                

#define success() 			cserve(CSERVE$SUCCESS)
#define min_func()			cserve(CSERVE$MIN_FUNC)
#if EV5
#define mtpr_alt_mode()	       		cserve(CSERVE$MTPR_ALT_MODE)
#define mtpr_bc_config(new_data)	cserve(CSERVE$MTPR_BC_CONFIG,new_data)
#define mtpr_bc_ctl(new_data)		cserve(CSERVE$MTPR_BC_CTL,new_data)
#define mfpr_bc_tag_addr()		cserve(CSERVE$MFPR_BC_TAG_ADDR)
#define mfpr_dbc_config()		cserve(CSERVE$MFPR_DBC_CONFIG)
#define mfpr_dbc_ctl()			cserve(CSERVE$MFPR_DBC_CTL)
#define mtpr_dc_flush(new_data)		cserve(CSERVE$MTPR_DC_FLUSH,new_data)
#define mtpr_dc_mode(new_data)		cserve(CSERVE$MTPR_DC_MODE,new_data)
#define mfpr_dc_mode()			cserve(CSERVE$MFPR_DC_MODE)
#define mfpr_dc_perr_stat()		cserve(CSERVE$MFPR_DC_PERR_STAT)
#define mtpr_dc_perr_stat(new_data)	cserve(CSERVE$MTPR_DC_PERR_STAT,new_data)
#define mfpr_dc_test_ctl()		cserve(CSERVE$MFPR_DC_TEST_CTL)
#define mtpr_dc_test_ctl(new_data)	cserve(CSERVE$MTPR_DC_TEST_CTL,new_data)
#define mfpr_dc_test_tag()		cserve(CSERVE$MFPR_DC_TEST_TAG)
#define mtpr_dc_test_tag(new_data)	cserve(CSERVE$MTPR_DC_TEST_TAG,new_data)
#define mfpr_ei_addr()			cserve(CSERVE$MFPR_EI_ADDR)
#define mfpr_ei_stat()			cserve(CSERVE$MFPR_EI_STAT)
#define mfpr_fill_syn()			cserve(CSERVE$MFPR_FILL_SYN)
#define mtpr_ic_perr_stat(new_data)    cserve(CSERVE$MTPR_IC_PERR_STAT,new_data)
#define mfpr_ic_perr_stat()		cserve(CSERVE$MFPR_IC_PERR_STAT)
#define mfpr_icsr()			cserve(CSERVE$MFPR_ICSR)
#define mtpr_icsr(new_data)		cserve(CSERVE$MTPR_ICSR,new_data)
#define mfpr_isr()			cserve(CSERVE$MFPR_ISR)
#define mfpr_ld_lock()			cserve(CSERVE$MFPR_LD_LOCK)
#define mtpr_maf_mode(new_data)	       	cserve(CSERVE$MTPR_MAF_MODE,new_data)
#define mfpr_maf_mode()			cserve(CSERVE$MFPR_MAF_MODE)
#define mtpr_mcsr_ctl(new_data)		cserve(CSERVE$MTPR_MCSR_CTL,new_data)
#define mfpr_mcsr_ctl()			cserve(CSERVE$MFPR_MCSR_CTL)
#define mfpr_sc_addr()			cserve(CSERVE$MFPR_SC_ADDR)
#define mtpr_sc_ctl(new_data)		cserve(CSERVE$MTPR_SC_CTL,new_data)
#define mfpr_sc_ctl()			cserve(CSERVE$MFPR_SC_CTL)
#define mfpr_sc_stat()			cserve(CSERVE$MFPR_SC_STAT)
#define disable_mchkframe(new_data)     cserve(CSERVE$DISABLE_MCHKFRAME,new_data)
#define wr_bcache_blk(bc_ctl_st,adr_32k,bc_ctl_end,adr,ptrn) cserve(CSERVE$WR_BCACHE_BLK,bc_ctl_st,adr_32k,bc_ctl_end,adr,ptrn)
#endif
#if EV6
#define mfpr_isum()			cserve(CSERVE$MFPR_ISUM)
#define mfpr_i_ctl()			cserve(CSERVE$MFPR_I_CTL)
#define mtpr_i_ctl(new_data)		cserve(CSERVE$MTPR_I_CTL,new_data)
#define disable_mchkframe(new_data)     cserve(CSERVE$DISABLE_MCHKFRAME,new_data)
#define wr_bcache_blk(adr_32k,adr,ptrn) \
					cserve(CSERVE$WR_BCACHE_BLK,adr_32k,adr,ptrn)
#define wr_mem_error(new_data,adr,good_data) \
					cserve(CSERVE$WR_MEM_ERROR,new_data,adr,good_data)
#define mtpr_dc_ctl(new_data)		cserve(CSERVE$MTPR_DC_CTL,new_data)
#endif
#define max_func()			cserve(CSERVE$MAX_FUNC)
