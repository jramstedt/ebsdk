#define LOOP_INTERNAL_KEYWORD "internal"
#define LOOP_EXTERNAL_KEYWORD "external"

#define LOOP_PROTOCOL 0xffff
#define LOOP_MAX_QUEUED_REPLIES 100

struct LOOP_BLOCK
{
    struct QUEUE rq;			/* Reply queue */
    struct SEMAPHORE rq_s;		/* Synchronization semaphore */
    int rq_elements;			/* Number of elements in reply queue */
};

struct LOOP_MSG_HDR
{
    int receipt_number;
};
