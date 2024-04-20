#define FC_MAXLOOP  126


struct fc_ring {
    /*
     * The rcv queu structure is if we need to get sophisticated and 
     * allocate rcv buffers on the fly.  We have made the decision that
     * this is overkill and are simplifying (aug 27, 97)
     */
    struct QUEUE rcv_buf_q;
    int		 rcv_buf_cnt;
    int          rcv_buf_highwater;

    /*
     * In the simplified version, we just keep a pointer to a block of
     * contiguously allocated buffers.
     */
    char	 *rcv_blk;

    struct QUEUE pending_q;
    struct QUEUE active_q;
    struct QUEUE done_q;
    struct QUEUE receive_q;
    struct SEMAPHORE pending_s;
    struct SEMAPHORE active_s;
    struct SEMAPHORE done_s;
    struct SEMAPHORE receive_s;
        uchar	fc_numCiocb;		/* number of command iocb's per ring */
        uchar	fc_numRiocb;		/* number of rsp iocb's per ring */
	uchar	fc_rspidx;		/* current index in response ring */
	uchar	fc_cmdidx;		/* current index in command ring */
	ulong	fc_cmdringaddr;		/* virtual offset for cmd rings */
	ulong	fc_rspringaddr;		/* virtual offset for rsp rings */
	ushort	fc_iotag;		/* used to identify I/Os */ 
	ushort	fc_ringno;		/* ring number */
};
typedef struct fc_ring RING;


/*
*** Board Information Data Structure
*/

struct fc_brd_info
{

	volatile int	fc_ffstate;	/* Current state of FF init process */


 	/* Configuration Parameters */
	int	fc_ffnumrings;		/* number of FF rings being used */
	ulong	fc_nummask[4];		/* number of masks/rings being used */
	ulong	fc_rval[6];   		/* rctl for ring assume mask is 0xff */
	ulong	fc_tval[6];   		/* type for ring assume mask is 0xff */
	ulong	fc_edtov;		/* E_D_TOV timer value */
	ulong	fc_arbtov;		/* ARB_TOV timer value */
	ulong	fc_ratov;		/* R_A_TOV timer value */
	ulong	fc_rttov;		/* R_T_TOV timer value */
	ulong	fc_altov;		/* AL_TOV timer value */
	ulong	fc_crtov;		/* C_R_TOV timer value */
	ulong	fc_citov;		/* C_I_TOV timer value */
	ulong	fc_myDID;		/* fibre channel S_ID */

	/* The next three structures get DMA'ed directly into,
	   so they must be in the first page of the adapter structure! */
	SERV_PARM fc_sparam;		/* buffer for our service parameters */
	uchar	alpa_map[128];		/* AL_PA map from READ_LA */

	RING	fc_ring[4];
};

typedef struct fc_brd_info FC_BRD_INFO;


