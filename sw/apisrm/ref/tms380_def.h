/* Memory usage for host / DW300 adapter                                    */
/*                                                                          */
/* System Command Blocks (SCBs), System Status Blocks (SSBs), Transmit      */
/* Descriptor List, Receive Descriptor List, and Frame buffers are all      */
/* allocated in host memory.  The adapter transfers data to and from host   */
/* memory by DMA. Only a single gigabyte block is accessible at a time      */
/* using the adapter DMA (the adapter only supports 30 bits of address ..   */
/* address bits 30 and 31 are static and are defined through the mmrcsr.)   */
/*                                                                          */
/* The adapter has on-board memory (128k) that is used by the adapter for   */
/* config / status information accessible to the host, and for buffering    */
/* DMA transfers to/from the host memeory                                   */

/*                                                                          */
/* SCB - System Command Block                                               */
/* --------------------------                                               */
/*                                                                          */
/*  This structure is read by the TMS380 when a command is initiated.       */
/*  Each command defines how this block is used.                            */
/*                                                                          */

typedef struct _scb {
    unsigned short int scb_w_cmd;       /* Command value */
    unsigned short int scb_w_param_0;   /* Parameter */
    unsigned short int scb_w_param_1;   /* Parameter */
    } SCB;

/* test pattern in scb after adapter init */

#define SCB_W_TEST1 0x0000
#define SCB_W_TEST2 0xe2c1
#define SCB_W_TEST3 0x8bd4

/*                                                                          */
/* SSB - System Status Block                                                */
/* -------------------------                                                */
/*                                                                          */
/* This structure is written by the TMS380 when a command finishes.  Each   */
/* command has its own value and usage of the parameters.                   */

typedef struct _ssb {
    unsigned short int ssb_w_cmd;
    unsigned short int ssb_w_param_0;
    unsigned short int ssb_w_param_1;
    unsigned short int ssb_w_param_2;
    } SSB;


/* test pattern in ssb after adapter init */

#define SSB_W_TEST1 0xffff
#define SSB_W_TEST2 0xd7d1
#define SSB_W_TEST3 0xd9c5
#define SSB_W_TEST4 0xd4c3

/*          TMS380 System Interface Function  (SIF)                         */
/* SIFACL, SIFADR, SIFCMD, SIFSTS (etc) CSR definitions                     */
/* ----------------------------------------------------                     */
/*                                                                          */
/* Since these values/locations are adapter specific, their definitions     */
/* will be in a separate include file (tms380.h, .mar, .etc).               */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* SIFACL bits  -- Software Interface Adapter Control                       */
/* -----------                                                              */
/*                                                                          */
/*  This may change as it is learned how the bits are seen on the adapter.  */
/*  It is assumed that the bit definitions will be the same on all devices. */
/*  If this is not true, this section should be made conditional for all    */
/*  conforming devices (and put into adapter specific include file).        */
/*                                                                          */
/*  NOTE: the CSR bit definitions do not need to be byte swapped when using */
/*  16-read/writes                                                          */

    /* psuedo DMA control bits - not used in this driver */
#define ACL_M_SWHLDA  0x0800            /* Software Hold Acknowledge        */
#define ACL_M_SWDDIR  0x0400            /* Current SDDIRR signal value      */
#define ACL_M_SWHRQ   0x0200            /* Current SHRQ signal value        */
#define ACL_M_PSDMAEN 0x0100            /* Pseudo DMA enable                */

#define ACL_M_ARESET  0x0080            /* Adapter Reset                    */
#define ACL_M_CPHALT  0x0040            /* Communication processor halt     */
#define ACL_M_BOOT    0x0020            /* Bootstrapped CP code             */
#define ACL_M_SINTEN  0x0008            /* System interrupt enable          */
#define ACL_M_PEN     0x0004            /* Adapter parity enable            */
#define ACL_M_INPUT0  0X0002            /* Reserved                         */

/* SIFCMD/SIFSTS csr bits                                                       */
/* ------------------                                                       */
/*                                                                          */
/* These registers are normally at the same location and are accessed by    */
/* either a read or write operation.  Write operations access the SIFCMD    */
/* register, read operations access the SIFSTS register.                    */

#define CMD_M_INTADP   0x8000   /* Adapter interrupt                        */
#define CMD_M_RESET    0x4000   /* Software reset (adapter reset)           */
#define CMD_M_SSBCLR   0x2000   /* SSB clear                                */
#define CMD_M_EXECUTE  0x1000   /* Execute command in SCB                   */
#define CMD_M_SCBREQ   0x0800   /* Interrupt when SCB is empty              */
#define CMD_M_RCONCAN  0x0400   /* Receive continue/cancel                  */
#define CMD_M_RCVVAL   0x0200   /* Receive list valid                       */
#define CMD_M_XMTVAL   0x0100   /* Transmit list valid                      */
#define CMD_M_SYSINT   0x0080   /* System Interrupt                         */

#define CMD_M_HARD_RESET 0x7f80 /* All cmd bits, except _INTADP             */
#define CMD_M_DEINSERT 0xc080   /* De-insert from ring                      */
#define CMD_M_REINSERT 0xff80   /* Re-inseert into ring                     */

/* not too sure this is right -- functionally equiv to _REINSERT            */
#define CMD_M_SWRESET  0xfff0   /* All bits must be set for SWreset         */

#define STS_M_INTADP   0x8000   /* Adapter interrupt                        */
#define STS_M_RESET    0x4000   /* Software reset                           */
#define STS_M_SSBCLR   0x2000   /* SSB clear                                */
#define STS_M_EXECUTE  0x1000   /* Executing command in SCB                 */
#define STS_M_SCBREQ   0x0800   /* Interrupt when SCB is empty              */
#define STS_M_RCONCAN  0x0400   /* Receive continue/cancel                  */
#define STS_M_RCVVAL   0x0200   /* Receive list valid                       */
#define STS_M_XMTVAL   0x0100   /* Transmit list valid                      */

#define STS_M_SYSINT   0x0080   /* System interrupt                         */
#define STS_M_INIT     0x0040   /* BUD complete, ready for INIT             */
#define STS_M_TEST     0x0020   /* Test in Progress (SetAfterSWReset)       */
#define STS_M_ERROR    0x0010   /* BUD detected an error                    */
#define STS_M_CODE     0x000f   /* Interrupt/Error code                     */

/*                                                                          */
/* sts_m_code values (for a running system).                                */
/*                                                                          */
#define STS_C_ADPCHK 0          /* Unrecoverable Adapter Check error        */
#define STS_C_ERRITE 0          /* BUD Error - Initial test error           */
#define STS_C_ERRCS 1           /* BUD Error - Adapter checksum error       */
#define STS_C_ERRRAM 2          /* BUD Error - Adapter ram err 1st 128K     */
#define STS_C_ERRTST 3          /* BUD Error - Instruction test error       */
#define STS_C_ERRCTX 4          /* BUD Error - Context/Intrpt test error    */
#define STS_C_RNGSTS 4          /* SSB contains Ring Status Interrupt       */
#define STS_C_ERRPRO 5          /* BUD Error - Protocol Handler Error       */
#define STS_C_LLCSTS 5          /* LLC status in SSB ((not used in MAC))    */
#define STS_C_ERRSIR 6          /* BUD Error - System Interface Reg err     */
#define STS_C_SCBCLR 6          /* SCB has been copied to TMS380            */
#define STS_C_TIMER 7           /* Timer has expired (LLC only)(not used)   */
#define STS_C_CMD 8             /* Command is complete in SSB               */
#define STS_C_RCV 10            /* Receive is complete in SSB               */
#define STS_C_XMT 12            /* Transmit is complete in SSB              */
#define STS_C_RCVPND 14         /* Receive pending (LLC only)(not used)     */

struct DW_REGS {
    unsigned short dw_w_sifdat;        /* SIFDAT register                  */
    unsigned short dw_w_sifinc;        /* SIFDAT/INC register              */
    unsigned short dw_w_sifad1;        /* SIFADR (1st one) register        */
    unsigned short dw_w_sifcmdsts;     /* SIFCMD/STS register              */
    unsigned short dw_w_sifacl;        /* SIFACL register                  */
    unsigned short dw_w_sifadr;        /* SIFADR register                  */
    unsigned short dw_w_sifadx;        /* SIFADX register                  */
    unsigned short dw_w_sifdmalen;     /* DMALEN register                  */
   };
/* _sifad1 and _sifadr both write to the same internal TMS380 register      */

/* TMS380 Commands                                                          */
/* ---------------                                                          */
/*                                                                          */
/* All adapter commands are handled through DMA                             */
/* All parameter blocks and lists are hangled through DMA                   */
/* All reads and writes by adapter to SCBs, SSBs are handled through DMA    */
/* All frame data is handled through DMA                                    */
/*    DMA transfers are in byte order (big-endian) so for host (little-     */
/* endian reads and writes of WORDS, the bytes must be swapped to be        */
/* transferred in the proper order.  Host DOUBLE WORDS must be word-swapped */
/* then each word byte-swapped.                                             */
/*                                                                          */
/* Host read/write directly to adapter registers (16-bit Intel mode):       */
/*   WORDS can be read/written normally                                     */
/*   DOUBLE WORDS: the most significant word must be read/written first,    */
/*      then the least significant word.                                    */
/*                                                                          */
/*                                                                          */
/* The following command / status definitions are byte-swapped              */
/* to eliminate the need to do run-time byte swapping                       */

#define CMD_C_RNGSTS   0x0100   /* SSB_CMD only - Ring Status Int.          */
#define CMD_C_CMDREJ   0x0200   /* SSB_CMD only - Command Reject            */
#define CMD_C_OPEN     0x0300   /* Open                                     */
#define CMD_C_XMIT     0x0400   /* Transmit                                 */
#define CMD_C_XMITHALT 0x0500   /* Transmit Halt                            */
#define CMD_C_RECV     0x0600   /* Receive                                  */
#define CMD_C_CLOSE    0x0700   /* Close                                    */
#define CMD_C_GROUP    0x0800   /* Set group address (last 32 bits)         */
#define CMD_C_FUNC     0x0900   /* Set Functional address                   */
#define CMD_C_RDERR    0x0a00   /* Read Error Log                           */
#define CMD_C_RDADP    0x0b00   /* Read Adapter                             */
#define CMD_C_MDOPEN   0x0d00   /* Modify Open parameters                   */
#define CMD_C_RSOPEN   0x0e00   /* Restore Open parameters                  */
#define CMD_C_F16GRP   0x0f00   /* Set group address (first 16 bits)        */
#define CMD_C_BRIGDE   0x1000   /* Set Bridge params (not used)             */
#define CMD_C_CBRIDGE  0x1100   /* Configure Bridge params (not used)       */

#define STS_C_CLOSED   0x0080   /* Close command succeeded                  */

/* INIT command                                                             */
/* ------------                                                             */
/*                                                                          */
/* The init block must be copied manually to the TMS380, and it has the     */
/* following format;                                                        */
/*                                                                          */
/* w_init_options                                                           */
/* b_xmit_vector    \                                                       */
/* b_cmd_sts_vector  \                                                      */
/* b_rng_sts_vector                                                         */
/* b_rcv_vector         We don't use these byte fields                      */
/* b_adp_chk_vector  /                                                      */
/* b_scb_clr_vector /                                                       */
/* w_rcv_burst_size                                                         */
/* w_xmt_burst_size                                                         */
/* w_dma_abort_thresh                                                       */
/* l_scb_addr                                                               */
/* l_ssb_addr                                                               */

#define INIT_AX 0x0001           /* sifadx address of init block            */
#define INIT_A  0x0A00           /* sifadr address of init block            */

#define INIT_C_DATH 514          /* DMA abort threshold                     */


/* Init options bits.                                                       */

#define INIT_M_RESERVED 0x8000   /* Reserved bit (must be set)              */
#define INIT_M_PARITY   0x09ac   /* Parity enable                           */
#define INIT_M_BRSTSB   0x1000   /* Burst SCB/SSB                           */
#define INIT_M_BRSTLST  0x0800   /* Burst Lists                             */
#define INIT_M_BRSTLSS  0x0400   /* Burst List Status                       */
#define INIT_M_BRSTRCV  0x0200   /* Burst Receive Data                      */
#define INIT_M_BRSTXMT  0x0100   /* Burst Transmit Data                     */
#define INIT_M_LLCENA   0x0040   /* LLC Enable (not used)                   */

/* Init SIFCMD/SIFSTS error bits.                                           */

#define INIT_C_INVIB 1           /* Invalid Initialization Block            */
#define INIT_C_INVOPT 2          /* Invalid Options                         */
#define INIT_C_INVRB 3           /* Invalid Receive Burst Size              */
#define INIT_C_INVTB 4           /* Invalid Transmit Burst Count            */
#define INIT_C_INVDAT 5          /* Invalid DMA abort threshold             */
#define INIT_C_ODDSCB 6          /* Invalid SCB address (odd address)       */

#define INIT_C_ODDSSB 7          /* Invalid SSB address (odd address)       */

#define INIT_C_DIOPAR 8          /* DIO parity error                        */
#define INIT_C_DMATMO 9          /* DMA timeout                             */
#define INIT_C_DMAPAR 10         /* DMA parity error                        */
#define INIT_C_DMABE 11          /* DMA bus error                           */
#define INIT_C_DMADE 12          /* DMA data error                          */
#define INIT_C_ADPCHK 13         /* Adapter check                           */

/* Adapter Internal Pointers (readable only after INIT command and before   */
/* OPEN command).                                                           */

#define INIT_C_AXBASE 0x0001            /* chapter of pointer table         */
#define INIT_C_ABASE  0x0a00            /* base address of pointer table    */
#define INIT_C_PSIZE 16                 /* byte count of pointer table      */

/* all pointers are in Chapter 1 memory                                     */

typedef struct _init_pointer_table {
    unsigned short int init_w_bia;      /* Pointer to "burned-in-address    */
    unsigned short int init_w_slev;     /* Pointer to software level        */
    unsigned short int init_w_addrs;    /* Pointer to adapter addresses     */
    unsigned short int init_w_param;    /* Pointer to adapter parameters    */
    unsigned short int init_w_mac;      /* Useless pointer to MAC buffer    */
    unsigned short int init_w_llc;      /* Useless pointer to LLC counters  */
    unsigned short int init_w_speed;    /* Pointer to last set speed        */
    unsigned short int init_w_ram;      /* Pointer to total adapter RAM     */
    } INIT_POINTER_TABLE;


/* Software level data */
typedef struct _tms380_slevel {
    unsigned short int w_1;  /* vendor code msb, 2nd msb */
    unsigned short int w_2;  /* vendor code lsb, microcode type */
    unsigned short int w_3;  /* chip type, chip step msb */
    unsigned short int w_4;  /* chip step lsb, microcode rev msb */
    unsigned short int w_5;  /* microcode 2nd msb, lsb */
    } SW_LEVEL_INFO;


/* Structure of init_w_param table                                          */

#define INIT_C_PARAMSIZE 68
typedef struct _init_adp_params {
    unsigned int init_l_phydrp;         /* Physical drop number             */
    unsigned char init_b_una [6];       /* Upstream neighbor address        */
    unsigned int init_l_unaphydrp;      /* Upstream neighbor phys drop num  */

    unsigned char init_b_poll [6];      /* Last ring poll address           */
    unsigned short int init_w_res1;     /* Reserved                         */
    unsigned short int init_w_xmtap;    /* Transmit access priority         */
    unsigned short int init_w_srcca;    /* Source class authorization       */
    unsigned short int init_w_lstatt;   /* Last attention code              */
    unsigned char init_b_lstsrc [6];    /* Last source address              */
    unsigned short int init_w_lstbea;   /* Last beacon type                 */
    unsigned short int init_w_lstvec;   /* Last major vector                */
    unsigned short int init_w_rngsts;   /* Ring status                      */
    unsigned short int init_w_setime;   /* Soft error timer value           */
    unsigned short int init_w_rierr;    /* Ring Interface error counter     */
    unsigned short int init_w_local;    /* Local Ring number                */
    unsigned short int init_w_monerr;   /* Monitor Error Code               */
    unsigned short int init_w_beatt;    /* Beacon Transmit type             */
    unsigned short int init_w_beart;    /* Beacon Receive type              */
    unsigned short int init_w_fcs;      /* Frame correlator save            */
    unsigned char init_b_beauna [6];    /* Beaconing station's UNA          */
    unsigned int init_l_res2;           /* Reserved                         */
    unsigned int init_l_beapd;          /* Beacning station's phys drop num */
    } INIT_ADP_PARAMS;

/* RING.STATUS Interrupt                                                    */
/* ---------------------                                                    */
/*                                                                          */
/* These bits are set in the ssb_param_0 upon the RING.STATUS interrupt.    */
/* Some of these events occur often.                                        */

#define RSTS_M_SIGLOSS 0x0080           /* Signal Loss                      */
#define RSTS_M_HARDERR 0x0040           /* Hard error                       */
#define RSTS_M_SOFTERR 0x0020           /* Soft error                       */
#define RSTS_M_XMTBEA  0x0010           /* Xmit beacon                      */
#define RSTS_M_LOBE    0x0008           /* Lobe wire fault                  */
#define RSTS_M_ARERR   0x0004           /* Auto removal error               */
#define RSTS_M_RRECV   0x0001           /* Remove received                  */
#define RSTS_M_CNTOVR  0x8000           /* Counter overflow                 */
#define RSTS_M_SS      0x4000           /* Single Station                   */
#define RSTS_M_RR      0x2000           /* Ring Recovery                    */

/* COMMAND.REJECT Interrupt                                                 */
/* ------------------------                                                 */
/*                                                                          */
/* The following bits are set in the ssb_param_0 word. ssb_param_1 contains */
/* the command value that failed.                                           */

#define CR_M_ILLEGAL   0x0100           /* Illegal Command                  */
#define CR_M_ADDERROR  0x0200           /* Address Error                    */
#define CR_M_ADPOPEN   0x0400           /* Adapter is Open                  */
#define CR_M_ADPCLOSED 0x0800           /* Adapter is Closed                */
#define CR_M_SAMECMD   0x1000           /* Same command is executing        */

/* ADAPTER.CHECK Interrupt                                                  */
/* -----------------------                                                  */
/*                                                                          */
/* The following information comes from reading 8 bytes from the adapter    */
/* upon the interrupt.  This must be read via DIO since the DMA engine on   */
/* the adapter is dead as a result of this interrupt (therefore the         */
/* READ.ADAPTER command cannot function).  Must perform a hard reset after  */
/* this interrupt.                                                          */

/*                                                                          */
/* w_adp_chk_sts        0x105e0 Is base address for this information.       */
/* w_adp_chk_parm_0                                                         */
/* w_adp_chk_parm_1                                                         */
/* w_adp_chk_parm_2                                                         */
/*                                                                          */
            
#define ADAPCHK_AX 0x1
#define ADAPCHK_A  0x05e0            

#define AC_C_CFSIZE 8                   /* Size (bytes) in the check frame  */
typedef struct _check_frame {
    unsigned short int w_adp_chk_sts;
    unsigned short int w_adp_chk_parm_0;
    unsigned short int w_adp_chk_parm_1;
    unsigned short int w_adp_chk_parm_2;
    } CHECK_FRAME;

/* byte swapped values */

#define AC_M_DIOPAR      0x0080         /* Dio parity error                 */
#define AC_M_DMARDABORT  0x0040         /* DMA Read Abort                   */
#define AC_M_DMAWRABORT  0x0020         /* DMA Write Abort                  */
#define AC_M_ILLEGALOP   0x0010         /* Illegal opcode                   */
#define AC_M_PARITY      0x0008         /* Parity errors                    */
#define AC_M_RAMDATA     0x8000         /* RAM data error                   */
#define AC_M_RAMPARITY   0x4000         /* RAM parity error                 */
#define AC_M_RINGUNDER   0x2000         /* Internal DMA underrun            */
#define AC_M_INVINTER    0x0800         /* Invalid Interrupt                */

#define AC_M_INVERRINT   0x0400         /* Invalid error interrupt          */
#define AC_M_INVALXOP    0x0200         /* Invalid XOP                      */

/* OPEN command                                                             */
/* ------------                                                             */
/*                                                                          */
/* The structure that is passed to TMS380 in the OPEN command.              */

typedef struct _cmd_open {
    unsigned short int opn_w_options;   /* Open options (see below)         */
    unsigned short int opn_w_node_address [3]; /* if zero, uses BIA rom     */
    unsigned int opn_l_group_address;   /* Low 32 bits of group addr        */
    unsigned int opn_l_functional_address; /* Functional address mask       */
    unsigned short int opn_w_rcv_list_size; /* Size of receive list         */
    unsigned short int opn_w_xmt_list_size; /* Size of xmit list            */
    unsigned short int opn_w_buffer_size; /* Internal TMS380 buff size      */
    unsigned int opn_l_reserved1;       /* Reserved                         */
    unsigned char opn_b_xmt_buf_min;   /* Min bufs reserved for xmt         */
    unsigned char opn_b_xmt_buf_max;   /* Max bufs to use for xmt           */
    unsigned int opn_l_product_id_address; /* Pointer to product id         */
    } CMD_OPEN;

/* Open options bits                                                        */

/* THESE OPTION BITS ARE BYTE SWAPPED                                       */

#define OPN_M_WRAP        0x0080        /* Wrap interface -- Loopback mode  */
#define OPN_M_DISHARD     0x0040        /* Disable Hard Errors              */
#define OPN_M_DISSOFT     0x0020        /* Disable Soft Errors              */
#define OPN_M_PASSMAC     0x0010        /* Pass Adapater MAC frames         */
#define OPN_M_PASSATN     0x0008        /* Pass Attention MAC frames        */
#define OPN_M_PADROUTE    0x0004        /* Pad routing field to 18 bytes    */
#define OPN_M_FRAMEHOLD   0x0002        /* Frame Hold                       */
#define OPN_M_CONTENDER   0x0001        /* Participate in contender process */
#define OPN_M_PASSBEA     0x8000        /* Pass beacon MAC frames           */
#define OPN_M_MBZ         0x6800        /* must be zero                     */
#define OPN_M_ETR         0x1000        /* Early Token Release              */
#define OPN_M_COPYALL     0x0400        /* Copy all frames (not in TURBOMAC)*/
#define OPN_M_COPYNMAC    0x0200        /* Copy all non-mac frames (N/A)    */
#define OPN_M_PASSFIRST   0x0100        /* Pass first buffer only           */

/* Open Status/Error                                                        */
/*                                                                          */
/* These bits/codes are set in ssb_param_0.                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* OPEN_COMPLETION bits                                                     */

#define OPN_M_SUCCESS 0x0080            /* Open is succesful                */
#define OPN_M_NODEERR 0x0040            /* Node address error               */
#define OPN_M_LISTERR 0x0020            /* List Size Error                  */
#define OPN_M_BUFFERR 0x0010            /* Buffer Size Error                */
#define OPN_M_XMTERR  0x0004            /* Transmit Buffer Count Error      */
#define OPN_M_OPNERR  0x0002            /* Open Error                       */
#define OPN_M_INVOPT  0x0001            /* Invalid open option              */

/* OPEN PHASES codes                                                        */

#define OPN_M_PHASECODE 0xf000          /* Mask for extracting phase code   */
#define OPN_C_LOBEMED   0x1000          /* Lobe media test                  */
#define OPN_C_PHYSINS   0x2000          /* Physical insertion               */
#define OPN_C_ADDRVER   0x3000          /* Address Verification             */
#define OPN_C_PARTPOLL  0x4000          /* Participation in Ring Poll       */
#define OPN_C_REQINIT   0x5000          /* Request initialization failed    */

/* OPEN ERROR codes                                                         */

#define OPN_M_ERRCODE  0x0f00           /* Mask for extracting error code   */
#define OPN_C_FUNFAIL  0x0100           /* Adapter's loopbacks don't work   */
#define OPN_C_SIGLOSS  0x0400           /* Signal lost on ring              */
#define OPN_C_TIMEOUT  0x0500           /* Insert timed out                 */
#define OPN_C_RINGFAIL 0x0600           /* Bad Ring (ring failure)          */
#define OPN_C_RINGBEAC 0x0700           /* Bad connection (ring beaconing)  */
#define OPN_C_DUPADDR  0x0800           /* Duplicate address exists on ring */
#define OPN_C_REQINIT2 0x0900           /* Request initiailization failed   */
#define OPN_C_REMOVE   0x0a00           /* Remove received                  */

/* TRANSMIT and RECEIVE Parameter Lists                                     */
/* ------------------------------------                                     */
/*                                                                          */
/* The following defines the structure of the transmit and receive          */
/* parameter lists. These fields, except for the last three, are big-endian */
/* byte ordered to yield the correct byte ordering in the adapter memory    */
/* after DMA movement of data between the adapter and system memory.        */
/* ssb_w_status is byte swapped - it is a copy of the xmt or rcv status     */
/* from the ssb.  pl_a_addr, and pl_a_next are normal (little_endian)       */
/* pointers to the associated translation buffer and the next parameter     */
/* list in the xmt or rcv rings.                                            */ 

#define PL_C_BUFFADDR 1                 /* Number buffer addr entries/list  
                                         * VALID RANGE 1..3 */
typedef struct _param_list {
    unsigned int pl_l_next;             /* Swapped PA of next list entry    */
    unsigned short int pl_w_cstat;
    unsigned short int pl_w_size;
    struct  {                           /* Assumes list size of 26 is used  */
        unsigned short int pl_w_count;  /* byte count for data buffer       */
        unsigned int pl_l_addr;         /* Swapped PA of data buffer        */
        } pl_buf [PL_C_BUFFADDR];
/* -------------------- additional variables used only by the driver ------ */
    unsigned short int ssb_w_status;    /* saved status from ssb            */
    TOKEN_RING_HEADER *pl_a_buf;        /* ptr to xlate buffer              */
    struct _param_list *pl_a_next;      /* ptr to next list entry           */ 
    } PARAM_LIST;


#define PL_C_SIZE 32                    /* Size of list entry               */
#define PL_M_ODDFP 16777216             /* Odd forward pointer value        */
#if !defined(__VAXC) && !defined(VAXC)
#undef pl_w_count
#undef pl_l_addr
#endif          /* #if !defined(__VAXC) && !defined(VAXC) */

/*                                                                          */
/* Buff chaining constant (bit15 of the pl_r_buff[].pl_w_count)             */
/* field is set to 1 to indicate the next entry in the table                */
/* is used (0 if entry is last in the list entry).                          */
/*                                                                          */
#define PL_M_CONTINUE 128
/*                                                                          */
/* TRANSMIT CSTAT bits/values                                               */
/*                                                                          */

#define TC_M_VALID      0x0080          /* Adapter owns when set            */
#define TC_M_COMPLETE   0x0040          /* must be zero on request          */
#define TC_M_START      0x0020          /* List starts a frame              */
#define TC_M_END        0x0010          /* List ends a frame                */
#define TC_M_INTER      0x0008          /* Causes interrupt on completion   */
#define TC_M_XERR       0x0004          /* Transmit error                   */
#define TC_M_CRC        0x0002          /* Don't generate CRC               */
#define TC_M_SOURCE     0x0001          /* Don't override source address    */
#define TC_M_FS         0xff00          /* Transmit frame status            */

/* RECEIVE CSTAT bits/values                                                */

#define RC_M_VALID      0x0080          /* Adapter owns when set            */
#define RC_M_COMPLETE   0x0040          /* must be zero on request          */
#define RC_M_START      0x0020          /* List starts a frame              */
#define RC_M_END        0x0010          /* List ends a frame                */
#define RC_M_INTER      0x0008          /* Causes interrupt on completion   */
#define RC_M_IFWAIT     0x0004          /* Interframe Wait                  */
#define RC_M_CRC        0x0002          /* Include CRC with receive         */
#define RC_M_FS         0xfc00          /* Receive frame status             */
#define RC_M_AM         0x0300          /* Address match                    */

/* TRANSMIT command status bits                                             */
/* ----------------------------                                             */
/*                                                                          */
/* The following bits appear in ssb_w_param_0 upon transmit complete        */
/* interrupt.                                                               */

#define TS_M_COMPLETE  0x0080           /* Command complete                 */
#define TS_M_FRAME     0x0040           /* Frame complete                   */
#define TS_M_LERR      0x0020           /* List error                       */
#define TS_M_FSIZERR   0x8000           /* Frame size error                 */
#define TS_M_THRESH    0x4000           /* Transmit threshold               */
#define TS_M_ODD       0x2000           /* Odd address                      */
#define TS_M_FERR      0x1000           /* Frame error                      */
#define TS_M_APERR     0x0800           /* Access Priority error            */
#define TS_M_UMFRAME   0x0400           /* Unenabled MAC Frame              */
#define TS_M_IFF       0x0200           /* Illegal Frame Format             */

/* RECEIVE command status bits                                              */
/* ---------------------------                                              */
/*                                                                          */
/* The following bits appear in ssb_w_param_0 upon receive complete         */
/* interrupt.                                                               */

#define RS_M_COMPLETE 0x0080            /* Frame complete                   */
#define RS_M_SUSPEND  0x0040            /* Receive suspended                */
           
/* Standard OK completion value                                             */
/* ----------------------------                                             */
/*                                                                          */
/* This value is used in ssb_w_param_0 for several comands to indicate      */
/* successful execution of the command.                                     */

#define CMD_C_SUCCESS 0x0080

/* READ.ERROR.LOG command                                                   */
/* ----------------------                                                   */

typedef struct _error_log {
    U_INT_8 erl_b_line_error;
    U_INT_8 erl_b_res1;
    U_INT_8 erl_b_burst_error;
    U_INT_8 erl_b_arifci_error;
    U_INT_8 erl_b_res2;
    U_INT_8 erl_b_res3;
    U_INT_8 erl_b_lostframe;
    U_INT_8 erl_b_rcv_congest;
    U_INT_8 erl_b_framecopy;
    U_INT_8 erl_b_res4;
    U_INT_8 erl_b_tokenerr;
    U_INT_8 erl_b_res5;
    U_INT_8 erl_b_dmabuserr;
    U_INT_8 erl_b_dmaparerr;
    } ERROR_LOG;

/* READ.ADAPTER command                                                     */
/* --------------------                                                     */
/*                                                                          */
/* This structure must reside in host memory to initiate an adapter read    */

typedef struct _read_adp {
    unsigned short int ra_w_count;      /* Bytes to read                    */
    unsigned short int ra_w_addr;       /* Chapter 1 memory to read         */
    } READ_ADP;

/* Buffering parameters                                                     */
/* --------------------                                                     */

#define TMS380_C_RCV_ALIGN 8            /* Quadword align buffers           */
#define TMS380_C_ARCV_ALIGN 3           /* Quadword align buffers           */
#define TMS380_C_RCV_ENTRIES 32         /* Recieve List entries             */
#define TMS380_C_XMT_ENTRIES 32         /* Transmit List entries            */
#define TMS380_C_RCV_BSZ 4530

/* Receive buffer size                                                      */
/*                                                                          */
/* Xmit specific paramters to tms380$xmit_data                              */
/* -------------------------------------------                              */

#define TMS380_C_XMIT_TAIL 0            /* Save on tail of xmtreq           */
#define TMS380_C_XMIT_HEAD 1            /* Save at head of xmtreq           */

/* SCB, SSB, and LISTs configuration constants                              */
/* -------------------------------------------                              */
/*                                                                          */
/* The aim is to provide quadword aligned addresses for the start of all    */
/* data structures that are shared with the device.  All sizes are bytes.   */
/*                                                                          */
/* These device shared data structures reside in physically contiguous      */
/* memory and is divided up in the following manner;                        */
/*                                                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  SCB                                       */
/* |                           | /                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  SSB                                       */
/* |                           | /                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  Open Parameters                           */
/* |                           | /                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  Transmit Lists                            */
/* |                           | /                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  Receive Lists                             */
/* |                           | /                                          */
/* +---------------------------+                                            */
/* |                           | \                                          */
/* |                           |  Scratch Area (for READ.ADAPTER???)        */
/* |                           | /                                          */
/* +---------------------------+                                            */
/*                                                                          */

#define TMS380_C_SCBLEN 8
#define TMS380_C_SSBLEN 8
#define TMS380_C_OPNLEN 32
#define TMS380_C_XMTLEN 1024
#define TMS380_C_RCVLEN 1024
#define TMS380_C_SCRLEN 104
#define TMS380_C_MEMLEN 2200

