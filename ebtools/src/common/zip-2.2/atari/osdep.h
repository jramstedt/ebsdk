#define FOPR "rb"
#define FOPM "r+b"
#define FOPW "wb"

#define DIRENT
#define NO_TERMIO
#define USE_CASE_MAP
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))

#include <sys/types.h>
#include <sys/stat.h>
