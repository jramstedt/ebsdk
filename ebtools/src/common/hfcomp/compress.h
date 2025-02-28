#define FALSE 0
#define TRUE 1

/* typedef int size_t; */
#if defined (__alpha) && defined (__osf__)
typedef unsigned int ULONG;
typedef int LONG;
typedef int INT;
typedef unsigned int UINT;
#elif __LP64__
typedef unsigned int ULONG;
typedef int LONG;
typedef int INT;
typedef unsigned int UINT;
#else
typedef unsigned long ULONG;
typedef long LONG;
typedef long INT;
typedef unsigned long UINT;
#endif
typedef ULONG *PULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

#if 0
extern INT verbose ;
extern FILE *objIn;
extern FILE *objOut;
#endif

#define WSIZE (32768u)

#define MIN_MATCH  3
#define MAX_MATCH  258

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)

#define UNKNOWN (-1) 
#define DEFLATE 8

#define STORE 0
#define BINARY 0

#define ASCII 1

#ifdef DEBUG
#define Assert(x,y) if (x) {fprintf(stderr, y);fprintf(stderr,"\n");}
#define Trace(x) fprintf x; fprintf(stderr, "\n")
#define Tracec(x,y) if (x) {fprintf y;fprintf(stderr,"\n");}
#define Tracev(x) fprintf x; fprintf(stderr, "\n")
#define Tracecv(x,y) if (x) {fprintf y;fprintf(stderr,"\n");}
#define Tracevv(x) fprintf x;
#else
#define Assert(x,y)
#define Trace(x)
#define Tracec(x,y)
#define Tracev(x)
#define Tracecv(x,y)
#define Tracevv(x)
#endif

extern	int verbose;

/* function protos */

extern	void send_bits(int value,int length);
extern	unsigned int bi_reverse(unsigned int code,int len);
extern	void bi_windup(void);
extern	void copy_block(char *buf,unsigned int len,int header);
extern	ULONG memcompress(unsigned char *tgt,
    ULONG tgtsize,
    unsigned char *src,
    ULONG srcsize);
extern	int mem_read(char *buf,unsigned int size);
extern	void lm_init(void);
extern	int longest_match(ULONG cur_match);
extern	ULONG deflate(void);
extern	void compress_start(void);
extern	void compress_stop(void);
extern	int compress(FILE *in,FILE *out);
extern	int main(int argc,char **argv );
extern	int next_element(char **s ,char *fieldsep,char *result);
extern	int classify_specifier(char *id);
extern	int substr_nocase(char *s,char *t);
extern	void ct_init(void);
extern	ULONG flush_block(char *buf,ULONG stored_len,int eof);
extern	int ct_tally(int dist,int lc);

