/*
 * Minimal IP header for use with bootp and tftp
 * see RFC
 */
typedef struct{
  unsigned char ip_hl : 4;		/* Header Length */         
  unsigned char ip_v  : 4;		/* Version */               
  unsigned char ip_tos;			/* Type of service */       
  short ip_len;  			/* Total length */          
  unsigned short ip_id;			/* Identification */        
  short  ip_off;	         	/* Fragment offset field */ 
  unsigned char ip_ttl;			/* Time to live */          
  unsigned char ip_p;			/* Protocol */              
  unsigned short ip_sum;		/* Checksum */              
  unsigned int   ip_src;		/* IP source address */     
  unsigned int   ip_dst;		/* IP destination address */
} ip_hdr_t;

#define DODIP_PROTOCOL 0x0800
#define ARP_PROTOCOL   0x0806
#define IP_DF 0x4000		/* Don't fragment flag */
#define IP_MF 0x2000		/* More fragments flag */
#define IP_FRAGS 0x3fff         /* frag offset | more frags */
#define IPVERSION 4
#define STDTTL 32               /* recommended value; see RFC 1060 p 23 */

#define IP_UDP_PROTOCOL 17
#define IP_ICMP_PROTOCOL 1
#define INADDR_BROADCAST -1

#if 0
#define TFTP_RETRY 4            /* seconds before retrying a transmit */
#define TFTP_MAX_RETRIES 15     /* timeout after 1 minute */
#define ARP_RETRY 4
#define ARP_MAX_RETRIES 7
#endif

#define TFTP_DEF_TRIES 3        /* try for about 12 seconds */
#define BOOTP_DEF_TRIES 3       /* try for about 12 seconds */
#define ARP_DEF_TRIES 3         /* try for about 12 seconds */

/*
 * UDP header
 */
typedef struct {
  unsigned short udp_sport;	/* Source port */
  unsigned short udp_dport;	/* Destination port */
  unsigned short udp_len;	/* UDP length */
  unsigned short udp_sum;	/* UDP checksum */
} udp_hdr_t;

/*
 * UDP pseudo-header
 */
typedef union {
  struct {
    long src_iaddr;
    long dst_iaddr;
    char zero;
    char proto;
    unsigned short len;
  } st;
  unsigned short shorts[6];
} udp_pseudo_hdr_t;

/*
 * packet format for bootp
 * see RFC 951
 */
typedef struct {
  char op;		/*   1 byte   --  1=BOOTREQUEST, 2=BOOTREPLY */
  char htype;		/*   1 byte   --  hardware address type; see ARP */
			                /* section in assigned numbers */
  char hlen;		/*   1 byte   --  hardware address length */
  char hops;		/*   1 byte   --  client=0; used by gateways */
  long xid;		/*   4 bytes  --  transaction id (random number) */
  short secs;		/*   2 bytes  --  secs since client started booting */
  short unused;		/*   2 bytes */
  long ciaddr;		/*   4 bytes  --  client IP address */
  long yiaddr;		/*   4 bytes  --  'your' (client) address */
  long siaddr;		/*   4 bytes  --  server IP address */
  long giaddr;		/*   4 bytes  --  gateway IP address */
  char chaddr[16];	/*  16 bytes  --  client hardware address */
  char sname[64];	/*  64 bytes  --  server host name (null term str)*/
  char file[128];	/* 128 bytes  --  boot file name (null term str) */
  char vend[64];	/*  64 bytes  --  vendor specific options */
} bootp_pkt_t;

#define BOOTPREQUEST  1
#define BOOTPREPLY    2
#define HTYPE_ETHER10 1
#define HLEN_ETHER10  6
#define HTYPE_IEEE8025 6		/* Token Ring: IEEE 802.5 */
#define PLEN_DODIP 4

#define TFTPRRQ   1
#define TFTPWRQ   2
#define TFTPDATA  3
#define TFTPACK   4
#define TFTPERROR 5

#define ARP_REQUEST   1
#define ARP_RESPONSE  2
#define RARP_REQUEST  3
#define RARP_RESPONSE 4

#define BOOTPSERVER 67          /* well known port numbers - RFC 1060 p 10 */
#define BOOTPCLIENT 68
#define TFTPSERVER 69

#define htons(x) ((((x)<<8)&0xff00)|(((x)>>8)&0xff))
#define htonl(x) ((((x)<<24)&0xff000000)|\
                 (((x)<< 8)&0x00ff0000)|\
                 (((x)>> 8)&0x0000ff00)|\
                 (((x)>>24)&0x000000ff))
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)

/*
 * per-interface data base
 * one entry for each network interface we have been fopened on top of
 */
typedef struct pidb_struct {         /* per-interface data base */
  struct pidb_struct *flink;
  struct pidb_struct *blink;
  struct INODE *ip;      /* inode pointer for interface */
  int inited;            /* if zero then entry requires init */
  bootp_pkt_t bootp;     /* bootp packet containing  hw addr, ip addr, etc */
} pidb_t;

/*
 * bootp per-request information
 * one of these for each open bootp
 */
typedef struct bp_struct {
  struct bp_struct *flink;
  struct bp_struct *blink;
  struct FILE *fp;       /* fp to bootp */
  pidb_t *pie;           /* pidb entry for interface we are using */
  long xid;              /* transaction id of bootp broadcast */
  struct TIMERQ tq;      /* timer queue entry to sleep on */
  char *offset;          /* pointer into pidb for read/write */
  int size;              /* remaining bytes for read/write */
} bootp_t;

/*
 * tftp per-request information
 * one of these for each open tftp
 */
typedef struct tftp_struct {
  struct tftp_struct *flink;
  struct tftp_struct *blink;
  struct FILE *fp;                /* fp to tftp */
  struct FILE *net;               /* network we're sitting above */
  pidb_t *pie;                    /* pidb for network */
  int direction;                  /* read == 1, write = 2 */
  unsigned short xid;             /* unique trans id, also local udp port */
  unsigned short rem_port;        /* remote udp port */
  long rem_iaddr;                 /* ip addr we're talking to */
  int block;                      /* current block number */
  char *pkt;                      /* current buffer packet */
  char *offset;                   /* next byte in buffer packet */
  int size;                       /* bytes remaining in buffer packet */
  int last;                       /* TRUE if pkt is last (had < 512 bytes) */
  int error;                      /* tftp error code + 1 on error; o/w 0 */
  int tries;                      /* number of attempts to transmit packets */
  struct TIMERQ tq;
} tftp_t;

typedef struct {
  unsigned short op;
  unsigned short param;
} tftp_hdr_t;

typedef struct {
  unsigned short op;
  unsigned short block;
  char data[512];
} tftp_data_t;

typedef struct {
  unsigned short op;
  unsigned short error;
} tftp_error_t;

typedef struct {
  unsigned short op;
  unsigned short block;
} tftp_ack_t;
  
typedef struct {
  int valid;
  long iaddr;
  char haddr[16];
} arp_cache_t;

/*
 * arp packets - hardwired for ethernet
 */
typedef struct {
  unsigned short htype;
  unsigned short protocol;
  char hlen;
  char plen;
  unsigned short op;
  char src_haddr[6];
  long src_iaddr;
  char dst_haddr[6];
  long dst_iaddr;
} arp_pkt_t;

/*
 * iovec for passing data buffers
 */

#define MAX_IOVEC 6
typedef struct {
  void *iov_base;
  int iov_len;
} iovec_t;
