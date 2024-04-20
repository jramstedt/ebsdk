/* These stubs are needed by the Sable failsafe image. */
/* The memory diagnostics are not run in the failsafe */
/* image.  The stubs are used so we don't have to carry */
/* around all the extra unused code. */
/* routines from smm_diag.c */
int	mem_init(){
	return(0);
}
int	mem_logic_diag(){
	return(0);
}
int	mem_dram_diag(){
	return(0);
}
int	mem_report_error(){
	return(0);
}
int	mem_configure(){
	return(0);
}
