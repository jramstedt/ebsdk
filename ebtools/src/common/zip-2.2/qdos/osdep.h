#ifndef _QDOS_OPDEP
#define _QDOS_OPDEP

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

char * ql2Unix(char *);
char * Unix2ql(char *, char **);
int wild (char *);
char *LastDir(char *);
void QDOSexit(void);
short devlen(char *);

/*
 * XXX NO_RENAME instead of the following define ?
 */
#define link rename
#define USE_CASE_MAP
#define USE_EF_UT_TIME
#define PROCNAME(n) (action == ADD || action == UPDATE ? wild(n) : procname(n))
#endif
