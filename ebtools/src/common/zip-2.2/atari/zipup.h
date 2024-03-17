#ifndef O_RDONLY
#  define O_RDONLY 0
#endif
#define fhow O_RDONLY
#define fbad (-1)
typedef int ftype;
#define zopen(n,p) open(n,p)
#define zread(f,b,n) read(f,b,n)
#define zclose(f) close(f)
#define zerr(f) (k == (extent)(-1L))
#define zstdin 0
