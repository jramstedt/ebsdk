#if DEBUG
extern dump_ci_pb(struct ci_pb *pb);
extern dump_pb(struct pb *pb);
extern dump_cixcd_blk(struct cixcd_blk *blk_ptr);
extern dump_type_ptr(TYPE_PTR *type_ptr);
extern dump_pa_ptr(PA_PTR *pa_ptr);
extern dump_ampb_ptr(AMPB_PTR *ampb_ptr);
extern dump_buffer_name(BUF_NAME *buffer_name);
extern dump_bdl(BDL *bdl);
extern dump_bdl_ptr(BDL_PTR *bdl_ptr);
extern dump_n_flags(N_FLAGS *n_flags);
extern dump_n_status(N_STATUS *n_status);
extern dump_n_q_buffer(N_Q_BUFFER *n_q_buffer);
extern dump_n_ccq(N_CCQ *n_ccq);
extern dump_n_apb(N_APB *n_abp);
extern dump_n_cpb(N_CPB *n_cpb);
extern dump_nport_ab(NPORT_AB *nport_ab);
extern dump_vcd(VCD *vcd);
extern dump_n_setckt(N_SETCKT *n_setckt);
extern dump_n_cktset(N_CKTSET *n_cktset);
extern dump_n_chnv(N_CHNV *n_chnv);
extern dump_n_setchnl(N_SETCHNL *n_setchnl);
extern dump_n_chnlset(N_CHNLSET *n_chnlset);
extern dump_n_prugq(N_PURGQ *n_purgq);
extern dump_n_qpurge(N_QPURG *n_qpurg);
#endif
