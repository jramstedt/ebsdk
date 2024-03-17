//; Alpha assembly macro (-m) output created by DEFNR
//   
//  EV6_SYSTEM_PAL_IMPURE.DNR  
//   
//  $Log: impure.h,v $
//  Revision 1.1.1.1  1998/12/29 21:36:09  paradis
//  Initial CVS checkin
//
// Revision 1.9  1998/07/09  16:00:09  samberg
// add cns__write_many
//
// Revision 1.8  1997/11/20  15:58:29  samberg
// put srom parameters back to placeholder location
// only for debug monitor. srm has its own layout.
//
// Revision 1.7  1997/11/12  17:36:45  samberg
// put parens around calculation of cns__size
//
// Revision 1.6  1997/11/12  17:35:40  samberg
// Change CNS size to match size of impure area for pass1/pass2
//
// Revision 1.5  1997/11/12  17:33:45  samberg
// Change srom parameters to end of cns impure area
//
// Revision 1.4  1997/08/16  00:35:36  fdh
// Updated based on the latest EV6_SYSTEM_PAL_IMPURE.DNR file.
//
// Revision 1.3  1997/07/10  19:29:26  gries
// added #define CNS_Q_BASE CNS__PAL_BASE
// #define CNS_Q_SROM_REV (CNS__SIZE + 0)
// #define CNS_Q_PROC_ID (CNS__SIZE + 8)
// #define CNS_Q_MEM_SIZE (CNS__SIZE + 16)
// #define CNS_Q_CYCLE_CNT (CNS__SIZE + 24)
// #define CNS_Q_SIGNATURE (CNS__SIZE + 32)
// #define CNS_Q_PROC_MASK (CNS__SIZE + 40)
// #define CNS_Q_SYSCTX (CNS__SIZE + 48)
//
// Revision 1.2  1997/07/10  02:17:10  fdh
// Manually created from EV6_SYSTEM_PAL_IMPURE.DNR
//
//  Revision 1.1  1997/07/10  02:00:11  fdh
//   
//  EDIT HISTORY  
//  WHO	WHEN		WHAT  
//  ---	----		----  
//  ES	17-SEP-96	MOVED AROUND IMPURE LOCATIONS TO MATCH PREVIOUS  
// 				IMPLEMENTATIONS  
//  ES	17-OCT-96	RENAMED TO EV6_SYSTEM_PAL_IMPURE.DNR  
//  ES	21-NOV-96	CHANGED CBOX ERROR REGISTERS, DELETED THEM FROM CNS  
//  ES	08-APR-97	ELIMINATE CPU_BASE AND SYSTEM_BASE AND SIZE AS  
// 				AS LOCATIONS.  
// 				NOW NEED TO UPDATE THESE MANUALLY!  
//  ES	10-APR-97	CHANGED CBOX READ CHAIN NAMES.  
// 			ADDED PAL_BASE, I_CTL, PROCESS_CONTEXT TO MCHK.  
//  ES	09-APR-97	NEW LOGOUT FRAME ARRANGEMENT.  
//  ES	24-JUN-97	ELIMINATE VA AND VA_CTL. ADD MCHK REV'S. ADD/MOVE  
// 				AROUND RESERVED SPOTS.  
//   
//   
//  DEFINE CNS_SIZE  
//  THIS IS MANUALLY SET!  
//   
#define CNS__SIZE 	(0x3b0)
// 
// DEFINE PREFIX AS CNS 
// 
//   
//  DEFINE OFFSETS  
//   

#define CNS__FLAG 	0x0	// DUMP FLAG 
#define CNS__HALT 	0x8	// HALT CODE 
#define CNS__R0 	0x10	// INTEGER REGISTERS 
#define CNS__R1 	0x18	
#define CNS__R2 	0x20	
#define CNS__R3 	0x28	
#define CNS__R4 	0x30	
#define CNS__R5 	0x38	
#define CNS__R6 	0x40	
#define CNS__R7 	0x48	
#define CNS__R8 	0x50	
#define CNS__R9 	0x58	
#define CNS__R10 	0x60	
#define CNS__R11 	0x68	
#define CNS__R12 	0x70	
#define CNS__R13 	0x78	
#define CNS__R14 	0x80	
#define CNS__R15 	0x88	
#define CNS__R16 	0x90	
#define CNS__R17 	0x98	
#define CNS__R18 	0xa0	
#define CNS__R19 	0xa8	
#define CNS__R20 	0xb0	
#define CNS__R21 	0xb8	
#define CNS__R22 	0xc0	
#define CNS__R23 	0xc8	
#define CNS__R24 	0xd0	
#define CNS__R25 	0xd8	
#define CNS__R26 	0xe0	
#define CNS__R27 	0xe8	
#define CNS__R28 	0xf0	
#define CNS__R29 	0xf8	
#define CNS__R30 	0x100	
#define CNS__R31 	0x108	
#define CNS__F0 	0x110	// FLOATING POINT REGISTERS 
#define CNS__F1 	0x118	
#define CNS__F2 	0x120	
#define CNS__F3 	0x128	
#define CNS__F4 	0x130	
#define CNS__F5 	0x138	
#define CNS__F6 	0x140	
#define CNS__F7 	0x148	
#define CNS__F8 	0x150	
#define CNS__F9 	0x158	
#define CNS__F10 	0x160	
#define CNS__F11 	0x168	
#define CNS__F12 	0x170	
#define CNS__F13 	0x178	
#define CNS__F14 	0x180	
#define CNS__F15 	0x188	
#define CNS__F16 	0x190	
#define CNS__F17 	0x198	
#define CNS__F18 	0x1a0	
#define CNS__F19 	0x1a8	
#define CNS__F20 	0x1b0	
#define CNS__F21 	0x1b8	
#define CNS__F22 	0x1c0	
#define CNS__F23 	0x1c8	
#define CNS__F24 	0x1d0	
#define CNS__F25 	0x1d8	
#define CNS__F26 	0x1e0	
#define CNS__F27 	0x1e8	
#define CNS__F28 	0x1f0	
#define CNS__F29 	0x1f8	
#define CNS__F30 	0x200	
#define CNS__F31 	0x208	
#define CNS__MCHKFLAG 	0x210	// DOES ANYONE USE THIS????? 
#define CNS__IMPURE 	0x218	// IMPORTANT PALTEMPS 
#define CNS__WHAMI 	0x220	
#define CNS__SCC 	0x228	
#define CNS__PRBR 	0x230	
#define CNS__PTBR 	0x238	
#define CNS__TRAP 	0x240	
#define CNS__HALT_CODE 	0x248	
#define CNS__KSP 	0x250	
#define CNS__SCBB 	0x258	
#define CNS__PCBB 	0x260	
#define CNS__VPTB 	0x268	
#define CNS__SROM_REV 	0x270	// for dp264 debug monitor!
#define CNS__PROC_ID 	0x278
#define CNS__MEM_SIZE 	0x280	
#define CNS__CYCLE_CNT 	0x288	
#define CNS__SIGNATURE 	0x290	
#define CNS__PROC_MASK 	0x298	
#define CNS__SYSCTX 	0x2a0	
#define CNS__PLACE_HOLDER18 	0x2a8	
#define CNS__PLACE_HOLDER19 	0x2b0	
#define CNS__PLACE_HOLDER20 	0x2b8	
#define CNS__PLACE_HOLDER21 	0x2c0	
#define CNS__PLACE_HOLDER22 	0x2c8	
#define CNS__PLACE_HOLDER23 	0x2d0	
#define CNS__P4 	0x2d8	// SHADOW REGISTERS 
#define CNS__P5 	0x2e0	
#define CNS__P6 	0x2e8	
#define CNS__P7 	0x2f0	
#define CNS__P20 	0x2f8	
#define CNS__P_TEMP 	0x300	
#define CNS__P_MISC 	0x308	
#define CNS__P23 	0x310	
#define CNS__FPCR 	0x318	// IPRS 
#define CNS__VA 	0x320	
#define CNS__VA_CTL 	0x328	
#define CNS__EXC_ADDR 	0x330	
#define CNS__IER_CM 	0x338	
#define CNS__SIRR 	0x340	
#define CNS__ISUM 	0x348	
#define CNS__EXC_SUM 	0x350	
#define CNS__PAL_BASE 	0x358	
#define CNS__I_CTL 	0x360	
#define CNS__PCTR_CTL 	0x368	
#define CNS__PROCESS_CONTEXT 	0x370	
#define CNS__I_STAT 	0x378	
#define CNS__DTB_ALT_MODE 	0x380	
#define CNS__MM_STAT 	0x388	
#define CNS__M_CTL 	0x390	
#define CNS__DC_CTL 	0x398	
#define CNS__DC_STAT 	0x3a0
#define CNS__WRITE_MANY	0x3a8

//   
//  DEFINE MCHK_CRD__BASE, MCHK_CRD_CPU_BASE,  
//  		MCHK_CRD__SYSTEM_BASE, AND MCHK_CRD__SIZE  
//  THESE ARE MANUALLY SET!  
//  ALSO HAVE REVISION NUMBER.  
//   

#define MCHK_CRD__REV 	0x1	
#define MCHK_CRD__BASE 	0x0	
#define MCHK_CRD__CPU_BASE 	0x18	
#define MCHK_CRD__SYSTEM_BASE 	0x58	
#define MCHK_CRD__SIZE 	0x58	
// 
// DEFINE PREFIX AS MCHK_CRD 
// 
//   
//  DEFINE MCHK_CRD OFFSETS  
//   

#define MCHK_CRD__FLAG_FRAME 	0x0	
#define MCHK_CRD__OFFSETS 	0x8	
#define MCHK_CRD__MCHK_CODE 	0x10	// START OF PAL SPECIFIC 
#define MCHK_CRD__I_STAT 	0x18	// START OF CPU SPECIFIC 
#define MCHK_CRD__DC_STAT 	0x20	
#define MCHK_CRD__C_ADDR 	0x28	
#define MCHK_CRD__DC1_SYNDROME 	0x30	
#define MCHK_CRD__DC0_SYNDROME 	0x38	
#define MCHK_CRD__C_STAT 	0x40	
#define MCHK_CRD__C_STS 	0x48	
#define MCHK_CRD__RESERVED_0 	0x50	
//   
//  DEFINE MCHK__BASE, MCHK__CPU_BASE, MCHK__SYSTEM_BASE, MCHK__SIZE  
//  THESE ARE MANUALLY SET!  
//  ALSO HAVE REVISION NUMBER.  
//   

#define MCHK__REV 	0x1	
#define MCHK__BASE 	0x100	
#define MCHK__CPU_BASE 	0x18	
#define MCHK__SYSTEM_BASE 	0xa0	
#define MCHK__SIZE 	0xa0	
// 
// DEFINE PREFIX AS MCHK 
// 
//   
//  DEFINE MCHK OFFSETS  
//   

#define MCHK__FLAG_FRAME 	0x0	
#define MCHK__OFFSETS 	0x8	
#define MCHK__MCHK_CODE 	0x10	// START OF PAL SPECIFIC 
#define MCHK__I_STAT 	0x18	// START OF CPU SPECIFIC COMMON 
#define MCHK__DC_STAT 	0x20	
#define MCHK__C_ADDR 	0x28	
#define MCHK__DC1_SYNDROME 	0x30	
#define MCHK__DC0_SYNDROME 	0x38	
#define MCHK__C_STAT 	0x40	
#define MCHK__C_STS 	0x48	
#define MCHK__RESERVED_0 	0x50	
#define MCHK__EXC_ADDR 	0x58	// START OF CPU SPECIFIC, MCHK ONLY 
#define MCHK__IER_CM 	0x60	
#define MCHK__ISUM 	0x68	
#define MCHK__MM_STAT 	0x70	
#define MCHK__PAL_BASE 	0x78	
#define MCHK__I_CTL 	0x80	
#define MCHK__PROCESS_CONTEXT 	0x88	
#define MCHK__RESERVED_1 	0x90	
#define MCHK__RESERVED_2 	0x98	


#define CNS_Q_BASE	0
#define CNS_Q_SROM_REV	CNS__SROM_REV
#define CNS_Q_PROC_ID	CNS__PROC_ID
#define CNS_Q_MEM_SIZE	CNS__MEM_SIZE
#define CNS_Q_CYCLE_CNT	CNS__CYCLE_CNT
#define CNS_Q_SIGNATURE	CNS__SIGNATURE
#define CNS_Q_PROC_MASK	CNS__PROC_MASK
#define CNS_Q_SYSCTX	CNS__SYSCTX

#define CNS_Q_GPR	CNS__R0
#define CNS_Q_FPR	CNS__F0
#define CNS_Q_PT	0
#define CNS_Q_EXC_ADDR	CNS__EXC_ADDR
#define CNS_Q_PAL_BASE	CNS__PAL_BASE
#define CNS_Q_EXC_SUM	CNS__EXC_SUM
#define CNS_Q_FPCSR	CNS__FPCR
