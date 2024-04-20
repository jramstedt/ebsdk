/*
 * Copyright (C) 1994 by
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
 * Abstract:	Qlogic ISP1020 definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	03-Feb-1994	Initial entry.
 *  
 *  
 *  	mar	14-Feb-1996	Add constants to determine clock rate (for
 *  				KZPBA due to ship in May 1996.)
 *
 */

#define OPCODE_LOAD_RAM 0x1
#define OPCODE_EXECUTE_FW 0x2
#define OPCODE_WRITE_RAM_WORD 0x4
#define OPCODE_VERIFY_CHECKSUM 0x7
#define OPCODE_ABOUT_FW 0x8
#define OPCODE_INIT_REQUEST_QUEUE 0x10
#define OPCODE_INIT_RESPONSE_QUEUE 0x11
#define OPCODE_EXECUTE_IOCB 0x12
#define OPCODE_BUS_RESET 0x18
#define OPCODE_SET_SCSI_ID 0x30
#define OPCODE_SET_RETRY_COUNT 0x32
#define OPCODE_SET_CLOCK_RATE 0x34
#define OPCODE_SET_ASYNC_SETUP_TIME 0x36
#define OPCODE_SET_TARGET_PARAMS 0x38

#define NVRAM_WRITE_ENABLE ((4<<6)|0x30)
#define NVRAM_WRITE_DISABLE ((4<<6)|0x00)
#define NVRAM_WRITE (5<<6)
#define NVRAM_READ (6<<6)
#define NVRAM_ERASE (7<<6)

#define C1R_BURST_ENA 0x0004
#define C1R_FIFO_8 0x0000
#define C1R_FIFO_16 0x0001
#define C1R_FIFO_32 0x0002
#define C1R_FIFO_64 0x0003
#define ICR_RISC_INT_ENA 0x0004
#define ICR_INT_ENA 0x0002
#define ICR_SOFT_RESET 0x0001
#define ISR_RISC_INT 0x0004
#define ISR_INT 0x0002
#define SR_STATUS 0x0002
#define SR_LOCK 0x0001
#define NR_CLOCK 0x0001
#define NR_SELECT 0x0002
#define NR_DATA_OUT 0x0004
#define NR_DATA_IN 0x0008
#define HCCR_PAUSE_MODE 0x0020
#define HCCR_HOST_INT 0x0080
#define HCCR_RESET_RISC 0x1000
#define HCCR_PAUSE_RISC 0x2000
#define HCCR_RELEASE_RISC 0x3000
#define HCCR_SET_HOST_INT 0x5000
#define HCCR_CLEAR_RISC_INT 0x7000
#define HCCR_WRITE_BIOS_ENA 0x9000
#define PSR_CLOCK_RATE 0x0020

#define C1R 0x6
#define ICR 0x8
#define ISR 0xa
#define SR 0xc
#define NR 0xe
#define MOR 0x70
#define MIR 0x70
#define HCCR 0xc0
#define PSR 0xa0

struct command {
    unsigned char entry_type;
    unsigned char entry_count;
    unsigned char entry_unused;
    unsigned char entry_flags;
    unsigned long handle;
    unsigned char lun;
    unsigned char target;
    unsigned short cdb_length;
    unsigned short control_flags;
    unsigned short reserved;
    unsigned short timeout;
    unsigned short data_count;
    unsigned char cdb[12];
    unsigned long data_address;
    unsigned long data_length;
    unsigned long unused[6];
    } ;

struct status {
    unsigned char entry_type;
    unsigned char entry_count;
    unsigned char entry_unused;
    unsigned char entry_flags;
    unsigned long handle;
    unsigned short scsi_status;
    unsigned short completion_status;
    unsigned short state_flags;
    unsigned short status_flags;
    unsigned short time;
    unsigned short request_sense_length;
    unsigned long residual_length;
    unsigned long unused[2];
    unsigned char request_sense[32];
    } ;

struct iocb {
    struct command command;
    struct status status;
    } ;

struct nvram {
    unsigned long isp_id;
    unsigned char version;
    unsigned fifo_threshold		: 2;
    unsigned not_used0			: 1;
    unsigned host_adapter_enable	: 1;
    unsigned initiator_scsi_id		: 4;
    unsigned char bus_reset_delay;
    unsigned char retry_count;
    unsigned char retry_delay;
    unsigned async_data_setup_time	: 4;
    unsigned req_ack_active_negation	: 1;
    unsigned data_line_active_negation	: 1;
    unsigned data_dma_burst_enable	: 1;
    unsigned command_dma_burst_enable	: 1;
    unsigned char tag_age_limit;
    unsigned low_term_enable		: 1;
    unsigned high_term_enable		: 1;
    unsigned pcmc_burst_enable		: 1;
    unsigned diff_enable		: 1;
    unsigned not_used1			: 4;
    unsigned short selection_timeout;
    unsigned short max_queue_depth;
    unsigned char pad0[12];	
    struct nvram_id {
	unsigned renegotiate_on_error	: 1;
	unsigned stop_queue_on_check	: 1;
	unsigned auto_request_sense	: 1;
	unsigned tagged_queuing		: 1;
	unsigned sync_data_transfers	: 1;
	unsigned wide_data_transfers	: 1;
	unsigned parity_checking	: 1;
	unsigned disconnect_allowed	: 1;
	unsigned char execution_throttle;
	unsigned char sync_period;
	unsigned sync_offset		: 4;
	unsigned device_enable		: 1;
	unsigned not_used2		: 3;
	unsigned char available0;
	unsigned char available1;
	} id[16];
    unsigned char pad1[3];	
    unsigned char check_sum;
    };

#define NVRAM_DEF_FIFO_THRESHOLD		2	/* 32 bytes */
#define NVRAM_DEF_ADAPTER_ENABLE		1
#define NVRAM_DEF_INITIATOR_SCSI_ID		7
#define NVRAM_DEF_BUS_RESET_DELAY		1	/* 1 second */
#define NVRAM_DEF_RETRY_COUNT			0
#define NVRAM_DEF_RETRY_DELAY			1	/* 100ms between tries */
#define NVRAM_DEF_ASYNC_SETUP_TIME		6	/* 4 clock periods */
#define NVRAM_DEF_ASYNC_SETUP_TIME_60           9	/* for 60 Mhz */
#define NVRAM_DEF_REQ_ACK_ACTIVE_NEGATION	1
#define NVRAM_DEF_DATA_ACTIVE_NEGATION		1
#define NVRAM_DEF_DATA_DMA_BURST_ENABLE		1
#define NVRAM_DEF_CMD_DMA_BURST_ENABLE		1
#define NVRAM_DEF_TAG_AGE_LIMIT			8
#define NVRAM_DEF_LOW_TERM_ENABLE		1
#define NVRAM_DEF_HIGH_TERM_ENABLE		1
#define NVRAM_DEF_PCMC_BURST_ENABLE		0
#define NVRAM_DEF_DIFF_ENABLE			0
#define NVRAM_DEF_SELECTION_TIMEOUT		250	/* 250 ms */
#define NVRAM_DEF_MAX_QUEUE_DEPTH		256

#define NVRAM_DEF_RENEGOTIATE_ON_ERROR		1
#define NVRAM_DEF_STOP_QUEUE_ON_CHECK		0
#define NVRAM_DEF_AUTO_REQUEST_SENSE		1
#define NVRAM_DEF_TAGGED_QUEUING		1
#define NVRAM_DEF_SYNC_DATA_TRANSFERS		1
#define NVRAM_DEF_WIDE_DATA_TRANSFERS		1
#define NVRAM_DEF_PARITY_CHECKING		1
#define NVRAM_DEF_DISCONNECT_ALLOWED		1
#define NVRAM_DEF_EXECUTION_THROTTLE		16
#define NVRAM_DEF_SYNC_PERIOD                   25
#define NVRAM_DEF_SYNC_PERIOD_60                12 /* for 60 Mhz */
#define NVRAM_DEF_SYNC_OFFSET                   12
#define NVRAM_DEF_SYNC_OFFSET_60		8  /* for 60 Mhz */
#define NVRAM_DEF_DEVICE_ENABLE			1

#define NVRAM_DEF_ID \
	{ \
	NVRAM_DEF_RENEGOTIATE_ON_ERROR, \
	NVRAM_DEF_STOP_QUEUE_ON_CHECK, \
	NVRAM_DEF_AUTO_REQUEST_SENSE, \
	NVRAM_DEF_TAGGED_QUEUING, \
	NVRAM_DEF_SYNC_DATA_TRANSFERS, \
	NVRAM_DEF_WIDE_DATA_TRANSFERS, \
	NVRAM_DEF_PARITY_CHECKING, \
	NVRAM_DEF_DISCONNECT_ALLOWED, \
	NVRAM_DEF_EXECUTION_THROTTLE, \
	NVRAM_DEF_SYNC_PERIOD, \
	NVRAM_DEF_SYNC_OFFSET, \
	NVRAM_DEF_DEVICE_ENABLE, \
	0, \
	0, \
	0 \
	}

#define NVRAM_DEF \
    'ISP ', \
    2, \
    NVRAM_DEF_FIFO_THRESHOLD, \
    0, \
    NVRAM_DEF_ADAPTER_ENABLE, \
    NVRAM_DEF_INITIATOR_SCSI_ID, \
    NVRAM_DEF_BUS_RESET_DELAY, \
    NVRAM_DEF_RETRY_COUNT, \
    NVRAM_DEF_RETRY_DELAY, \
    NVRAM_DEF_ASYNC_SETUP_TIME, \
    NVRAM_DEF_REQ_ACK_ACTIVE_NEGATION, \
    NVRAM_DEF_DATA_ACTIVE_NEGATION, \
    NVRAM_DEF_DATA_DMA_BURST_ENABLE, \
    NVRAM_DEF_CMD_DMA_BURST_ENABLE, \
    NVRAM_DEF_TAG_AGE_LIMIT, \
    NVRAM_DEF_LOW_TERM_ENABLE, \
    NVRAM_DEF_HIGH_TERM_ENABLE, \
    NVRAM_DEF_PCMC_BURST_ENABLE, \
    NVRAM_DEF_DIFF_ENABLE, \
    0, \
    NVRAM_DEF_SELECTION_TIMEOUT, \
    NVRAM_DEF_MAX_QUEUE_DEPTH, \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID, \
	NVRAM_DEF_ID \
    }, \
    { 0, 0, 0 }, \
    0

/********* for 60 Mhz *********/

#define NVRAM_DEF_ID_60 \
	{ \
	NVRAM_DEF_RENEGOTIATE_ON_ERROR, \
	NVRAM_DEF_STOP_QUEUE_ON_CHECK, \
	NVRAM_DEF_AUTO_REQUEST_SENSE, \
	NVRAM_DEF_TAGGED_QUEUING, \
	NVRAM_DEF_SYNC_DATA_TRANSFERS, \
	NVRAM_DEF_WIDE_DATA_TRANSFERS, \
	NVRAM_DEF_PARITY_CHECKING, \
	NVRAM_DEF_DISCONNECT_ALLOWED, \
	NVRAM_DEF_EXECUTION_THROTTLE, \
	NVRAM_DEF_SYNC_PERIOD_60, \
	NVRAM_DEF_SYNC_OFFSET_60, \
	NVRAM_DEF_DEVICE_ENABLE, \
	0, \
	0, \
	0 \
	}

#define NVRAM_DEF_60 \
    'ISP ', \
    2, \
    NVRAM_DEF_FIFO_THRESHOLD, \
    0, \
    NVRAM_DEF_ADAPTER_ENABLE, \
    NVRAM_DEF_INITIATOR_SCSI_ID, \
    NVRAM_DEF_BUS_RESET_DELAY, \
    NVRAM_DEF_RETRY_COUNT, \
    NVRAM_DEF_RETRY_DELAY, \
    NVRAM_DEF_ASYNC_SETUP_TIME_60, \
    NVRAM_DEF_REQ_ACK_ACTIVE_NEGATION, \
    NVRAM_DEF_DATA_ACTIVE_NEGATION, \
    NVRAM_DEF_DATA_DMA_BURST_ENABLE, \
    NVRAM_DEF_CMD_DMA_BURST_ENABLE, \
    NVRAM_DEF_TAG_AGE_LIMIT, \
    NVRAM_DEF_LOW_TERM_ENABLE, \
    NVRAM_DEF_HIGH_TERM_ENABLE, \
    NVRAM_DEF_PCMC_BURST_ENABLE, \
    NVRAM_DEF_DIFF_ENABLE, \
    0, \
    NVRAM_DEF_SELECTION_TIMEOUT, \
    NVRAM_DEF_MAX_QUEUE_DEPTH, \
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60, \
	NVRAM_DEF_ID_60 \
    }, \
    { 0, 0, 0 }, \
    0
