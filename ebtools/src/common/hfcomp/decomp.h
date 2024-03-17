#define FALSE 0
#define TRUE 1

#if defined (__alpha) && defined (__osf__)
typedef unsigned int ULONG;
typedef int LONG;
typedef int INT;
typedef unsigned int UINT;
typedef ULONG *PULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

extern INT verbose;
extern LONG compressedSize;
extern LONG decompressedSize;
#else
typedef unsigned long ULONG;
typedef long LONG;
typedef long INT;
typedef unsigned long UINT;
typedef ULONG *PULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

extern INT verbose;
extern LONG compressedSize;
extern LONG decompressedSize;
#endif

extern PUCHAR compressed;
extern PUCHAR decompressed;

extern PUCHAR inptr;
extern PUCHAR outptr;

extern INT bits_left;

#define MAX_BITS      13                 /* used in unShrink() */
#define HSIZE         (1 << MAX_BITS)    /* size of global work area */


int inflate (void);

#ifdef ROM

#define slide area->Slide
typedef int size_t;

/* #define NULL (char *)0 */

/*
//
// For ROM image, define the basic address scheme
//
*/

#define _1MEG 0x100000
#define _32K 0x8000
#define _16K 0x4000
#define _8K 0x2000
#define _4K 0x1000

#define KSEG0_BASE 0x80000000

#define BASE_OF_DECOMPRESSED_IMAGE (_8K)

#if 1
#define BASE_OF_HEAP ((_1MEG * 3) - (64+32+32)*1024)
#else
extern unsigned int BASE_OF_HEAP;
#endif

/*
//
// define add extra code into ROM space
//
*/

#define huft_free(x) 
#define fprintf(x,y)

/* functional prototypes for memory routines */

void *malloc(size_t size);
void free(void *dummy);
void *memcpy(void *d, void const *s, size_t size);
void *memset(void *d, int Char, size_t size);

#else

#define slide area.Slide

#endif

