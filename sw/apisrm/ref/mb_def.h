/*
 * Copyright (C) 1990, 1991 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	MOP Block (MB) Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *      phk     31-Oct-1991     Add pointer to the MOP block field MBP
 *
 *	hcb	29-Mar-1991	Add MOPLP_DB, loop_count
 *
 *	hcb	19-Mar-1991	Add to_count, rec_count and done to message 
 *				outstanding
 *
 *	hcb	15-Mar-1991	Add failure flag to message outstanding
 *
 *	jad	25-Jul-1990	Initial entry.
 */
 
/*Constants*/
/*Default environment variables*/
#define MOP$K_LOOP_SIZE 46
#define MOP$K_LOOP_INC 10
#define MOP$K_LOOP_COUNT 1000
#define MOP$K_LP_MSG_NODE 1

/*General constants*/
#define MOP$K_DEV_NAME_MAX 4            /* Device name max characters.      */
#define MOP$K_DONT_INC_LENGTH 0		/* Include the length flags.        */
#define MOP$K_INC_LENGTH 1
#define MOP$K_MEM_OFFSET 0x200000	/*Memory offset*/
#define MOP$K_BOOT_POLL	 16		/*Number of times check kill pending*/
					/*During boot			    */

/*Wait message return values*/
#define MOP$K_SUCCESS 0
#define MOP$K_FAILURE 1
#define MOP$K_KILLED 2

/*Fill pattern constants*/
#define MOP$K_ALL -1			/*All the patterns		    */
#define MOP$K_ALL_ZEROS 0               /*All zero's pattern                */
#define MOP$K_ALL_ONES 1                /*All one's pattern                 */
#define MOP$K_ALL_FIVES 2               /*All five's pattern                */
#define MOP$K_ALL_AAS 3                 /*All AA's pattern                  */
#define MOP$K_INCREMENTING 4            /*Incrementing pattern              */
#define MOP$K_DECREMENTING 5            /*Decrementing pattern              */
#define MOP$K_NUMBER_LOOP_PATT 6	/*Number of loop patterns	    */

/*Build info constants*/
/*These constants must be a power of 2*/
#define MOP$K_CH_IN_MAX 64		/*Reserved console character in max */
#define MOP$K_CH_OUT_MAX 512		/*Reserved console character out max*/

/*Transact constants*/
#define MOP$K_INITIAL_BACKOFF (4*1000)	/*4 Seconds*/
#define MOP$K_MAX_BACKOFF (5*60*1000)	/*5 Minutes*/
#define MOP$K_5_MINUTES (5*60*1000)	/*5 Minutes*/
#define MOP$K_MIN_RETRANSMIT_TIME (4*1000) /*4 Seconds*/
#define MOP$K_BURST_SIZE 5
#define MOP$K_LCG_INDEX 13		/*Index into the LCG*/
#define MOP$K_RETRY_MAX 10		/*Retry max*/
#define MOP$K_ID_RAN 60000		/*Range for random time range + or -*/
					/*for unsolicited ID*/
/*Load progress constants*/
#define MOP$K_LOAD_STARTED 0
#define MOP$K_FOUND_VOLUNTEER 1
#define MOP$K_MEM_LOAD_STARTED 2
#define MOP$K_MEM_LOAD_CONTINUING 3
#define MOP$K_MEM_LOAD_COMPLETE 4

/*First message constants*/
#define MOP$K_FIRST_MSG_EXPECTED  0
#define MOP$K_FIRST_MSG_PENDING   1
#define MOP$K_FIRST_MSG_COMPLETED 2

/*Remote constants*/
#define MOP$K_RC_WATCHDOG_POLL (5*1000)	/*5 Seconds*/
#define MOP$K_RC_CONNECTION_TIMEOUT (MOP$K_SYSID_VAL_CON_TIMER*1000) 

/*Masks*/
#define MOP$M_VERSION 0x4000		/*..0 : Version 3, 1 : Version 4.*/
#define MOP$M_LIP 0x2000		/*..0 : No volunteer, 1 : Loading.*/
#define MOP$M_RC_ENABLED 0x0800		/*..0 : disabled, 1 : enabled.*/
#define MOP$M_RC_RESERVED 0x0400	/*..0 : free, 1 : reserved.*/

/*MOP global structures*/
/*Dump/load file information*/
volatile struct DL_FILE_BLOCK 
{
	struct FILE *fp;		/*Pointer to the next level of protocol*/
	char load_file[256];		/*Name of the file being loaded*/
};

/*MOP loop Database*/
volatile struct MOPLP_DB
{
	struct FILE *fp;		/*Pointer to the next level of protocol*/
	long int lp_count;              /*loop count*/
        long int b_delay;		/*Burst delay*/
};

/*Message received element*/
volatile struct MSG_RCVD {
	struct MSG_RCVD *flink;		/*Forward link*/
	struct MSG_RCVD *blink;		/*Backward link*/
	int version;			/*Packet version*/
	int inc_len;			/*Should we include the length flag*/
	U_INT_16 size;			/*Size of the message*/
	U_INT_8 source[6];		/*Source of the packet*/
	U_INT_8 *msg;			/*Message pointer*/
    };

/*Message outstanding element*/
volatile struct MSG_OUTSTANDING {                     
	int count;			/*Number of packets outstanding*/
	unsigned long crc;		/*CRC of the message*/
	int time;			/*Max time the packet is outstanding*/
	int message_sent;		/*Message sent flag*/
	unsigned char sa[6];		/*Station address*/
	unsigned char fill[2];		/*Fill*/
    };

/*Dump/load globals*/
volatile struct DUMP_LOAD_PARAMS {
    struct SEMAPHORE lock;		/*Lock for the dump load list*/
    struct SEMAPHORE wakeup;		/*Wake up for the dump load process*/
    struct SEMAPHORE ready;		/*Ready to start up/shutdown */
    struct MSG_RCVD qh; 		/*Head of the dump load queue*/
    struct TIMERQ avt;			/*Assistance volunteer timer*/
    struct TIMERQ mlt;			/*Memory load timer*/
    void *load_start_addr;		/*The dump load start address*/
    void *xfer_addr;			/*Transfer address*/     
    int load_status;			/*Load status flag*/
    int load_progress;			/*Load progress flag*/
    int offset;                         /*Offset flag*/
    int offset_length;                  /*Offset_length*/
    int  req_data_size;                 /*Requested from read*/
    int  actual_data_size;              /*Actual data from read*/
    char *data_ptr;                     /*Data pointer*/
    int image_size;			/*Image size*/
    int version;			/*Dump/load version number*/
    U_INT_8 tgt_sys_name[20];		/*Target system name*/
    U_INT_8 host_sys_name[20];		/*Host system name*/
    U_INT_8 tgt_sys_addr[6];		/*Target system address*/
    U_INT_8 host_sys_addr[6];		/*Host system address*/
    U_INT_8 v3_tod[10];			/*V3 time of day*/
    U_INT_8 v4_tod[16];			/*V4 time of day*/
    U_INT_8 va[6];			/*A volunteer address*/
    U_INT_8 seq;			/*Dump/load sequence number*/
    U_INT_8 shutdown;			/* Shutdown dump/load process */
    U_INT_8 fill[2];
};

/*Console globals*/
volatile struct CONSOLE_PARAMS {
    struct SEMAPHORE lock;		/*Lock for the console list*/
    struct SEMAPHORE wakeup;		/*Wake up for the console process*/
    struct MSG_RCVD qh; 		/*Head of the console queue*/
    U_INT_8 seq;			/*Console sequence number*/
    U_INT_8 fill[3];
};

/*Remote console globals*/
volatile struct RC_PARAMS {
    struct SEMAPHORE tx_hole;		/*Transmit spaces in the buffer*/
    struct SEMAPHORE rx_avail;		/*Characters in the rx buffer*/
    int timer;				/*Connection timer*/
    int timer_pid;			/*Connection timer pid*/
    int shell_pid;			/*Shell pid*/
    unsigned int tx_index_in,tx_index_out; /*Transmit index*/
    unsigned int rx_index_in,rx_index_out; /*Receive index*/
    int tx_sent;			/*Character counters*/
    char *tbuf,*rbuf;			/*Transmit and receive buffers*/
    U_INT_8 console_user[6];		/*Current console user*/
    U_INT_8 message_number;		/*Last message number*/
    U_INT_8 fill;
};

/*Loop globals*/
volatile struct LOOP_PARAMS {
    struct QUEUE rq;			/* Reply queue */
    struct SEMAPHORE rq_s;		/* Synchronization semaphore */
    int rq_elements;			/* Number of elements in reply queue */
    struct MSG_RCVD rph; 		/*Head of the loop list*/
    struct MSG_OUTSTANDING *mo;		/*Message outstanding list*/
    int mo_count;			/*Message outstanding count*/
    int kill_server;			/*Kill server flag*/
    int tx_bytes,rx_bytes;		/*Loop sizes*/
    unsigned int moplp_flag;		/*flag for moplp existence*/
    U_INT_8 loop_data[MOP$K_NUMBER_LOOP_PATT] /*Loop data table*/
	     [MOP$K_MAX_DATA_AVAIL_V3_NO_LEN];
};

/*Node ID*/
volatile struct NODE_ID {
    U_INT_8 sa[6];			/*Station address*/
};

/*MOP block*/
volatile struct MB {
    struct LOCK dllock;			/*Must be aligned on a quadword*/
    struct DUMP_LOAD_PARAMS dl;		/*Dump/load parameters*/
    struct LOOP_PARAMS lp;		/*Loop parameters*/
    struct CONSOLE_PARAMS cn;		/*Console parameters*/
    struct RC_PARAMS rc;		/*Remote console parameters*/
    struct SEMAPHORE cmplt_mopdl_s;	/*Completion sem for mop_dump_load*/
    struct SEMAPHORE cmplt_mopid_s; 	/*Completion sem for mop_unsolicited_id*/
    struct SEMAPHORE cmplt_mopcn_s;	/*Completion sem for mop_console */
    int mopid_pid;			/*Needed to set killpending on mopid */
    int no_mop_unsolicited_id;		/* mop_unsolicited_id inhibit flag */
    int no_mop_info;			/* mop request program no info flag */
    unsigned int ndb_count;		/*Number of elements in the network*/
    char short_name [32];		/*Port name (Short version)*/
    U_INT_16 flags;			/*MOP local flags*/
					/*database*/
    U_INT_8 sa[6];			/*Our station address*/
    U_INT_8 sid[8];			/*fddi Station ID*/
    U_INT_8 net_type;			/*MOP network type*/
    U_INT_8 dl_type;			/*Datalink type*/
    char ndb_filename[100];		/*Network database file name*/
    char loop_file[100];		/*Loop file name*/
    };
