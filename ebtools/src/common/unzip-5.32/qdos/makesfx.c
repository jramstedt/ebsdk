/*
 * makesfx - Makes a QDOS sfx zip file
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * not copyrighted at all by Jonathan Hudson, 04/09/95
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef QDOS
# include <qdos.h>
# define ZMODE (X_OK|R_OK)
#else
# define ZMODE (R_OK)
# define getchid(p1) p1

typedef struct
{
        long id;
        long dlen;
} NTC;

struct qdirect  {
    long            d_length __attribute__ ((packed));  /* file length */
    unsigned char   d_access __attribute__ ((packed));  /* file access type */
    unsigned char   d_type __attribute__ ((packed));    /* file type */
    long            d_datalen __attribute__ ((packed)); /* data length */
    long            d_reserved __attribute__ ((packed));/* Unused */
    short           d_szname __attribute__ ((packed));  /* size of name */
    char            d_name[36] __attribute__ ((packed));/* name area */
    long            d_update __attribute__ ((packed));  /* last update */
    long            d_refdate __attribute__ ((packed));
    long            d_backup __attribute__ ((packed));   /* EOD */
    } ;

int fs_headr (int fd, long t, struct qdirect *qs, short size)
{
    NTC ntc;
    int r = -1;

    lseek(fd, -8, SEEK_END);
    read(fd, &ntc, 8);
    if(ntc.id == *(long *)"XTcc")
    {
        qs->d_datalen = ntc.dlen;    /* This is big endian */
        qs->d_type = 1;
        r = 0;
    }
    lseek(fd, 0, 0);
    return 42;                       /* why not ??? */
}

void fs_heads (int fd, long t, struct qdirect *qs, short size)
{
    NTC ntc;

    read(fd, &ntc, 8);
    ntc.id = *(long *)"XTcc";
    ntc.dlen = qs->d_datalen;
    write (fd, &ntc, 8);
}

#endif


#define RBUFSIZ 4096

int main (int ac, char **av)
{
    int fd;
    static char local_sig[4] = "PK\003\004";
    char *p, tmp[4];
    short ok = 0;

    if(ac != 3)
    {
        fputs("makesfx unzipsfx zip_file\n", stderr);
        exit (0);
    }
    if(isatty(1))
    {
        fputs("makesfx: writing sfx to terminal is TERMINAL !\n", stderr);
        exit (0);
    }

    if((fd = open(*(av+2), O_RDONLY)) > 0)
    {
        if((read(fd, tmp, 4) == 4))
        {
            if(*(long *)tmp == *(long *)local_sig)
            {
                ok = 1;
            }
        }
        close(fd);
    }
    if(!ok)
    {
        fprintf(stderr,
                "Huum, %s doesn't look like a ZIP file to me\n", *(av+2));
        exit(0);
    }

    if(strstr(*(av+1), "unzipsfx"))
    {
        if(access(*(av+1), ZMODE))
        {
            fprintf(stderr, "Sorry, don't like the look of %s\n", *(av+1));
            exit(0);
        }
    }

    if(p = malloc(RBUFSIZ))
    {
        struct qdirect qd;
        int n;

        if((fd = open(*(av+1), O_RDONLY)))
        {
            if(fs_headr(getchid(fd), -1, &qd, sizeof(qd)) > 0)
            {
                while((n = read(fd, p, RBUFSIZ)) > 0)
                {
                    write(1, p, n);
                }
                close(fd);
                if((fd = open(*(av+2), O_RDONLY)) > 0)
                {
                    while((n = read(fd, p, RBUFSIZ)) > 0)
                    {
                        write(1, p, n);
                    }
                    close(fd);
                }
                fs_heads(getchid(1), -1, &qd, sizeof(qd));
            }
            else
            {
                close(fd);
                fputs("Can't read unzipsfx header", stderr);
                exit(0);
            }
        }
        free(p);
    }
    else
    {
        fputs("Out of memory", stderr);
        exit(0);
    }
    return 0;
}
