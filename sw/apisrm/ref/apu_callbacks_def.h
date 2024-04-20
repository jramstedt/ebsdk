/********************************************************************************************************************************/
/* Created  2-FEB-1994 06:59:12 by VAX SDL V3.2-12     Source:  2-FEB-1994 06:59:11 MORGN:[PEACOCK.AVANTI.TOOLS]CALLBACKS_DEF.SDI */
/********************************************************************************************************************************/
 
/*** MODULE cp_callbacks IDENT V1.0 ***/
/* Console Device I/O callback function codes                               */
#define cbfunc$k_getc 1
#define cbfunc$k_puts 2
#define cbfunc$k_reset_term 3
#define cbfunc$k_set_term_int 4
#define cbfunc$k_set_term_ctl 5
#define cbfunc$k_process_keycode 6
/* Console Boot Device I/O callback function codes                          */
#define cbfunc$k_open 16
#define cbfunc$k_close 17
#define cbfunc$k_ioctl 18
#define cbfunc$k_read 19
#define cbfunc$k_write 20
/* Console Environment Variable Function codes                              */
#define cbfunc$k_set_env 32
#define cbfunc$k_reset_env 33
#define cbfunc$k_get_env 34
#define cbfunc$k_save_env 35
/* Console Callback function Status codes                                   */
#define CBS$SUCCESS 0
#define CBS$ERROR 1
/* Console Callback Status definitions                                      */
/* GETC definitions                                                         */
#define CBS$SUCCESS_MORE_TO_READ 1
#define CBS$FAIL_TERM_NOT_READY 4
#define CBS$FAIL_RCV_ERROR 6
#define CBS$FAIL_RCV_ERROR_MORE 7
#define cbs$v_status 61
#define cbs$v_cdev_status 48
#define cbs$v_unit 32
#define cbs$v_char_read 0
/* PUTS Definitions                                                         */
#define CBS$SUCCESS_SOME 1
#define CBS$FAIL_TERM_NO_BYTES 6
#define CBS$FAIL_TERM_SOME_BYTES 7
#define cbs$v_bytes_written 0
/* RESET_TERM defintions                                                    */
#define cbs$v_bit_status 63
/* SET_TERM_INT definitions                                                 */
#define CBS$TX_INT_ENA 1
#define CBS$RX_INT_ENA 1
#define cbs$v_tx_int_ena 0
#define cbs$v_rx_int_ena 1
#define cbs$k_ena_tx_ints 16
#define cbs$k_dis_tx_ints 0
#define cbs$m_tx_int_cmd 3
#define cbs$k_ena_rx_ints 16
#define cbs$k_dis_rx_ints 0
#define cbs$m_rx_int_cmd 48
/* PROCESS_KEYCODE definitions                                              */
#define CBS$FAIL_MORE_TIME_NEEDED 5
#define CBS$FAIL_NOT_SUPPORTED 6
#define CBS$FAIL_NO_CHARACTER 7
#define CBS$SUCCESS_ERR_CORR 0
#define CBS$FAIL_ERR_NOT_CORR 1
#define cbs$v_corr_error 60
/* OPEN definitions                                                         */
#define CBS$FAIL_DEV_NOT_EXISTS 2
#define CBS$FAIL_DEV_NO_ACCESS 3
#define cbs$v_bit2_status 62
#define cbs$v_gdev_status 32
/* IOCTL defintions                                                         */
/* READ and WRITE Definitions                                               */
#define CBS$FAIL_END_OF_DEV 1073741824
#define cbs$v_end_of_dev 62
#define CBS$FAIL_ILL_REC_LENGTH 536870912
#define cbs$v_ill_rec_length 61
#define CBS$FAIL_RUN_OFF_TAPE 268435456
#define cbs$v_run_off_tape 60
#define CBS$FAIL_READ -2147483648
#define CBS$FAIL_WRITE -2147483648
#define CBS$FAIL_READ_WRITE -2147483648
/* SET_ENV Definitions                                                      */
#define CBS$FAIL_VAR_NOT_RECOG 6
#define CBS$FAIL_VAR_READ_ONLY 4
#define CBS$FAIL_BYTE_STREAM_TOO_LONG 7
/* GET_ENV Definitions                                                      */
#define CBS$SUCCESS_BYTE_STREAM_TRUNC 1
#define CBS$FAIL_BST_AND_VRO 5
/* SAVE_ENV Definitions                                                     */
#define CBS$SUCCESS_SOME_SAVED 1
#define CBS$FAIL_SAVING_VALS 7
/* Channel Descriptor Data Structure                                        */
struct chan_desc {
    char *chan$a_dev_string;
    int chan$l_dev_string_length;
    int (*chan$a_boot_read)();
    int (*chan$a_boot_write)();
    int (*chan$a_boot_ioctl)();
    unsigned int chan$l_status [2];
    int chan$l_capacity;           /* for capacity of disk             */
    int chan$l_block_size;         /* for block size                   */
    int chan$l_scsi_bus;           /* SCSI_A or SCSI_B...open rtn loads this */
    int chan$l_device_type;        /* disk, tape, mop, ... open rtn loads this */
    int chan$l_scsi_lun;           /* SCSI logical unit number         */
    int chan$l_scsi_id;            /* SCSI id                          */
    unsigned char *chan$a_buffer_va;
    char chan$b_ethernet_address [8];
    } ;
/* Environment Variable Descriptor                                          */
#define env$m_size 65535
#define env$m_flag_nvr 65536
#define env$m_flag_wnvr 131072
#define env$m_flag_ro_sys 262144
#define env$m_flag_ro_all 524288
#define env$m_flag_cons_act 1048576
#define env$m_flag_unused 14680064
#define env$m_id -16777216
struct env_flags {
    unsigned env$v_size : 16;
    unsigned env$v_flag_nvr : 1;
    unsigned env$v_flag_wnvr : 1;
    unsigned env$v_flag_ro_sys : 1;
    unsigned env$v_flag_ro_all : 1;
    unsigned env$v_flag_cons_act : 1;
    unsigned env$v_flag_unused : 3;
    unsigned env$v_id : 8;
    } ;
struct env_desc {
    int env$l_flags;
    int env$l_offset;
    } ;
/* Environment Variable ID values                                           */
#define envid$auto_action 1
#define envid$boot_dev 2
#define envid$bootcmd_dev 3
#define envid$booted_dev 4
#define envid$boot_file 5
#define envid$booted_file 6
#define envid$boot_osflags 7
#define envid$booted_osflags 8
#define envid$boot_reset 9
#define envid$dump_dev 10
#define envid$enable_audit 11
#define envid$license 12
#define envid$char_set 13
#define envid$language 14
#define envid$tty_dev 15
#define envid$op_updown 64
#define envid$op_size 65
#define envid$scsi_hostids 66
#define envid$fast_scsi 67
#define ppid$op_updown 11
#define ppid$op_size 126
struct boot_block {
    int bb$l_reserved_0 [34];
    int bb$l_reserved_1 [86];
    int bb$l_count [2];
    int bb$l_start_lbn [2];
    int bb$l_flags [2];
    int bb$l_checksum [2];
    } ;
/* Boot definitions                                                         */
#define boot$k_proto_mscp 0
#define boot$k_proto_scsi 1
#define boot$k_proto_dssi 2
#define boot$k_proto_mop 3
#define boot$k_proto_bootp 4
#define boot$k_scsi_disk 0
#define boot$k_scsi_tape 1
#define boot$k_ni 2
#define boot$k_bus_a 0
#define boot$k_bus_b 1
