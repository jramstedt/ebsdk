#define LOG_RTN		1

#define LOG_RTN_RQ_AVAIL 	(void *)1
#define LOG_RTN_DRIVER_GET_	(void *)2
#define LOG_RTN_PUT_		(void *)3
#define LOG_RTN_DO_SEND		(void *)4
#define LOG_RTN_INSERT_ENTRY	(void *)5
#define LOG_RTN_DRIVER_INSERT	(void *)6
#define LOG_RTN_DRIVER_REMOVE	(void *)7
#define LOG_RTN_ADAPTER_REMOVE	(void *)8
#define LOG_RTN_DO_SEND_DONE	(void *)9
#define LOG_RTN_DRIVER_PUT_	(void *)0xA

#define LOG_QP 		2
#define LOG_STOPPER	3
#define LOG_CARRIER	4
#define LOG_Q_BUF	5
#define LOG_POST	6

#define LOG_DEBUG_ENABLED 1
#define CDEBUG(c,x) if ( LOG_DEBUG_ENABLED ) { if ( \
		 pkz_debug_flags & ( 1 << c ) ) { x } }

/* bit 0 == make unrecognized command */
/* bit 1 == only poll one scsi device */
/* bit 2 == only init first adapter */
/* bit 3 == print trace information */
/* bit 4 == one response per interrupt */
/* bit 5 == log buffer calls */

typedef unsigned int  BASE_ITEM;

typedef volatile struct log_item {
  int                 flag;
  unsigned short int  item_type;
  unsigned short int  item_size;
  unsigned int        log_num;
  void               *item_ptr;
  BASE_ITEM           item;
} LOG_ITEM;

typedef volatile struct log_buffer {
  char                name[32];
  struct SEMAPHORE    sem;
  unsigned int        log_num;
  unsigned int        bufsize;
  LOG_ITEM           *nextbuf, 
                     *lastbuf, 
                     *buf;
} LOG_BUFFER;

init_log( LOG_BUFFER **log_buf, int size, char *name );
void log_item( LOG_BUFFER *lb, unsigned short int item_type, 
	      void *item_ptr, unsigned short int item_size );
